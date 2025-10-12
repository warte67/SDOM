// SDOM_Core.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_SDL_Utils.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDL3_ttf/SDL_ttf.h>

#include <SDOM/SDOM_Utils.hpp> // for parseColor
#include <SDOM/lua_Core.hpp>
#include <SDOM/lua_BindHelpers.hpp>
#include <algorithm>
#include <filesystem>

namespace SDOM
{

    Core::Core() : IDataObject()
    {
        // Open base libraries plus package and io so embedded scripts can use
        // `require`/`package` and basic I/O if desired by examples.
        lua_.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::package, sol::lib::io);

        // Register Core and SDL_Utils usertypes
        SDL_Utils::registerLua(lua_);
        factory_ = new Factory();
        eventManager_ = new EventManager();
        
        // register the DisplayHandle handle last so other types can use it
        DisplayHandle prototypeHandle; // Default DisplayHandle for registration
        prototypeHandle._registerLuaBindings("DisplayHandle", lua_);

        AssetHandle prototypeAssetHandle; // Default AssetHandle for registration
        prototypeAssetHandle._registerLuaBindings("AssetHandle", lua_);

        // Note: Factory initialization is performed later (e.g. during
        // configuration) to avoid recursive-construction ordering issues.
    }

    Core::~Core()
    {
        if (factory_)
        {
            delete factory_;
            factory_ = nullptr;
        }
        if (eventManager_)
        {
            delete eventManager_;
            eventManager_ = nullptr;
        }
    }

    void Core::configure(const CoreConfig& config)
    {
        // initialize or reconfigure SDL as needed
        reconfigure(config);
        // Initialize the Factory if it hasn't been initialized yet.
        if (factory_ && !factory_->isInitialized()) {
            factory_->onInit();
        }
    }

    void Core::configureFromLua(const sol::table& lua)
    {
        CoreConfig config;
        sol::table coreObj = lua;
        config.windowWidth = coreObj["windowWidth"].get_or(1280.0f);
        config.windowHeight = coreObj["windowHeight"].get_or(720.0f);
        config.pixelWidth  = coreObj["pixelWidth"].get_or(2.0f);
        config.pixelHeight = coreObj["pixelHeight"].get_or(2.0f);
        config.preserveAspectRatio = coreObj["preserveAspectRatio"].get_or(true);
        config.allowTextureResize = coreObj["allowTextureResize"].get_or(true);
        config.rendererLogicalPresentation = SDL_Utils::rendererLogicalPresentationFromString(
            coreObj["rendererLogicalPresentation"].get_or(std::string("SDL_LOGICAL_PRESENTATION_LETTERBOX")));
        config.windowFlags = SDL_Utils::windowFlagsFromString(
            coreObj["windowFlags"].get_or(std::string("SDL_WINDOW_RESIZABLE")));
        config.pixelFormat = SDL_Utils::pixelFormatFromString(
            coreObj["pixelFormat"].get_or(std::string("SDL_PIXELFORMAT_RGBA8888")));
        if (coreObj["color"].valid()) {
            config.color = parseColor(coreObj["color"]);
        } else {
            config.color = {0, 0, 0, 255};
        }        
        configure(config);

        // Recursive resource creation
        std::function<void(sol::table, DisplayHandle)> createResourceRecursive;
        createResourceRecursive = [&](sol::table obj, DisplayHandle parent) 
        {
            std::string type = obj["type"].get_or(std::string(""));
            std::string name = obj["name"].get_or(std::string(""));
            DisplayHandle handle;
            if (!type.empty()) 
            {
                // DEBUG_LOG("Core::configureFromLua: creating type='" + type + "' name='" + name + "'");
                handle = factory_->create(type, obj);
                if (!handle.isValid()) {
                    ERROR("Core::configureFromLua: factory->create returned invalid handle for type '" + type + "' name='" + name + "'");
                } 
                // else 
                // {
                //     DEBUG_LOG("Core::configureFromLua: created handle for '" + type + "' name='" + name + "'");
                // }
                if (parent.isValid() && handle.isValid()) 
                {
                    parent.get()->addChild(handle);
                }
            }
            if (obj["children"].valid() && obj["children"].is<sol::table>()) 
            {
                sol::table children = obj["children"];
                for (auto& kv : children) 
                {
                    sol::table child = kv.second.as<sol::table>();
                    createResourceRecursive(child, handle);
                }
            }
        };

        // Parse children and create resources
        if (coreObj["children"].valid() && coreObj["children"].is<sol::table>()) 
        {
            sol::table children = coreObj["children"];
            for (auto& kv : children) 
            {
                sol::table child = kv.second.as<sol::table>();
                createResourceRecursive(child, DisplayHandle());
            }
        }

        // // Debug: print factory registries so we can see registered/created types
        // try {
        //     factory_->printObjectRegistry();
        //     factory_->printAssetRegistry();
        // } catch(...) {}

        // Set the "mainStage" as the root node
        std::string rootStageName = "mainStage";
        if (coreObj["rootStage"].valid())
            rootStageName = coreObj["rootStage"].get<std::string>();
        rootNode_ = factory_->getDisplayObject(rootStageName);
        setWindowTitle("Stage: " + rootStageName);
    }

    void Core::configureFromLuaFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) 
        {
            ERROR("Could not open configuration file: " + filename);
            return;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();

        // Execute the configuration file in the Core's Lua state so that
        // globals registered by the host (Core, CLR, unit-test functions)
        // are available to the script.
        try {
            // Ensure Core usertype is registered into this lua state
            // this->_registerLua("Core", lua_);
            this->_registerLuaBindings("Core", lua_);

            // Prefer scripts that return the config table. If the chunk
            // returns a table, use it. Otherwise fall back to a global
            // `config` table for backward compatibility.
            sol::load_result chunk = lua_.load(buffer.str());
            if (!chunk.valid()) {
                sol::error err = chunk;
                ERROR(std::string("Failed to load Lua config chunk: ") + err.what());
                return;
            }

            sol::protected_function_result result = chunk();
            if (!result.valid()) {
                sol::error err = result;
                ERROR(std::string("Error executing Lua config chunk: ") + err.what());
                return;
            }

            // If the chunk returned a table, use that as the config.
            sol::object ret = result.get<sol::object>();
            // If the script overwrote the global `Core` (common when authors
            // set a config table into Core), restore our forwarding table so
            // callbacks and later code see the table-based API. The
            // _registerLuaBindings() created CoreForward.
            try {
                if (lua_["CoreForward"].valid()) {
                    lua_["Core"] = lua_["CoreForward"];
                }
            } catch (...) {}
            if (ret.is<sol::table>()) {
                sol::table configTable = ret.as<sol::table>();
                // Defensive: scripts may call the global `configure(table)`
                // themselves and also return the table. To avoid double-apply
                // (which can lead to duplicate-named object errors), detect
                // whether the returned table's rootStage has already been
                // created and skip re-applying if so.
                std::string rootStageName = "mainStage";
                try {
                    if (configTable["rootStage"].valid())
                        rootStageName = configTable["rootStage"].get<std::string>();
                } catch(...) {}

                bool alreadyApplied = false;
                try {
                    if (factory_) {
                        DisplayHandle existing = factory_->getDisplayObject(rootStageName);
                        if (existing.isValid()) alreadyApplied = true;
                    }
                } catch(...) { alreadyApplied = false; }

                if (alreadyApplied) {
                    INFO(std::string("Lua config file appears to have already applied configuration for rootStage '") + rootStageName + "'; skipping duplicate configure().");
                    return;
                }

                configureFromLua(configTable);
                return;
            }

            // Fall back to global `config` table
            sol::table configTable = lua_["config"];
            if (!configTable.valid()) {
                ERROR("Lua config file did not produce a valid 'config' table (no return value and global 'config' missing).");
                return;
            }
            configureFromLua(configTable);
        } catch (const sol::error& e) {
            ERROR(std::string("Error executing Lua config file: ") + e.what());
            return;
        }
    }

    void Core::shutdown_SDL()
    {
        if (texture_) {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }
        if (renderer_) {
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
        }
        if (window_) {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }
        // Shutdown SDL_ttf if initialized
        if (TTF_WasInit()) {
            TTF_Quit();
        }
        SDL_Quit();
    }


    bool Core::run(const sol::table& config)
    {
        configureFromLua(config);
        // Now run normally
        return run();
    }

    bool Core::run(const CoreConfig& config)
    {
        // Apply the provided configuration synchronously and then start the main loop.
        configure(config);
        return run();
    }


    bool Core::run(const std::string& configFile)
    {
        if (!configFile.empty()) {
            // Prefer an explicit existence check (robust against debugger CWD differences)
            namespace fs = std::filesystem;
            bool fileExists = false;
            try {
                fileExists = fs::exists(fs::path(configFile));
            } catch (...) {
                fileExists = false;
            }

            if (fileExists) {
                configureFromLuaFile(configFile);
            } else {
                // Heuristic: only treat the string as raw Lua source if it clearly *looks* like code.
                // Otherwise assume the caller meant a filename and report a clear error.
                bool looksLikeCode = (configFile.find('\n') != std::string::npos)
                                    || (configFile.find("return") != std::string::npos)
                                    || (configFile.find("function") != std::string::npos)
                                    || (configFile.find("local") != std::string::npos)
                                    || (configFile.rfind(".lua") == configFile.size() - 4 && configFile.size() >= 4);

                // If the string contains path separators but the file doesn't exist, give a helpful error
                bool containsPathSep = (configFile.find('/') != std::string::npos) || (configFile.find('\\') != std::string::npos);

                if (!looksLikeCode && containsPathSep) {
                    ERROR(std::string("Config file not found: ") + configFile);
                    return false;
                }

                // If it doesn't look like code and no path separators, treat as missing file rather than raw script
                if (!looksLikeCode) {
                    ERROR(std::string("Config string does not appear to be Lua code and no file found: ") + configFile);
                    return false;
                }

                // Treat as inline Lua code
                // this->_registerLua("Core", lua_);
                this->_registerLuaBindings("Core", lua_);
                try {
                    sol::load_result chunk = lua_.load(configFile);
                    if (!chunk.valid()) {
                        sol::error err = chunk;
                        ERROR(std::string("Failed to load Lua config chunk: ") + err.what());
                        return false;
                    }
                    sol::protected_function_result result = chunk();
                    if (!result.valid()) {
                        sol::error err = result;
                        ERROR(std::string("Error executing Lua config chunk: ") + err.what());
                        return false;
                    }
                    sol::object ret = result.get<sol::object>();
                    // Restore Core global to CoreForward (if present) so that
                    // subsequent code and callbacks use the forwarding table
                    // instead of any config table the script may have assigned.
                    try {
                        if (lua_["CoreForward"].valid()) {
                            lua_["Core"] = lua_["CoreForward"];
                        }
                    } catch (...) {}
                    if (ret.is<sol::table>()) {
                        configureFromLua(ret.as<sol::table>());
                    } else {
                        sol::table configTable = lua_["config"];
                        if (configTable.valid()) {
                            configureFromLua(configTable);
                        } else {
                            ERROR("Lua script did not produce a valid 'config' table.");
                        }
                    }
                } catch (const sol::error& e) {
                    ERROR(std::string("Error executing Lua script for configuration: ") + e.what());
                }
            }
        }
        return run();
    }

    void Core::reconfigure(const CoreConfig& config)
    {
        // Note: reconfigure should be called from the main thread. This
        // function is not thread-safe; consider adding a deferred-apply
        // mechanism if configs may be changed from worker threads.

        // Resource recreation matrix (CoreConfig -> SDL resource effect)

        // Config field                | Window    | Renderer  | Texture  | Notes
        // ----------------------------+-----------+-----------+----------+---------------------------------------------------------
        // windowWidth                 | Recreate  | Recreate  | Recreate | Immediate: window/renderer/texture rebuilt
        // windowHeight                | Recreate  | Recreate  | Recreate | Immediate
        // windowFlags                 | Recreate  | Recreate  | Recreate | Immediate
        // rendererLogicalPresentation | No change | Recreate  | Recreate | Renderer/texture recreated to apply presentation
        // pixelWidth                  | No change | No change | Recreate | Texture recreated (pixel sizing)
        // pixelHeight                 | No change | No change | Recreate | Texture recreated (pixel sizing)
        // pixelFormat                 | No change | No change | Recreate | Texture recreated (pixel format)

        // Legend:
        // - "Recreate" = Core will destroy and recreate that SDL resource immediately when the config changes.
        // - "No change" = Core does not recreate that resource for this field.
        // - "No change (flag only)" = Core updates the config flag but does not immediately change any SDL resource; the new flag value only affects behavior when the window is next resized (so the actual texture/recreate work happens on/after resize).


        // reconfigure the SDL resources based on what is being changed
        bool windowWidth_changed = (config_.windowWidth != config.windowWidth);
        bool windowHeight_changed = (config_.windowHeight != config.windowHeight);
        bool windowFlags_changed = (config_.windowFlags != config.windowFlags);
        bool rendererLogicalPresentation_changed = (config_.rendererLogicalPresentation != config.rendererLogicalPresentation);
        bool pixelWidth_changed = (config_.pixelWidth != config.pixelWidth);
        bool pixelHeight_changed = (config_.pixelHeight != config.pixelHeight);
        bool pixelFormat_changed = (config_.pixelFormat != config.pixelFormat);

        // compute each recreation flag independently (clearer and no
        // accidental masking of combined changes)
        bool recreate_window = (windowWidth_changed || windowHeight_changed || windowFlags_changed);
        bool recreate_renderer = recreate_window || rendererLogicalPresentation_changed;
        bool recreate_texture = recreate_window || rendererLogicalPresentation_changed || pixelWidth_changed || pixelHeight_changed || pixelFormat_changed;

        // update config_
        config_.windowWidth = config.windowWidth;
        config_.windowHeight = config.windowHeight;
        config_.windowFlags = config.windowFlags;
        config_.rendererLogicalPresentation = config.rendererLogicalPresentation;
        config_.pixelWidth = config.pixelWidth;
        config_.pixelHeight = config.pixelHeight;
        config_.pixelFormat = config.pixelFormat;
        config_.preserveAspectRatio = config.preserveAspectRatio;
        config_.allowTextureResize = config.allowTextureResize;

        // -- initialize or reconfigure SDL resources as needed -- //
        if (!SDL_WasInit(SDL_INIT_VIDEO)) 
        {
            if (!SDL_Init(SDL_INIT_VIDEO)) 
            {
                std::string errorMsg = "SDL_Init() Error: " + std::string(SDL_GetError());
                ERROR(errorMsg);
                return;
            }
            // Initialize SDL_ttf for TrueType font support; if it fails
            // record the error in debug logs. Actual font loads will
            // produce clear errors if ttf is unavailable.
            if (!TTF_Init()) {
                DEBUG_LOG(std::string("TTF_Init() failed: ") + SDL_GetError());
            }
        }

        // destroy in reverse order
        if (recreate_texture && texture_) {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }
        if (recreate_renderer && renderer_) {
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
        }
        if (recreate_window && window_) {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }

        // recreate in normal order
        if (recreate_window && !window_) {
            window_ = SDL_CreateWindow(getWindowTitle().c_str(), config.windowWidth, config.windowHeight, config.windowFlags);
            if (!window_) 
            {
                std::string errorMsg = "SDL_CreateWindow() Error: " + std::string(SDL_GetError());
                ERROR(errorMsg);
            }
            SDL_ShowWindow(window_);     // not needed in SDL3, but included for clarity
        }
        if (recreate_renderer && !renderer_) 
        {
            // SDL_Log("Available renderer drivers:");
            // for (int i = 0; i < SDL_GetNumRenderDrivers(); i++) {
            //     SDL_Log("%d. %s", i + 1, SDL_GetRenderDriver(i));
            // }            
            renderer_ = SDL_CreateRenderer(window_, nullptr);
            if (!renderer_) {
                std::string errorMsg = "SDL_CreateRenderer() Error: " + std::string (SDL_GetError());
                ERROR(errorMsg);
            }
        }
        if (recreate_texture && !texture_) {
            // compute texture size safely (avoid divide-by-zero and ensure >=1)
            int tWidth = 1;
            int tHeight = 1;
            if (config.pixelWidth != 0.0f) tWidth = std::max(1, static_cast<int>(config.windowWidth / config.pixelWidth));
            if (config.pixelHeight != 0.0f) tHeight = std::max(1, static_cast<int>(config.windowHeight / config.pixelHeight));

            texture_ = SDL_CreateTexture(renderer_,
                config.pixelFormat,
                SDL_TEXTUREACCESS_TARGET,
                tWidth,
                tHeight);
            if (!texture_) {
                std::string errorMsg = "SDL_CreateTexture() Error: " + std::string(SDL_GetError());
                ERROR(errorMsg);
            }
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);   
            SDL_SetRenderLogicalPresentation(renderer_, tWidth, tHeight, config.rendererLogicalPresentation);
        }
        sdlStarted_ = true;
    }

    void Core::requestConfigApply(const CoreConfig& cfg)
    {
        std::lock_guard<std::mutex> lock(pendingConfigMutex_);
        pendingConfig_ = cfg;
        pendingConfigRequested_.store(true, std::memory_order_release);
    }

    void Core::applyPendingConfig()
    {
        if (!pendingConfigRequested_.load(std::memory_order_acquire))
            return;
        CoreConfig cfg;
        {
            std::lock_guard<std::mutex> lock(pendingConfigMutex_);
            cfg = pendingConfig_;
            pendingConfigRequested_.store(false, std::memory_order_release);
        }
        // Apply on main thread
        reconfigure(cfg);
    }

    bool Core::run()
    {
        // std::cout << "Core::run()" << std::endl;

        bool overallSuccess = true;
        try
        {
            static bool runWasStarted = false;
            if (runWasStarted) {
                // just silently return if called a second time.  Recursion here would be inapproprite.
                // ERROR("Core::run() Error: run() has already been called once. Multiple calls to run() are not allowed.");
                return true;
            }
            runWasStarted = true;

            // // Ensure SDL is initialized
            // startup_SDL();

            // register and initialize the factory object (after creating SDL resources)
            onInit();   // for now just call onInit(). Later Factory will call onInit() 
                        // for each object as it creates them.

            Stage* rootStage = dynamic_cast<Stage*>(rootNode_.get());
            if (!rootStage)
            {
                ERROR("Core::run() Error: Root node is null or not a valid Stage.");
                overallSuccess = false;
                onQuit();
                return false;
            }
            // else
            // {
            //     std::cout << "Core::run() Root node is a valid Stage: " << rootStage->getName() << std::endl;
            //     rootStage->printTree();
            // }

            // Now run user tests after initialization
            // SDL_Delay(250); // give some time for tiled window to be configured by the OS Compositor 

            // Temporarily ignore real mouse input while running unit tests
            this->setIgnoreRealInput(true);
            bool testsPassed = onUnitTest();
            this->setIgnoreRealInput(false);
            if (!testsPassed) 
            {
                ERROR("Unit tests failed. Aborting run.");
                overallSuccess = false;
                // stop the main loop to allow graceful shutdown
                bIsRunning_ = false;
            }

            SDL_Event event;

            // // Apply any pending configuration requested from other threads
            // applyPendingConfig();

            while (bIsRunning_) 
            {
                while (SDL_PollEvent(&event)) 
                {
                    // If configured to ignore real mouse input, drop any mouse events
                    if (this->getIgnoreRealInput()) {
                        // Drop real mouse and keyboard input while unit tests run so
                        // synthetic events are not interfered with by the user's input.
                        if (event.type == SDL_EVENT_MOUSE_MOTION || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP || event.type == SDL_EVENT_MOUSE_WHEEL
                            || event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP || event.type == SDL_EVENT_TEXT_INPUT || event.type == SDL_EVENT_TEXT_EDITING
                            || event.type == SDL_EVENT_WINDOW_FOCUS_GAINED || event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
                            // skip these events
                            continue;
                        }
                    }
                    // Global event handling
                    if (event.type == SDL_EVENT_QUIT) 
                        bIsRunning_ = false;

                    if (event.type == SDL_EVENT_WINDOW_RESIZED)
                    {
                        int newWidth, newHeight;
                        if (!SDL_GetWindowSize(window_, &newWidth, &newHeight))
                            ERROR("Unable to get the new window size: " + std::string(SDL_GetError()));
// INFO("Window resized to " << newWidth << "x" << newHeight);                            
                        onWindowResize(newWidth, newHeight);
                    }

                    // TEMPORARY: Use [Q] to quit globally
                        if (event.type == SDL_EVENT_KEY_DOWN) 
                            if (event.key.key == SDLK_Q) 
                                bIsRunning_ = false;
                    // END TEMPORARY              

                    // Handle and dispatch events based on the SDL_Event                
                    if (eventManager_) 
                    {
                        eventManager_->Queue_SDL_Event(event);
                        eventManager_->DispatchQueuedEvents();

                        // handle TAB keypress
                        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_TAB) 
                        {
                            if (event.key.mod & SDL_KMOD_SHIFT) 
                            {
                                // Shift + Tab: Move focus to the previous object
                                handleTabKeyPressReverse();
                            } 
                            else 
                            {
                                // Tab: Move focus to the next object
                                handleTabKeyPress();
                            }
                        }
                        // handle ESC keypress
                        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) 
                        {
                            keyboardFocusedObject_.reset(); // Clear keyboard focus
                        }
                    }

                    // Dispatch EventType::OnEvent to rootNode_ so global listeners can receive raw SDL_Events
                    if (eventManager_ && rootNode_)
                    {
                        DisplayHandle rootHandle = getStageHandle();

                        // dispatch OnEvent to rootNode_ so global listeners can inspect raw SDL_Events
                        auto ev = std::make_unique<Event>(EventType::OnEvent, rootHandle);
                        ev->setSDL_Event(event);
                        ev->setRelatedTarget(rootHandle);
                        eventManager_->dispatchEvent(std::move(ev), DisplayHandle(rootHandle.getName(), rootHandle.getType()));

                        // dispatch SDL_Event to rootNode_ so global listeners can inspect raw SDL_Events
                        auto sdl_ev = std::make_unique<Event>(EventType::SDL_Event, rootHandle);
                        sdl_ev->setSDL_Event(event);
                        sdl_ev->setRelatedTarget(rootHandle);
                        eventManager_->dispatchEvent(std::move(sdl_ev), DisplayHandle(rootHandle.getName(), rootHandle.getType()));
                    }

                }
                
                // frame timing
                static Uint64 lastTime = SDL_GetPerformanceCounter();
                Uint64 currentTime = SDL_GetPerformanceCounter();
                float fElapsedTime = static_cast<float>(currentTime - lastTime) / SDL_GetPerformanceFrequency();
                this->fElapsedTime_ = fElapsedTime;

                // update this stage and its children
                if (!renderer_) {
                    ERROR("Core::run() Error: Renderer is null.");
                }

                if (!texture_) {
                    ERROR("Core::run() Error: Texture is null.");
                }
                SDL_SetRenderTarget(renderer_, texture_); // set the render target to the proper background texture

                // Send Updates
                onUpdate(fElapsedTime);

                // render the stage and its children
                onRender();

                // present this stage
                if (renderer_ && texture_) 
                {
                    SDL_SetRenderTarget(renderer_, nullptr); // Reset to default target
                    SDL_RenderTexture(renderer_, texture_, NULL, NULL);
                    SDL_RenderPresent(renderer_);
                }

                // update timing
                lastTime = currentTime;

                factory_->detachOrphans();          // Detach orphaned display objects
                factory_->attachFutureChildren();   // Attach future children
                factory_->collectGarbage();         // Clean up any orphaned objects

                // static int s_iterations = 0;
                // if (s_iterations == 0)
                // {
                //     // Now run user tests after initialization
                //     bool testsPassed = onUnitTest();
                //     if (!testsPassed) 
                //     {
                //         ERROR("Unit tests failed. Aborting run.");
                //     }                    
                // }
                // s_iterations++;

            }  // END: while (SDL_PollEvent(&event)) 
        }

        catch (const SDOM::Exception& e) 
        {
            // Handle exceptions gracefully
            SDOM::printMessageBox("Exception Caught", e.what(), e.getFile(), e.getLine(), CLR::RED, CLR::WHITE, CLR::BROWN);
            overallSuccess = false;
        }
        catch (const std::exception& e) 
        {
            // Handle standard exceptions
            std::cerr << CLR::RED << "Standard exception caught: " << CLR::WHITE << e.what() << CLR::RESET << std::endl;
            std::string errStr = "Exception Caught: " + std::string(e.what());
            overallSuccess = false;
        }
        catch (...) 
        {
            // Handle unknown exceptions
            std::cerr << CLR::RED << "Unknown exception caught!" << CLR::RESET << std::endl;
            overallSuccess = false;
        }
        // properly shutdown
        onQuit();
        return overallSuccess;
    } // END: run()

    
    // Test helper: poll and dispatch any pending SDL events once. This mirrors
    // the event handling inside Core::run but returns immediately after one
    // pass. Useful for unit tests that push synthetic SDL events.
    void Core::pumpEventsOnce()
    {
        SDL_Event event;
        // // Apply any pending configuration requested from other threads
        // applyPendingConfig();
        if (!eventManager_)
            return;

        // std::cout << "Core::pumpEventsOnce(): Dispatching queued event. Remaining queue size: " << eventManager_->getEventQueueSize() << std::endl;
        while (SDL_PollEvent(&event))
        {
            if (this->getIgnoreRealInput()) {
                if (event.type == SDL_EVENT_MOUSE_MOTION || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP || event.type == SDL_EVENT_MOUSE_WHEEL
                    || event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP || event.type == SDL_EVENT_TEXT_INPUT || event.type == SDL_EVENT_TEXT_EDITING
                    || event.type == SDL_EVENT_WINDOW_FOCUS_GAINED || event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
                    continue;
                }
            }
            if (event.type == SDL_EVENT_QUIT)
                bIsRunning_ = false;

            if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                int newWidth, newHeight;
                if (!SDL_GetWindowSize(window_, &newWidth, &newHeight))
                    ERROR("Unable to get the new window size: " + std::string(SDL_GetError()));
                onWindowResize(newWidth, newHeight);
            }
            eventManager_->Queue_SDL_Event(event);
        }
            // std::cout << "POST -- Core::pumpEventsOnce(): Dispatching queued event. Remaining queue size: " << eventManager_->getEventQueueSize() << std::endl;
        while (eventManager_->getEventQueueSize() > 0)
        {
            eventManager_->DispatchQueuedEvents();  // should pop all queued events
        }
    }

    bool Core::onInit()
    {
        // Note: We do not need to run onInit() for each object here because
        // the Factory creates each object and calls onInit() as it does so.

        bool ret = true;

        // Expose CLR constants and helpers to Lua through a single helper
        // so updates to CLR are reflected in embedded Lua states.
        sol::state& lua = this->getLua(); // get the Lua state
        CLR::exposeToLua(lua);

        // Call the users registered init function if available
        if (fnOnInit)
            ret &= fnOnInit();

        return ret;
    }


    void Core::onQuit()
    {
        // Lambda for recursive quit handling using std::function
        std::function<void(IDisplayObject&)> handleQuit;
        handleQuit = [&handleQuit](IDisplayObject& node) 
        {
            node.onQuit();
            for (const auto& child : node.getChildren()) 
            {
                auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
                if (childObj) 
                {
                    handleQuit(*childObj);
                }
            }
        };

        // Call the users registered quit function if available
        if (fnOnQuit)
            fnOnQuit();

        // Dispatch a global OnQuit event so any event listeners (including global listeners)
        // can perform cleanup before individual onQuit() methods are called.
        if (eventManager_)
        {
            // Use the root node as the target for the global dispatch if available
            DisplayHandle rootHandle;
            if (rootNode_)
            {
                // create a DisplayHandle referring to the root node
                rootHandle = rootNode_;
            }

            auto quitEvent = std::make_unique<Event>(EventType::OnQuit, rootHandle);
            // relatedTarget can be the root/stage handle too (useful to listeners)
            quitEvent->setRelatedTarget(rootHandle);

            // Dispatch via the central dispatch so global events reach both nodes and event-listeners
            eventManager_->dispatchEvent(std::move(quitEvent), DisplayHandle(rootHandle.getName(), rootHandle.getType()));
        }

        // Call recursive quit on the root node (if it exists)
        IDisplayObject* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get());
        if (rootObj)
        {
            setIsTraversing(true);
            handleQuit(*rootObj);
            setIsTraversing(false);
        }

        // Shutdown SDL
        shutdown_SDL();
    }

    void Core::onRender()
    {
        SDL_Renderer* renderer = getRenderer();
        if (!renderer)
            ERROR("Core::onRender() Error: Renderer is null.");

        // Lambda for recursive render handling using std::function
        std::function<void(IDisplayObject&)> handleRender;
        SDL_Texture* texture = texture_;
        handleRender = [this, &handleRender, renderer, texture](IDisplayObject& node) 
        {
            // render the node
            node.onRender();

            DisplayHandle rootHandle = this->getStageHandle();
            // PreRender EventListeners
            if (node.hasEventListeners(EventType::OnPreRender, false))
            {
                auto preRenderEv = std::make_unique<Event>(EventType::OnPreRender, rootHandle);
                preRenderEv->setElapsedTime(this->getElapsedTime());
                preRenderEv->setRelatedTarget(rootHandle);
                eventManager_->dispatchEvent(std::move(preRenderEv), DisplayHandle(rootHandle.getName(), rootHandle.getType()));
            }

            // render children
            for (const auto& child : node.getChildren()) 
            {
                auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
                if (childObj) 
                {
                    // ensure the render target is set correctly before rendering
                    if (texture)
                        SDL_SetRenderTarget(renderer, texture);
                    handleRender(*childObj);
                }
            }

            // OnRender event listeners
            if (node.hasEventListeners(EventType::OnRender, false))
            {
                auto renderEv = std::make_unique<Event>(EventType::OnRender, rootHandle);
                renderEv->setElapsedTime(this->getElapsedTime());
                renderEv->setRelatedTarget(rootHandle);
                eventManager_->dispatchEvent(std::move(renderEv), DisplayHandle(rootHandle.getName(), rootHandle.getType()));        
            }
        };

        // set the render target to the proper background texture
        if (texture_)
            SDL_SetRenderTarget(renderer, texture_); 

        // Clear the entire window to the border color
        SDL_Color color = getColor();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);

        // Call recursive render on the root node (if it exists)
        IDisplayObject* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get());
        if (rootObj)
        {
            setIsTraversing(true);
            handleRender(*rootObj);
            setIsTraversing(false);
        }

        // Call the users registered render function if available. 
        // This is called after the entire scene has been rendered
        // so the user can overlay additional graphics if desired.
        if (fnOnRender)
            fnOnRender();        
    }

    void Core::onEvent(Event& event)
    {
        // -- Let the event dispatcher handle event propagation --
        // NOTE:  This never gets called...

        // // Lambda for recursive event handling using std::function
        // std::function<void(IDisplayObject&)> handleEvent;
        // handleEvent = [&handleEvent, event](IDisplayObject& node) {
        //     node.onEvent(event);
        //     for (const auto& child : node.getChildren()) {
        //         auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
        //         if (childObj) {
        //             handleEvent(*childObj);
        //         }
        //     }
        // };

        // if (fnOnEvent)
        //     fnOnEvent(event);
        // // Call recursive event on the root node (if it exists)
        // IDisplayObject* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get());
        // if (rootObj)
        // {
        //     setIsTraversing(true);
        //     handleEvent(*rootObj);
        //     setIsTraversing(false);
        // }
    }

    void Core::onUpdate(float fElapsedTime)
    {
        // Lambda for recursive update handling using std::function
        std::function<void(IDisplayObject&)> handleUpdate;
        handleUpdate = [this, &handleUpdate, fElapsedTime](IDisplayObject& node) 
        {
            // Dispatch to event listeners first so they can stopPropagation if needed
            if (node.hasEventListeners(EventType::OnUpdate, false))
            {
                DisplayHandle rootNode = this->getStageHandle();
                auto updateEv = std::make_unique<Event>(EventType::OnUpdate, rootNode);
                updateEv->setElapsedTime(fElapsedTime);
                updateEv->setRelatedTarget(rootNode);
                eventManager_->dispatchEvent(std::move(updateEv), rootNode);
            }
            
            // dispatch to the object::onUpdate()
            node.onUpdate(fElapsedTime);

            // update children
            for (const auto& child : node.getChildren()) 
            {
                auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
                if (childObj) 
                {
                    handleUpdate(*childObj);
                }
            }
        };
        if (fnOnUpdate)
            fnOnUpdate(fElapsedTime);
        // Call recursive update on the root node (if it exists)
        IDisplayObject* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get());
        if (rootObj)
        {
            setIsTraversing(true);
            handleUpdate(*rootObj);
            setIsTraversing(false);
        }
    }

    bool Core::onUnitTest()
    {
        // Lambda for recursive unitTest handling using std::function
        std::function<bool(IDisplayObject&)> handleUnitTest;
        handleUnitTest = [&handleUnitTest](IDisplayObject& node) -> bool 
        {
            bool result = node.onUnitTest();
            for (const auto& child : node.getChildren()) 
            {
                auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
                if (childObj) 
                {
                    result = handleUnitTest(*childObj) && result;
                }
            }
            return result;
        };

        bool allTestsPassed = true;
        std::cout << CLR::LT_BLUE << "Starting unit tests..." << CLR::RESET << std::endl;
        std::cout << CLR::indent_push();

        // Run Core-specific unit tests here
        allTestsPassed &= coreTests_();
        allTestsPassed &= factory_->onUnitTest();

        // Run registered unit test function if available
        if (fnOnUnitTest) {
            allTestsPassed &= fnOnUnitTest();
        }
        // Call recursive unitTest on the root node (if it exists)
        IDisplayObject* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get());
        if (rootObj)
        {
            setIsTraversing(true);
            allTestsPassed &= handleUnitTest(*rootObj);
            setIsTraversing(false);
        }

        std::cout << CLR::indent_pop();
        if (allTestsPassed) 
            std::cout << CLR::LT_BLUE << "...Unit tests " << CLR::GREEN << "[PASSED]" << CLR::RESET << std::endl;
        else 
            std::cout << CLR::LT_BLUE << "...Unit tests " << CLR::RED << "[FAILED]" << CLR::RESET << std::endl;

        if (stopAfterUnitTests_==true)
        {
            LUA_INFO("Core: stopping main loop after unit tests.");
            bIsRunning_ = false; // signal to stop the main loop after unit tests
        }

        return allTestsPassed;
    }

    void Core::onWindowResize(int new_window_width, int new_window_height)
    {
        // just return if we are not allowed to resize the texture
        if (!config_.allowTextureResize)
            return;
        config_.windowWidth = static_cast<float>(new_window_width);
        config_.windowHeight = static_cast<float>(new_window_height);
        static int last_window_width = new_window_width;
        static int last_window_height = new_window_height;
        // just return if the window size has not changed
        if (last_window_width == new_window_width && last_window_height == new_window_height)
            return;
        last_window_width = new_window_width;
        last_window_height = new_window_height;

        // Update the window dimensions
        int newWidth;
        int newHeight;

        if (config_.preserveAspectRatio)
        {
            // Use initial window dimensions to determine original aspect ratio
            float tw = static_cast<float>(texture_->w);
            float th = static_cast<float>(texture_->h);
            static float orig_aspect_ratio = 0.0f;
            if (orig_aspect_ratio == 0.0f)
                orig_aspect_ratio = tw / th;
            float windowAspect = static_cast<float>(new_window_width) / new_window_height;

            // Fit texture to window while preserving aspect ratio
            if (windowAspect > orig_aspect_ratio) {
                // Window is wider: fit height, adjust width
                newHeight = static_cast<int>(new_window_height / config_.pixelHeight);
                newWidth = static_cast<int>(newHeight * orig_aspect_ratio);
            } else {
                // Window is taller: fit width, adjust height
                newWidth = static_cast<int>(new_window_width / config_.pixelWidth);
                newHeight = static_cast<int>(newWidth / orig_aspect_ratio);
            }

            config_.windowWidth = static_cast<float>(new_window_width);
            config_.windowHeight = static_cast<float>(new_window_height);

        }
        else // resize the texture to fill the resized screen
        {            
            newWidth = config_.windowWidth / config_.pixelWidth;
            newHeight = config_.windowHeight / config_.pixelHeight;
        }
        // resize the texture
        if (texture_)
        {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
            // create the stage texture
            texture_ = SDL_CreateTexture(renderer_,
                config_.pixelFormat, SDL_TEXTUREACCESS_TARGET,
                newWidth, newHeight);                
            if (!texture_) {
                std::string errorMsg = "SDL_CreateTexture() Error: " + std::string(SDL_GetError());
                ERROR(errorMsg);
            }
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);   
            SDL_SetRenderLogicalPresentation(renderer_, newWidth, newHeight, config_.rendererLogicalPresentation);
        }

        if (fnOnWindowResize)
            fnOnWindowResize(newWidth, newHeight);
    }


    Core::CoreConfig& Core::getConfig()                 { return config_; }
    float Core::getWindowWidth() const                  { return config_.windowWidth; }
    float Core::getWindowHeight() const                 { return config_.windowHeight; }
    float Core::getPixelWidth() const                   { return config_.pixelWidth; }
    float Core::getPixelHeight() const                  { return config_.pixelHeight; }
    bool Core::getPreserveAspectRatio() const           { return config_.preserveAspectRatio; }
    bool Core::getAllowTextureResize() const            { return config_.allowTextureResize; }
    SDL_RendererLogicalPresentation Core::getRendererLogicalPresentation() const { return config_.rendererLogicalPresentation; }
    SDL_WindowFlags Core::getWindowFlags() const        { return config_.windowFlags; }
    SDL_PixelFormat Core::getPixelFormat() const        { return config_.pixelFormat; }

    void Core::setConfig(CoreConfig& config)            { config_ = config; refreshSDLResources(); }
    void Core::setWindowWidth(float width)              { config_.windowWidth = width; refreshSDLResources(); }
    void Core::setWindowHeight(float height)            { config_.windowHeight = height; refreshSDLResources(); }
    void Core::setPixelWidth(float width)               { config_.pixelWidth = width; refreshSDLResources(); }
    void Core::setPixelHeight(float height)             { config_.pixelHeight = height; refreshSDLResources(); }
    void Core::setPreserveAspectRatio(bool preserve)    { config_.preserveAspectRatio = preserve; refreshSDLResources(); }
    void Core::setAllowTextureResize(bool allow)        { config_.allowTextureResize = allow; refreshSDLResources(); }
    void Core::setWindowFlags(SDL_WindowFlags flags)    { config_.windowFlags = flags; refreshSDLResources(); }
    void Core::setPixelFormat(SDL_PixelFormat format)   { config_.pixelFormat = format; refreshSDLResources(); }
    void Core::setRendererLogicalPresentation(SDL_RendererLogicalPresentation presentation) { 
        config_.rendererLogicalPresentation = presentation; refreshSDLResources(); 
    }

    void Core::refreshSDLResources()
    {
        // Store previous config for comparison
        static CoreConfig prevConfig = config_;
        // initialize or reconfigure SDL resources as needed
        reconfigure(prevConfig);
        // Update previous config
        prevConfig = config_;
    } // END void Core::refreshSDLResources()

    bool Core::coreTests_()
    {
        // std::cout << CLR::indent() << "Core::coreTests_()" << std::endl;
        bool allTestsPassed = true;
        bool testResult;

        // SDL_Test: SDL Initialization
        testResult = UnitTests::run("Core System #1", "SDL_WasInit(SDL_INIT_VIDEO)", []() { return (SDL_WasInit(SDL_INIT_VIDEO) != 0); });
        if (!testResult) { std::cout << CLR::indent() << "SDL was NOT initialized!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // SDL_Test: SDL Texture
        testResult = UnitTests::run("Core System #2", "SDL Texture Validity", [this]() { return (texture_ != nullptr); });
        if (!testResult) { std::cout << CLR::indent() << "SDL Texture is null!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // SDL_Test: SDL Renderer
        testResult = UnitTests::run("Core System #3", "SDL Renderer Validity", [this]() { return (renderer_ != nullptr); });
        if (!testResult) { std::cout << CLR::indent() << "SDL Renderer is null!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // SDL_Test: SDL Window
        testResult = UnitTests::run("Core System #4", "SDL Window Validity", [this]() { return (window_ != nullptr); });
        if (!testResult) { std::cout << CLR::indent() << "SDL Window is null!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // RETURN success or failure
        return allTestsPassed;
    } // END bool Core::coreTests_()


    void Core::handleTabKeyPress()
    {
        // Clear the tabList_
        while (!tabList_.empty()) {
            tabList_.pop();
        }

        // Lambda to recursively add the stage and children to the tabList_ if these nodes are tabEnabled_
        auto populateTabList = [this](auto& populateTabListRef, DisplayHandle node) -> void 
        {
            if (!node.isValid()) return;

            // Resolve the DisplayHandle to an IDisplayObject*
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(node.get());
            if (!obj) return;

            // Check if the object is tab-enabled and has a valid tab priority
            if (obj->isTabEnabled() && obj->getTabPriority() >= 0) 
            {
                tabList_.push(node);
            }

            // Recursively process children
            for (const auto& child : obj->getChildren()) 
            {
                if (child.isValid()) 
                {
                    populateTabListRef(populateTabListRef, child);
                }
            }
        };

        // Populate the tabList_ starting from the stage
        DisplayHandle node = getStageHandle();
        populateTabList(populateTabList, node);

            // Find the current object (keyboardFocusedObject_) with key focus within the tabList_
        DisplayHandle currentFocus = keyboardFocusedObject_;
        DisplayHandle nextFocus;

        bool foundCurrentFocus = false;
        std::vector<DisplayHandle> tempList;

        while (!tabList_.empty()) {
            auto candidate = tabList_.top();
            tabList_.pop();
            tempList.push_back(candidate);

            if (candidate == currentFocus) {
                foundCurrentFocus = true;
            } else if (foundCurrentFocus) {
                nextFocus = candidate;
                break;
            }
        }

        // Wrap around if no next focus is found
        if (!nextFocus.isValid() && !tempList.empty()) {
            nextFocus = tempList.front();
        }

        // Restore tabList_
        for (const auto& item : tempList) {
            tabList_.push(item);
        }

        // Update the current object (keyboardFocusedObject_) to the next entry in the tabList_        
    setKeyboardFocusedObject(nextFocus);    // keyboardFocusedObject_ = nextFocus;
    }// END: void Core::handleTabKeyPress(Stage& stage)

    void Core::handleTabKeyPressReverse()
    {
        // Clear the tabList_
        while (!tabList_.empty()) {
            tabList_.pop();
        }

        // Lambda to recursively add the stage and children to the tabList_ if these nodes are tabEnabled_
        auto populateTabList = [this](auto& populateTabListRef, DisplayHandle node) -> void 
        {
            if (!node.isValid()) return;

            // Resolve the DisplayHandle to an IDisplayObject*
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(node.get());
            if (!obj) return;

            // Check if the object is tab-enabled and has a valid tab priority
            if (obj->isTabEnabled() && obj->getTabPriority() >= 0) 
            {
                tabList_.push(node);
            }

            // Recursively process children
            for (const auto& child : obj->getChildren()) 
            {
                if (child.isValid()) 
                {
                    populateTabListRef(populateTabListRef, child);
                }
            }
        };

        // Populate the tabList_ starting from the stage
        DisplayHandle node = getStageHandle();
        populateTabList(populateTabList, node);

        // Find the current object (keyboardFocusedObject_) with key focus within the tabList_
        DisplayHandle currentFocus = keyboardFocusedObject_;
        DisplayHandle previousFocus;

        bool foundCurrentFocus = false;
        std::vector<DisplayHandle> tempList;

        while (!tabList_.empty()) {
            auto candidate = tabList_.top();
            tabList_.pop();
            tempList.push_back(candidate);

                if (candidate == currentFocus) {
                foundCurrentFocus = true;
            } else if (!foundCurrentFocus) {
                previousFocus = candidate;
            }
        }
        // Wrap around if no previous focus is found
        if (!previousFocus.isValid() && !tempList.empty()) {
            previousFocus = tempList.back();
        }

        // Restore tabList_
        for (const auto& item : tempList) {
            tabList_.push(item);
        }
        // Update the current object (keyboardFocusedObject_) to the previous entry in the tabList_
        setKeyboardFocusedObject(previousFocus);    // keyboardFocusedObject_ = previousFocus;
    } // END:void Core::handleTabKeyPressReverse(Stage& stage)

    void Core::setKeyboardFocusedObject(DisplayHandle obj)
    { keyboardFocusedObject_ = obj; }
    DisplayHandle Core::getKeyboardFocusedObject() const
    { return keyboardFocusedObject_; }
    void Core::setMouseHoveredObject(DisplayHandle obj)
    { hoveredObject_ = obj; }
    DisplayHandle Core::getMouseHoveredObject() const
    { return hoveredObject_; }

    void Core::setRootNode(const std::string& name)
    {
        DisplayHandle stageHandle = factory_->getDisplayObject(name);
        if (stageHandle.isValid() && dynamic_cast<Stage*>(stageHandle.get()))
        {
            rootNode_ = stageHandle;
            setWindowTitle("Stage: " + rootNode_.get()->getName());
        }
    }
    void Core::setRootNode(const DisplayHandle& handle) 
    { 
        rootNode_ = handle;     
        if (rootNode_.isValid() && rootNode_.get()) setWindowTitle("Stage: " + rootNode_.get()->getName());
    }
    void Core::setStage(const std::string& name) 
    { 
        setRootNode(name); 
    } 
    // Alias for backward compatibility
    Stage* Core::getStage() const 
    { 
        return dynamic_cast<Stage*>(getRootNodePtr()); 
    } 
    // Alias for backward compatibility
    IDisplayObject* Core::getRootNodePtr() const 
    { 
        return dynamic_cast<IDisplayObject*>(rootNode_.get()); 
    }
    DisplayHandle Core::getRootNode() const 
    {
        return rootNode_; 
    }



    // --- Factory Wrapper Implementations --- //

    DisplayHandle Core::createDisplayObject(const std::string& typeName, const sol::table& config) {
        return getFactory().create(typeName, config);
    }
    DisplayHandle Core::createDisplayObject(const std::string& typeName, const IDisplayObject::InitStruct& init) {
        return getFactory().create(typeName, init);
    }
    DisplayHandle Core::createDisplayObjectFromScript(const std::string& typeName, const std::string& luaScript) {
        return getFactory().create(typeName, luaScript);
    }

    AssetHandle Core::createAssetObject(const std::string& typeName, const sol::table& config) {
        return getFactory().createAsset(typeName, config);
    }
    AssetHandle Core::createAssetObject(const std::string& typeName, const SDOM::IAssetObject::InitStruct& init) {
        return getFactory().createAsset(typeName, init);
    }
    AssetHandle Core::createAssetObjectFromScript(const std::string& typeName, const std::string& luaScript) {
        return getFactory().createAsset(typeName, luaScript);
    }


    IDisplayObject* Core::getDisplayObjectPtr(const std::string& name) {
        return getFactory().getDomObj(name);
    }
    DisplayHandle Core::getDisplayObject(const std::string& name) {
        return getFactory().getDisplayObject(name);
    }
    bool Core::hasDisplayObject(const std::string& name) const {
        DisplayHandle handle = factory_->getDisplayObject(name);
        return handle.isValid();
    }

    IAssetObject* Core::getAssetObjectPtr(const std::string& name) {
        return getFactory().getResObj(name);
    }
    AssetHandle Core::getAssetObject(const std::string& name) {
        return getFactory().getAssetObject(name);
    }
    bool Core::hasAssetObject(const std::string& name) const {
        AssetHandle handle = factory_->getAssetObject(name);
        return handle.isValid();   
    } 


    void Core::addDisplayObject(const std::string& name, std::unique_ptr<IDisplayObject> displayObject) {
        getFactory().addDisplayObject(name, std::move(displayObject));
    }
    void Core::destroyDisplayObject(const std::string& name) {
        getFactory().destroyDisplayObject(name);
    }

    int Core::countOrphanedDisplayObjects() const {
        return getFactory().countOrphanedDisplayObjects();
    }
    std::vector<DisplayHandle> Core::getOrphanedDisplayObjects() {
        return getFactory().getOrphanedDisplayObjects();
    }
    void Core::destroyOrphanedDisplayObjects() {
        getFactory().destroyOrphanedDisplayObjects();
    }
    void Core::detachOrphans() {
        getFactory().detachOrphans();
    }

    void Core::attachFutureChildren() {
        getFactory().attachFutureChildren();
    }
    void Core::addToOrphanList(const DisplayHandle& orphan) {
        getFactory().addToOrphanList(orphan);
    }
    void Core::addToFutureChildrenList(const DisplayHandle& child, const DisplayHandle& parent,
        bool useWorld, int worldX, int worldY) {
        getFactory().addToFutureChildrenList(child, parent, useWorld, worldX, worldY);
    }

    std::vector<std::string> Core::listDisplayObjectNames() const {
        return getFactory().listDisplayObjectNames();
    }
    void Core::clearFactory() {
        getFactory().clear();
    }
    void Core::printObjectRegistry() const {
        getFactory().printObjectRegistry();
    }


    // --- Lua UserType Registration --- //


    void Core::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // --- Call base class registration to include inherited properties/commands --- //
        SUPER::_registerLuaBindings(typeName, lua);

        // --- Debug output --- //
        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Core";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN 
                    << typeName << CLR::RESET << std::endl;
        }

        // --- Create the Core usertype (no constructor) and bind methods directly --- //

        // Note: Do NOT expose the raw Core userdata as the global `Core` since
        // configuration scripts and other code may treat `Core` as a plain table.
        // Instead create a forwarding table (`CoreForward`) that dispatches to the
        // Core singleton, assign it to both `CoreForward` and the global `Core`,
        // and keep the underlying userdata registered separately.  Individual
        // helper bindings may also register best-effort global aliases for some
        // functions, but those are per-function and do not replace the forwarding table.

        sol::usertype<Core> objHandleType = lua.new_usertype<Core>(typeName,
            sol::no_constructor, sol::base_classes, sol::bases<IDataObject>());        
        this->objHandleType_ = objHandleType;   // Save usertype
        sol::table coreTable = lua.create_table(); //Create convenience CoreForward table (do NOT overwrite Core global)

        // --- Register Event types and EventType table (best-effort) --- //

        // Register EventType bindings into Lua and export a read-only table
        // `EventType` containing references to the C++ EventType instances.
        // Note: we reference the existing C++ EventType objects (via
        // std::ref) so those instances must outlive the Lua state using them.
        // Failures here are non-fatal for consumers, but we log them so
        // developers can see registration problems during development.
        
        try {
            Event::registerLua(lua);
            sol::table etbl = lua.create_table();
            const auto& reg = EventType::getRegistry();
            for (const auto& kv : reg) {
                const std::string& name = kv.first;
                EventType* ptr = kv.second;
                if (ptr) {
                    etbl[name] = sol::make_object(lua, std::ref(*ptr));
                }
            }
            lua["EventType"] = etbl;
        } catch (const std::exception& e) {
            DEBUG_LOG("Failed to register EventType bindings: " << e.what());
        } catch (...) {
            DEBUG_LOG("Failed to register EventType bindings: unknown exception");
        }


        // ---------------------------------------- //
        // --- Register Core Functions with Lua --- //
        // ---------------------------------------- //


        // --- Main Loop & Event Dispatch --- //        
        SDOM::bind_noarg("quit", quit_lua, objHandleType, coreTable, lua);
        SDOM::bind_noarg("shutdown", shutdown_lua, objHandleType, coreTable, lua);
        // SDOM::bind_noarg("run", run_lua, objHandleType, coreTable, lua);
        SDOM::bind_table("configure", configure_lua, objHandleType, coreTable, lua);
        // Also expose a top-level global `configure(table)` convenience
        // function that forwards to the Core singleton.  The Core method
        // bound into `Core` expects a userdata first-argument (the Core
        // forwarder), so we must expose a separate free function that
        // accepts a table and calls the singleton directly.
        try {
            // Expose a top-level `configure(table)` that forwards to our
            // configure_lua wrapper so the 'resources' preprocessing runs.
            lua.set_function("configure", [](const sol::table& t) {
                SDOM::configure_lua(t);
            });
        } catch(...) {}
        SDOM::bind_string("configureFromFile", configureFromFile_lua, objHandleType, coreTable, lua);

	    // --- Callback/Hook Registration --- //
        SDOM::bind_callback_bool("registerOnInit", registerOnInit_lua, objHandleType, coreTable, lua);
        SDOM::bind_callback_void("registerOnQuit", registerOnQuit_lua, objHandleType, coreTable, lua);
        SDOM::bind_callback_update("registerOnUpdate", registerOnUpdate_lua, objHandleType, coreTable, lua);
        SDOM::bind_callback_event("registerOnEvent", registerOnEvent_lua, objHandleType, coreTable, lua);
        SDOM::bind_callback_void("registerOnRender", registerOnRender_lua, objHandleType, coreTable, lua);
        SDOM::bind_callback_bool("registerOnUnitTest", registerOnUnitTest_lua, objHandleType, coreTable, lua);
        SDOM::bind_callback_resize("registerOnWindowResize", registerOnWindowResize_lua, objHandleType, coreTable, lua);
        SDOM::bind_string_function_forwarder("registerOn", registerOn_lua, objHandleType, coreTable, lua); // custom handling above

	    // --- Stage/Root Node Management --- //
        SDOM::bind_name_or_handle("setRootNode", setRootNodeByName_lua, setRootNode_lua, objHandleType, coreTable, lua);
        SDOM::bind_name_or_handle("setRoot", setRootNodeByName_lua, setRootNode_lua, objHandleType, coreTable, lua);  // alias of setRootNode()
        SDOM::bind_name_or_handle("setStage", setRootNodeByName_lua, setRootNode_lua, objHandleType, coreTable, lua); // alias of setRoot()
        SDOM::bind_return_displayobject("getRoot", getRoot_lua, objHandleType, coreTable, lua);
        SDOM::bind_return_displayobject("getRootHandle", getRoot_lua, objHandleType, coreTable, lua);   // alias of getRoot()
        SDOM::bind_return_displayobject("getStage", getStage_lua, objHandleType, coreTable, lua);
        SDOM::bind_return_displayobject("getStageHandle", getStage_lua, objHandleType, coreTable, lua); // alias of getStage()

        // --- Factory & EventManager Access --- //
        SDOM::bind_return_bool("getIsTraversing", getIsTraversing_lua, objHandleType, coreTable, lua);
        SDOM::bind_bool_arg("setIsTraversing", setIsTraversing_lua, objHandleType, coreTable, lua);

        // --- Object Creation --- //
        SDOM::bind_string_table_return_do("createDisplayObject", createDisplayObject_lua, objHandleType, coreTable, lua);
        SDOM::bind_string_return_do("getDisplayObject", getDisplayObject_lua, objHandleType, coreTable, lua);
        SDOM::bind_string_return_bool("hasDisplayObject", hasDisplayObject_lua, objHandleType, coreTable, lua);

    SDOM::bind_string_table_return_asset("createAssetObject", createAssetObject_lua, objHandleType, coreTable, lua);
    // Alias for historical/shortcut usage: expose Core:createAsset -> createAssetObject
    SDOM::bind_string_table_return_asset("createAsset", createAssetObject_lua, objHandleType, coreTable, lua);
        SDOM::bind_string_return_asset("getAssetObject", getAssetObject_lua, objHandleType, coreTable, lua);
        SDOM::bind_string_return_bool("hasAssetObject", hasAssetObject_lua, objHandleType, coreTable, lua);
        
	    // --- Focus & Hover Management --- //
        SDOM::bind_noarg("doTabKeyPressForward", doTabKeyPressForward_lua, objHandleType, coreTable, lua);
        SDOM::bind_noarg("doTabKeyPressReverse", doTabKeyPressReverse_lua, objHandleType, coreTable, lua);
        SDOM::bind_noarg("handleTabKeyPress", doTabKeyPressForward_lua, objHandleType, coreTable, lua); // alias of doTabKeyPressForward()
        SDOM::bind_noarg("handleTabKeyPressReverse", doTabKeyPressReverse_lua, objHandleType, coreTable, lua); // alias of doTabKeyPressReverse()
        SDOM::bind_do_arg("setKeyboardFocusedObject", setKeyboardFocusedObject_lua, objHandleType, coreTable, lua);
        SDOM::bind_return_displayobject("getKeyboardFocusedObject", getKeyboardFocusedObject_lua, objHandleType, coreTable, lua);
        SDOM::bind_do_arg("setMouseHoveredObject", setMouseHoveredObject_lua, objHandleType, coreTable, lua);
        SDOM::bind_return_displayobject("getMouseHoveredObject", getMouseHoveredObject_lua, objHandleType, coreTable, lua);

	    // --- Window Title & Timing --- //
        SDOM::bind_return_string("getWindowTitle", getWindowTitle_lua, objHandleType, coreTable, lua);
        SDOM::bind_string("setWindowTitle", setWindowTitle_lua, objHandleType, coreTable, lua);
        SDOM::bind_return_float("getElapsedTime", getElapsedTime_lua, objHandleType, coreTable, lua);
        SDOM::bind_return_float("getDeltaTime", getElapsedTime_lua, objHandleType, coreTable, lua); // alias of getElapsedTime()

	    // --- Event helpers (exposed to Lua) --- //
        SDOM::bind_noarg("pumpEventsOnce", pumpEventsOnce_lua, objHandleType, coreTable, lua);
        SDOM::bind_object_arg("pushMouseEvent", pushMouseEvent_lua, objHandleType, coreTable, lua);
        SDOM::bind_object_arg("pushKeyboardEvent", pushKeyboardEvent_lua, objHandleType, coreTable, lua);

	    // --- Orphan / Future Child Management --- //
        SDOM::bind_string("destroyDisplayObject", destroyDisplayObject_lua, objHandleType, coreTable, lua);
        SDOM::bind_return_int("countOrphanedDisplayObjects", countOrphanedDisplayObjects_lua, objHandleType, coreTable, lua);
        SDOM::bind_return_vector_do("getOrphanedDisplayObjects", getOrphanedDisplayObjects_lua, objHandleType, coreTable, lua);
        SDOM::bind_noarg("destroyOrphanedDisplayObjects", destroyOrphanedDisplayObjects_lua, objHandleType, coreTable, lua);
        SDOM::bind_noarg("destroyOrphanedObjects", destroyOrphanedDisplayObjects_lua, objHandleType, coreTable, lua); // alias of destroyOrphanedDisplayObjects()
        SDOM::bind_noarg("destroyOrphans", destroyOrphanedDisplayObjects_lua, objHandleType, coreTable, lua); // alias of destroyOrphanedDisplayObjects()
        SDOM::bind_noarg("collectGarbage", collectGarbage_lua, objHandleType, coreTable, lua);

        // --- Utility Methods --- //
        SDOM::bind_return_vector_string("listDisplayObjectNames", listDisplayObjectNames_lua, objHandleType, coreTable, lua);
        SDOM::bind_noarg("printObjectRegistry", printObjectRegistry_lua, objHandleType, coreTable, lua);


        // Expose CoreForward (explicit) and make the global `Core` point to
        // the forwarding table so scripts can use the table-based API
        // consistently. The forwarding table dispatches to the Core
        // singleton internally so both dot- and colon-call styles are
        // supported.
        lua["CoreForward"] = coreTable;
        lua["Core"] = coreTable;

    } // End Core::_registerDisplayObject()


} // namespace SDOM

