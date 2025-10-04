// SDOM_Core.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_DisplayObject.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_SDL_Utils.hpp>
#include <SDOM/SDOM_Factory.hpp>

#include <SDOM/SDOM_Utils.hpp> // for parseColor
#include <SDOM/lua_Core.hpp>
#include <algorithm>
#include <filesystem>

namespace SDOM
{

    Core::Core() : IDataObject()
    {
        // Open base libraries plus package and io so embedded scripts can use
        // `require`/`package` and basic I/O if desired by examples.
        lua_.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::package, sol::lib::io);
        SDL_Utils::registerLua(lua_);
        factory_ = new Factory();
        eventManager_ = new EventManager();
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
        // Initialize the Factory (but only once)
        static bool factoryInitialized = false;
        if (factory_ && !factoryInitialized) {
            factory_->onInit();
            factoryInitialized = true;
        }
    }

    void Core::configureFromLua(const sol::table& lua)
    {
        CoreConfig config;
        sol::table coreObj = lua["Core"];
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
        std::function<void(sol::table, DisplayObject)> createResourceRecursive;
        createResourceRecursive = [&](sol::table obj, DisplayObject parent) 
        {
            std::string type = obj["type"].get_or(std::string(""));
            DisplayObject handle;
            if (!type.empty()) 
            {
                handle = factory_->create(type, obj);
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
                createResourceRecursive(child, DisplayObject());
            }
        }

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
            this->_registerDisplayObject("Core", lua_);

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
            // _registerDisplayObject() created CoreForward.
            try {
                if (lua_["CoreForward"].valid()) {
                    lua_["Core"] = lua_["CoreForward"];
                }
            } catch (...) {}
            if (ret.is<sol::table>()) {
                sol::table configTable = ret.as<sol::table>();
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
        SDL_Quit();
    }

    // // --- Core forwarding helpers to Factory ---
    // std::vector<std::string> Core::getPropertyNamesForType(const std::string& typeName) const
    // {
    //     if (factory_) return factory_->getPropertyNamesForType(typeName);
    //     return {};
    // }

    // std::vector<std::string> Core::getCommandNamesForType(const std::string& typeName) const
    // {
    //     if (factory_) return factory_->getCommandNamesForType(typeName);
    //     return {};
    // }

    // std::vector<std::string> Core::getFunctionNamesForType(const std::string& typeName) const
    // {
    //     if (factory_) return factory_->getFunctionNamesForType(typeName);
    //     return {};
    // }

    void Core::run(const sol::table& config)
    {
        configureFromLua(config);
        // Now run normally
        run();
    }

    // void Core::run(const std::string& configFile)
    // {
    //     // If configFile refers to an actual file on disk, load it.
    //     // Otherwise treat the string as raw Lua script and execute it
    //     // in this Core's Lua state, then pull the `config` table.
    //     if (!configFile.empty()) {
    //         // Portable file-exists check using ifstream to avoid std::filesystem
    //         std::ifstream infile(configFile);
    //         if (infile.good()) {
    //             configureFromLuaFile(configFile);
    //         } else {
    //             // Ensure Core usertype is registered in this lua state so the
    //             // script can reference `Core` (register callbacks, call Core:run, etc.)
    //             this->_registerLua("Core", lua_);
    //             // Treat the string as a raw Lua script
    //             try {
    //                 lua_.script(configFile);
    //                 sol::table configTable = lua_["config"];
    //                 if (configTable.valid()) {
    //                     configureFromLua(configTable);
    //                 } else {
    //                     ERROR("Lua script did not produce a valid 'config' table.");
    //                 }
    //             } catch (const sol::error& e) {
    //                 ERROR(std::string("Error executing Lua script for configuration: ") + e.what());
    //             }
    //         }
    //     }
    //     // Now run normally
    //     run();
    // }

    void Core::run(const std::string& configFile)
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
                    return;
                }

                // If it doesn't look like code and no path separators, treat as missing file rather than raw script
                if (!looksLikeCode) {
                    ERROR(std::string("Config string does not appear to be Lua code and no file found: ") + configFile);
                    return;
                }

                // Treat as inline Lua code
                // this->_registerLua("Core", lua_);
                this->_registerDisplayObject("Core", lua_);
                try {
                    sol::load_result chunk = lua_.load(configFile);
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
        run();
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
        if (recreate_renderer && !renderer_) {
            renderer_ = SDL_CreateRenderer(window_, NULL);
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

    void Core::run()
    {
        // std::cout << "Core::run()" << std::endl;

        try
        {
            static bool runWasStarted = false;
            if (runWasStarted) {
                // just silently return if called a second time.  Recursion here would be inapproprite.
                // ERROR("Core::run() Error: run() has already been called once. Multiple calls to run() are not allowed.");
                return;
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
                return;
            }
            // else
            // {
            //     std::cout << "Core::run() Root node is a valid Stage: " << rootStage->getName() << std::endl;
            //     rootStage->printTree();
            // }

            // Now run user tests after initialization
            SDL_Delay(125); // give some time for tiled window to be configured by the OS Compositor 
            bool testsPassed = onUnitTest();
            if (!testsPassed) 
            {
                ERROR("Unit tests failed. Aborting run.");
            }

            SDL_Event event;

            // // Apply any pending configuration requested from other threads
            // applyPendingConfig();

            while (bIsRunning_) 
            {
                while (SDL_PollEvent(&event)) 
                {
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
                        DisplayObject rootHandle = getStageHandle();

                        // dispatch OnEvent to rootNode_ so global listeners can inspect raw SDL_Events
                        auto ev = std::make_unique<Event>(EventType::OnEvent, rootHandle);
                        ev->setSDL_Event(event);
                        ev->setRelatedTarget(rootHandle);
                        eventManager_->dispatchEvent(std::move(ev), DisplayObject(rootHandle.getName(), rootHandle.getType()));

                        // dispatch SDL_Event to rootNode_ so global listeners can inspect raw SDL_Events
                        auto sdl_ev = std::make_unique<Event>(EventType::SDL_Event, rootHandle);
                        sdl_ev->setSDL_Event(event);
                        sdl_ev->setRelatedTarget(rootHandle);
                        eventManager_->dispatchEvent(std::move(sdl_ev), DisplayObject(rootHandle.getName(), rootHandle.getType()));
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

            // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Terminal Error", errStr.c_str(), nullptr);
        }
        catch (const std::exception& e) 
        {
            // Handle standard exceptions
            std::cerr << CLR::RED << "Standard exception caught: " << CLR::WHITE << e.what() << CLR::RESET << std::endl;
            std::string errStr = "Exception Caught: " + std::string(e.what());
            // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Standard Exception", errStr.c_str(), nullptr);
        }
        catch (...) 
        {
            // Handle unknown exceptions
            std::cerr << CLR::RED << "Unknown exception caught!" << CLR::RESET << std::endl;
            // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unknown Exception", "An unknown error occurred.", nullptr);
        }
        // properly shutdown
        onQuit();
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
            DisplayObject rootHandle;
            if (rootNode_)
            {
                // create a DisplayObject referring to the root node
                rootHandle = rootNode_;
            }

            auto quitEvent = std::make_unique<Event>(EventType::OnQuit, rootHandle);
            // relatedTarget can be the root/stage handle too (useful to listeners)
            quitEvent->setRelatedTarget(rootHandle);

            // Dispatch via the central dispatch so global events reach both nodes and event-listeners
            eventManager_->dispatchEvent(std::move(quitEvent), DisplayObject(rootHandle.getName(), rootHandle.getType()));
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

            DisplayObject rootHandle = this->getStageHandle();
            // PreRender EventListeners
            if (node.hasEventListeners(EventType::OnPreRender, false))
            {
                auto preRenderEv = std::make_unique<Event>(EventType::OnPreRender, rootHandle);
                preRenderEv->setElapsedTime(this->getElapsedTime());
                preRenderEv->setRelatedTarget(rootHandle);
                eventManager_->dispatchEvent(std::move(preRenderEv), DisplayObject(rootHandle.getName(), rootHandle.getType()));
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
                eventManager_->dispatchEvent(std::move(renderEv), DisplayObject(rootHandle.getName(), rootHandle.getType()));        
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
                DisplayObject rootNode = this->getStageHandle();
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
        auto populateTabList = [this](auto& populateTabListRef, DisplayObject node) -> void 
        {
            if (!node.isValid()) return;

            // Resolve the DisplayObject to an IDisplayObject*
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
    DisplayObject node = getStageHandle();
    populateTabList(populateTabList, node);

        // Find the current object (keyboardFocusedObject_) with key focus within the tabList_
    DisplayObject currentFocus = keyboardFocusedObject_;
    DisplayObject nextFocus;

        bool foundCurrentFocus = false;
    std::vector<DisplayObject> tempList;

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
        auto populateTabList = [this](auto& populateTabListRef, DisplayObject node) -> void 
        {
            if (!node.isValid()) return;

            // Resolve the DisplayObject to an IDisplayObject*
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
    DisplayObject node = getStageHandle();
    populateTabList(populateTabList, node);

        // Find the current object (keyboardFocusedObject_) with key focus within the tabList_
    DisplayObject currentFocus = keyboardFocusedObject_;
    DisplayObject previousFocus;

        bool foundCurrentFocus = false;
    std::vector<DisplayObject> tempList;

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

    void Core::setKeyboardFocusedObject(DisplayObject obj)
    { keyboardFocusedObject_ = obj; }
    DisplayObject Core::getKeyboardFocusedObject() const
    { return keyboardFocusedObject_; }
    void Core::setMouseHoveredObject(DisplayObject obj)
    { hoveredObject_ = obj; }
    DisplayObject Core::getMouseHoveredObject() const
    { return hoveredObject_; }

    void Core::setRootNode(const std::string& name)
    {
        DisplayObject stageHandle = factory_->getDisplayObject(name);
        if (stageHandle.isValid() && dynamic_cast<Stage*>(stageHandle.get()))
        {
            rootNode_ = stageHandle;
            setWindowTitle("Stage: " + rootNode_.get()->getName());
        }
    }
    void Core::setRootNode(const DisplayObject& handle) 
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
    DisplayObject Core::getRootNode() const 
    {
        return rootNode_; 
    }



    // --- Factory Wrapper Implementations --- //

    DisplayObject Core::createDisplayObject(const std::string& typeName, const sol::table& config) {
        return getFactory().create(typeName, config);
    }
    DisplayObject Core::createDisplayObject(const std::string& typeName, const IDisplayObject::InitStruct& init) {
        return getFactory().create(typeName, init);
    }
    DisplayObject Core::createDisplayObjectFromScript(const std::string& typeName, const std::string& luaScript) {
        return getFactory().create(typeName, luaScript);
    }

    IDisplayObject* Core::getDisplayObjectPtr(const std::string& name) {
        return getFactory().getDomObj(name);
    }
    DisplayObject Core::getDisplayObject(const std::string& name) {
        return getFactory().getDisplayObject(name);
    }
    bool Core::hasDisplayObject(const std::string& name) const {
        DisplayObject handle = factory_->getDisplayObject(name);
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
    std::vector<DisplayObject> Core::getOrphanedDisplayObjects() {
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
    void Core::addToOrphanList(const DisplayObject orphan) {
        getFactory().addToOrphanList(orphan);
    }
    void Core::addToFutureChildrenList(const DisplayObject child, const DisplayObject parent,
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


    void Core::_registerLua(const std::string& typeName, sol::state_view lua)
    {
    }

    void Core::_registerDisplayObject(const std::string& typeName, sol::state_view lua)
    {
        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Core";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN 
                    << typeName << CLR::RESET << std::endl;
        }

        // Call base class registration to include inherited properties/commands
        SUPER::_registerDisplayObject(typeName, lua);

        // // Create and save usertype table 
        // SDOM::Factory& factory = SDOM::getFactory();

        // Create the Core usertype (no constructor) and bind methods directly
        sol::usertype<Core> objHandleType = lua.new_usertype<Core>(typeName,
            sol::no_constructor, sol::base_classes, sol::bases<IDataObject>());

        // Bind direct methods on the Core usertype to ensure proper colon-call
        // signatures for Lua callers. These use the thin wrappers defined in
        // lua_Core.cpp which forward to the Core singleton or member methods.
        objHandleType["quit"] = [](Core& /*core*/) { quit_lua(); return true; };
        objHandleType["shutdown"] = [](Core& /*core*/) { shutdown_lua(); return true; };
        objHandleType["createDisplayObject"] = sol::overload(
            [](Core& core, const std::string& typeName, const sol::table& cfg) -> DisplayObject { return core.createDisplayObject(typeName, cfg); },
            [](const std::string& typeName, const sol::table& cfg) -> DisplayObject { return Core::getInstance().createDisplayObject(typeName, cfg); }
        );

        objHandleType["getDisplayObject"] = sol::overload(
            [](Core& core, const std::string& name) -> DisplayObject { DisplayObject h = core.getDisplayObject(name); return h; },
            [](const std::string& name) -> DisplayObject { DisplayObject h = Core::getInstance().getDisplayObject(name); return h; }
        );

        objHandleType["getStageHandle"] = sol::overload(
            [](Core& core) -> DisplayObject { return core.getStageHandle(); },
            []() -> DisplayObject { return Core::getInstance().getStageHandle(); }
        );
        objHandleType["hasDisplayObject"] = &Core::hasDisplayObject;
        objHandleType["destroyDisplayObject"] = &Core::destroyDisplayObject;
        objHandleType["pumpEventsOnce"] = &Core::pumpEventsOnce;

        objHandleType["run"] = sol::overload(
            [](Core& /*core*/) { run_lua(); },
            [](Core& core, const sol::table& cfg) { core.run(cfg); },
            [](Core& core, const std::string& s) { core.run(s); },
            []() { Core::getInstance().run(); },
            [](const sol::table& cfg) { Core::getInstance().run(cfg); },
            [](const std::string& s) { Core::getInstance().run(s); }
        );

        objHandleType["pushMouseEvent"] = [](Core& /*core*/, const sol::object& args) { pushMouseEvent_lua(args); };
        objHandleType["pushKeyboardEvent"] = [](Core& /*core*/, const sol::object& args) { pushKeyboardEvent_lua(args); };

        objHandleType["configure"] = sol::overload(
            [](Core& core, const sol::table& cfg) { core.configureFromLua(cfg); },
            [](const sol::table& cfg) { Core::getInstance().configureFromLua(cfg); }
        );
        objHandleType["configureFromFile"] = sol::overload(
            [](Core& core, const std::string& filename) { core.configureFromLuaFile(filename); },
            [](const std::string& filename) { Core::getInstance().configureFromLuaFile(filename); }
        );

        objHandleType["registerOnInit"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) {
                registerOnInit_lua([f]() -> bool {
                    // Debug: inspect the global Core object in the Lua state before calling
                    try {
                        sol::state& lua = Core::getInstance().getLua();
                        sol::object coreObj = lua["Core"];
                        std::cout << "[debug] Lua Core global valid=" << (coreObj.valid() ? "true" : "false") << " ";
                        if (coreObj.valid()) {
                            std::cout << "is<Core*>=" << (coreObj.is<Core*>() ? "true" : "false") << " ";
                            std::cout << "is<std::reference_wrapper<Core>>=" << (coreObj.is<std::reference_wrapper<Core>>() ? "true" : "false") << " ";
                            std::cout << "is<table>=" << (coreObj.is<sol::table>() ? "true" : "false") << " ";
                        }
                        std::cout << std::endl;
                    } catch (...) {}
                    sol::protected_function pf = f; sol::protected_function_result r = pf();
                    if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnInit error: ") + err.what()); return false; }
                    try { return r.get<bool>(); } catch (...) { return false; }
                });
            },
            [](const sol::function& f) {
                registerOnInit_lua([f]() -> bool {
                    try {
                        sol::state& lua = Core::getInstance().getLua();
                        sol::object coreObj = lua["Core"];
                        std::cout << "[debug] Lua Core global valid=" << (coreObj.valid() ? "true" : "false") << " ";
                        if (coreObj.valid()) {
                            std::cout << "is<Core*>=" << (coreObj.is<Core*>() ? "true" : "false") << " ";
                            std::cout << "is<std::reference_wrapper<Core>>=" << (coreObj.is<std::reference_wrapper<Core>>() ? "true" : "false") << " ";
                            std::cout << "is<table>=" << (coreObj.is<sol::table>() ? "true" : "false") << " ";
                        }
                        std::cout << std::endl;
                    } catch (...) {}
                    sol::protected_function pf = f; sol::protected_function_result r = pf();
                    if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnInit error: ") + err.what()); return false; }
                    try { return r.get<bool>(); } catch (...) { return false; }
                });
            }
        );

        objHandleType["registerOnQuit"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) { registerOnQuit_lua([f]() { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnQuit error: ") + err.what()); } }); },
            [](const sol::function& f) { registerOnQuit_lua([f]() { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnQuit error: ") + err.what()); } }); }
        );

        objHandleType["registerOnUpdate"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) { registerOnUpdate_lua([f](float dt) { sol::protected_function pf = f; sol::protected_function_result r = pf(dt); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUpdate error: ") + err.what()); } }); },
            [](const sol::function& f) { registerOnUpdate_lua([f](float dt) { sol::protected_function pf = f; sol::protected_function_result r = pf(dt); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUpdate error: ") + err.what()); } }); }
        );

        objHandleType["registerOnEvent"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) { registerOnEvent_lua([f](const Event& e) { sol::protected_function pf = f; sol::protected_function_result r = pf(e); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnEvent error: ") + err.what()); } }); },
            [](const sol::function& f) { registerOnEvent_lua([f](const Event& e) { sol::protected_function pf = f; sol::protected_function_result r = pf(e); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnEvent error: ") + err.what()); } }); }
        );

        objHandleType["registerOnRender"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) { registerOnRender_lua([f]() { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnRender error: ") + err.what()); } }); },
            [](const sol::function& f) { registerOnRender_lua([f]() { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnRender error: ") + err.what()); } }); }
        );

        objHandleType["registerOnUnitTest"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) { registerOnUnitTest_lua([f]() -> bool { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUnitTest error: ") + err.what()); return false; } try { return r.get<bool>(); } catch (...) { return false; } }); },
            [](const sol::function& f) { registerOnUnitTest_lua([f]() -> bool { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUnitTest error: ") + err.what()); return false; } try { return r.get<bool>(); } catch (...) { return false; } }); }
        );

        objHandleType["registerOnWindowResize"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) { registerOnWindowResize_lua([f](int w, int h) { sol::protected_function pf = f; sol::protected_function_result r = pf(w, h); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnWindowResize error: ") + err.what()); } }); },
            [](const sol::function& f) { registerOnWindowResize_lua([f](int w, int h) { sol::protected_function pf = f; sol::protected_function_result r = pf(w, h); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnWindowResize error: ") + err.what()); } }); }
        );

        objHandleType["registerOn"] = sol::overload(
            [](Core& /*core*/, const std::string& name, const sol::function& f) { registerOn_lua(name, f); },
            [](const std::string& name, const sol::function& f) { registerOn_lua(name, f); }
        );

        // objHandleType["getPropertyNamesForType"] = [](Core& core, const std::string& t) { return core.getPropertyNamesForType(t); };
        // objHandleType["getCommandNamesForType"] = [](Core& core, const std::string& t) { return core.getCommandNamesForType(t); };
        // objHandleType["getFunctionNamesForType"] = [](Core& core, const std::string& t) { return core.getFunctionNamesForType(t); };

        objHandleType["createDisplayObject"] = [](Core& core, const std::string& typeName, const sol::table& cfg) -> DisplayObject {
            DisplayObject h = core.createDisplayObject(typeName, cfg);
            return h;
        };

        // Save usertype
        this->objHandleType_ = objHandleType;

    // Do NOT expose the raw Core userdata as the global `Core` since
    // configuration scripts and other code may treat `Core` as a table.
    // Instead expose a convenience forwarding table (`CoreForward`) and
    // also set the global `Core` to that table so Lua callers use the
    // table-based API which dispatches to the Core singleton internally.

        // Register Event types and EventType table (best-effort)
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
        } catch (...) {}

        // Create convenience CoreForward table (do NOT overwrite Core global)
        sol::table coreTable = lua.create_table();
        coreTable.set_function("createDisplayObject", [](sol::this_state ts, sol::object /*self*/, const std::string& typeName, const sol::table& cfg) {
            sol::state_view sv = ts;
            DisplayObject h = Core::getInstance().createDisplayObject(typeName, cfg);
            return sol::make_object(sv, h);
        });
        // Robust CoreForward.getDisplayObject: accept either dot or colon call
        // shapes. We accept an explicit `self` (may be the table when called via
        // colon) followed by the name, or a single-name arg when called as a
        // plain function. Use variadic_args for flexibility.
        coreTable.set_function("getDisplayObject", [](sol::this_state ts, sol::object maybeSelf, sol::variadic_args va) {
            sol::state_view sv = ts;
            std::string name;

            // If maybeSelf is a string then caller used Core.getDisplayObject(name)
            if (maybeSelf.is<std::string>()) {
                try { name = maybeSelf.as<std::string>(); } catch(...) { name.clear(); }
            } else {
                // Otherwise, the name should be the first variadic arg (colon-call or dot-call with table self)
                if (va.size() >= 1) {
                    try { name = (*(va.begin())).as<std::string>(); } catch(...) { name.clear(); }
                }
            }

            // If still empty, and there are at least two args (self,name) try the second
            if (name.empty() && va.size() >= 2) {
                auto it = va.begin(); ++it; try { name = (*it).as<std::string>(); } catch(...) { name.clear(); }
            }

            if (name.empty()) return sol::make_object(sv, sol::lua_nil);
            DisplayObject h = getCore().getDisplayObject(name);
            // Always return a DisplayObject handle to Lua. Returning raw
            // Stage* userdata can cause sol2 type confusion when the same
            // object is later passed into other bindings expecting a
            // different registered usertype. Using the DisplayObject handle
            // keeps the Lua API stable and consistent.
            // try { std::cout << "[dbg:CoreForward.getDisplayObject] returning DisplayObject handle" << std::endl; } catch(...){ }
            return sol::make_object(sv, h);
        });
        coreTable.set_function("getStageHandle", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts;
            DisplayObject h = Core::getInstance().getStageHandle();
            // Return the DisplayObject handle instead of raw Stage*.
            if (h.isValid() && h.get()) {
                return sol::make_object(sv, h);
            }
            return sol::make_object(sv, DisplayObject());
        });
        // Allow setting the root node by name or by handle from Lua
        coreTable.set_function("setRootNode", [](sol::this_state ts, sol::object /*self*/, sol::object maybeArg) {
            sol::state_view sv = ts;
            if (!maybeArg.valid()) return sol::make_object(sv, sol::nil);
            if (maybeArg.is<std::string>()) {
                std::string name = maybeArg.as<std::string>();
                setRootNodeByName_lua(name);
                return sol::make_object(sv, sol::nil);
            }
                if (maybeArg.is<DisplayObject>()) {
                    DisplayObject h = maybeArg.as<DisplayObject>();
                    setRootNode_lua(h);
                    return sol::make_object(sv, sol::nil);
                }
            // If a table or other type was passed, try to resolve as DisplayObject
            if (maybeArg.is<sol::table>()) {
                sol::table t = maybeArg.as<sol::table>();
                // try to get name field
                if (t["name"].valid()) {
                    try { std::string name = t.get<std::string>("name"); setRootNodeByName_lua(name); return sol::make_object(sv, sol::nil); } catch(...) {}
                }
            }
            return sol::make_object(sv, sol::nil);
        });
        coreTable.set_function("hasDisplayObject", [](sol::this_state /*ts*/, sol::object /*self*/, const std::string& name) {
            return Core::getInstance().hasDisplayObject(name);
        });
        coreTable.set_function("destroyDisplayObject", [](sol::this_state /*ts*/, sol::object /*self*/, const std::string& name) {
            Core::getInstance().destroyDisplayObject(name);
        });

        coreTable.set_function("get_delta_time", [](sol::this_state /*ts*/, sol::object /*self*/) {
            return Core::getInstance().getElapsedTime();
        });
        // Backwards-compatible alias used by some Lua examples/tests
        coreTable.set_function("getElapsedTime", [](sol::this_state /*ts*/, sol::object /*self*/) {
            return Core::getInstance().getElapsedTime();
        });
        coreTable.set_function("getWindowTitle", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts; return sol::make_object(sv, Core::getInstance().getWindowTitle());
        });
        coreTable.set_function("setWindowTitle", [](sol::this_state ts, sol::object /*self*/, const std::string& title) {
            sol::state_view sv = ts; Core::getInstance().setWindowTitle(title); return sol::make_object(sv, sol::nil);
        });
        // Expose keyboard focus getters/setters to Lua
        coreTable.set_function("getKeyboardFocusedObject", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts;
            DisplayObject h = Core::getInstance().getKeyboardFocusedObject();
            if (h.isValid() && h.get()) return sol::make_object(sv, h);
            return sol::make_object(sv, DisplayObject());
        });
        coreTable.set_function("setKeyboardFocusedObject", [](sol::this_state ts, sol::object /*self*/, sol::object maybeArg) {
            sol::state_view sv = ts;
            if (!maybeArg.valid()) return sol::make_object(sv, sol::nil);
            if (maybeArg.is<DisplayObject>()) {
                DisplayObject h = maybeArg.as<DisplayObject>();
                Core::getInstance().setKeyboardFocusedObject(h);
                return sol::make_object(sv, sol::nil);
            }
            if (maybeArg.is<std::string>()) {
                std::string name = maybeArg.as<std::string>();
                DisplayObject h = Core::getInstance().getDisplayObject(name);
                if (h.isValid()) Core::getInstance().setKeyboardFocusedObject(h);
                return sol::make_object(sv, sol::nil);
            }
            return sol::make_object(sv, sol::nil);
        });
        coreTable.set_function("getIsTraversing", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts; return sol::make_object(sv, getIsTraversing_lua());
        });
        coreTable.set_function("setIsTraversing", [](sol::this_state ts, sol::object /*self*/, bool v) {
            sol::state_view sv = ts; setIsTraversing_lua(v); return sol::make_object(sv, sol::nil);
        });

        // Orphan lifecycle helpers exposed to Lua
        coreTable.set_function("countOrphanedDisplayObjects", [](sol::this_state /*ts*/, sol::object /*self*/) {
            return Core::getInstance().countOrphanedDisplayObjects();
        });
        coreTable.set_function("getOrphanedDisplayObjects", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts;
            auto orphans = Core::getInstance().getOrphanedDisplayObjects();
            sol::table t = sv.create_table();
            for (size_t i = 0; i < orphans.size(); ++i) t[i+1] = orphans[i];
            return sol::make_object(sv, t);
        });
        coreTable.set_function("destroyOrphanedDisplayObjects", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts; Core::getInstance().destroyOrphanedDisplayObjects(); return sol::make_object(sv, sol::nil);
        });
        // Allow Lua to trigger factory garbage collection
        coreTable.set_function("collectGarbage", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts; Core::getInstance().getFactory().collectGarbage(); return sol::make_object(sv, sol::nil);
        });
        // Expose setStage (alias) to Lua
        coreTable.set_function("setStage", [](sol::this_state ts, sol::object /*self*/, const std::string& name) {
            sol::state_view sv = ts; setStageByName_lua(name); return sol::make_object(sv, sol::nil);
        });

        coreTable.set_function("pumpEventsOnce", [](sol::this_state /*ts*/, sol::object /*self*/) {
            Core::getInstance().pumpEventsOnce();
        });
        coreTable.set_function("pushMouseEvent", [](sol::this_state ts, sol::object /*self*/, sol::object args) {
            sol::state_view sv = ts;
            pushMouseEvent_lua(args);
            return sol::make_object(sv, sol::nil);
        });
        coreTable.set_function("pushKeyboardEvent", [](sol::this_state ts, sol::object /*self*/, sol::object args) {
            sol::state_view sv = ts;
            pushKeyboardEvent_lua(args);
            return sol::make_object(sv, sol::nil);
        });

        // Expose mouse-hovered object getters/setters to Lua
        coreTable.set_function("getMouseHoveredObject", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts;
            DisplayObject h = Core::getInstance().getMouseHoveredObject();
            if (h.isValid() && h.get()) return sol::make_object(sv, h);
            return sol::make_object(sv, DisplayObject());
        });
        coreTable.set_function("setMouseHoveredObject", [](sol::this_state ts, sol::object /*self*/, sol::object maybeArg) {
            sol::state_view sv = ts;
            if (!maybeArg.valid()) return sol::make_object(sv, sol::nil);
            if (maybeArg.is<DisplayObject>()) {
                DisplayObject h = maybeArg.as<DisplayObject>();
                Core::getInstance().setMouseHoveredObject(h);
                return sol::make_object(sv, sol::nil);
            }
            if (maybeArg.is<std::string>()) {
                std::string name = maybeArg.as<std::string>();
                DisplayObject h = Core::getInstance().getDisplayObject(name);
                if (h.isValid()) Core::getInstance().setMouseHoveredObject(h);
                return sol::make_object(sv, sol::nil);
            }
            return sol::make_object(sv, sol::nil);
        });
        coreTable.set_function("run", [](sol::this_state ts, sol::object /*self*/, sol::optional<sol::object> maybeArg) {
            sol::state_view sv = ts;
            if (!maybeArg) { Core::getInstance().run(); return sol::make_object(sv, sol::nil); }
            sol::object arg = *maybeArg;
            if (arg.is<sol::table>()) { Core::getInstance().run(arg.as<sol::table>()); return sol::make_object(sv, sol::nil); }
            if (arg.is<std::string>()) { Core::getInstance().run(arg.as<std::string>()); return sol::make_object(sv, sol::nil); }
            ERROR("CoreForward.run: unsupported argument type; expected table or string");
            return sol::make_object(sv, sol::nil);
        });

        // Tab-key navigation helpers
        coreTable.set_function("handleTabKeyPress", [](sol::this_state /*ts*/, sol::object /*self*/) {
            Core::getInstance().handleTabKeyPress();
        });
        coreTable.set_function("handleTabKeyPressReverse", [](sol::this_state /*ts*/, sol::object /*self*/) {
            Core::getInstance().handleTabKeyPressReverse();
        });

        coreTable.set_function("configure", [](sol::this_state ts, sol::object /*self*/, const sol::table& cfg) {
            sol::state_view sv = ts; Core::getInstance().configureFromLua(cfg); return sol::make_object(sv, sol::nil);
        });
        coreTable.set_function("configureFromFile", [](sol::this_state ts, sol::object /*self*/, const std::string& filename) {
            sol::state_view sv = ts; Core::getInstance().configureFromLuaFile(filename); return sol::make_object(sv, sol::nil);
        });

        coreTable.set_function("registerOnInit", [](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
            registerOnInit_lua([f]() -> bool { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua CoreForward.registerOnInit error: ") + err.what()); return false; } try { return r.get<bool>(); } catch (...) { return false; } });
            return sol::make_object(ts, sol::nil);
        });

        coreTable.set_function("registerOn", [](sol::this_state ts, sol::object /*self*/, const std::string& name, const sol::function& f) {
            registerOn_lua(name, f);
            return sol::make_object(ts, sol::nil);
        });
        coreTable.set_function("registerOnQuit", [](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
            registerOnQuit_lua([f]() { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua CoreForward.registerOnQuit error: ") + err.what()); } });
            return sol::make_object(ts, sol::nil);
        });
        coreTable.set_function("registerOnUpdate", [](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
            registerOnUpdate_lua([f](float dt) { sol::protected_function pf = f; sol::protected_function_result r = pf(dt); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua CoreForward.registerOnUpdate error: ") + err.what()); } });
            return sol::make_object(ts, sol::nil);
        });
        coreTable.set_function("registerOnEvent", [](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
            registerOnEvent_lua([f](const Event& e) { sol::protected_function pf = f; sol::protected_function_result r = pf(e); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua CoreForward.registerOnEvent error: ") + err.what()); } });
            return sol::make_object(ts, sol::nil);
        });
        coreTable.set_function("registerOnRender", [](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
            registerOnRender_lua([f]() { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua CoreForward.registerOnRender error: ") + err.what()); } });
            return sol::make_object(ts, sol::nil);
        });
        coreTable.set_function("registerOnUnitTest", [](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
            registerOnUnitTest_lua([f]() -> bool { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua CoreForward.registerOnUnitTest error: ") + err.what()); return false; } try { return r.get<bool>(); } catch (...) { return false; } });
            return sol::make_object(ts, sol::nil);
        });
        coreTable.set_function("registerOnWindowResize", [](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
            registerOnWindowResize_lua([f](int w, int h) { sol::protected_function pf = f; sol::protected_function_result r = pf(w, h); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua CoreForward.registerOnWindowResize error: ") + err.what()); } });
            return sol::make_object(ts, sol::nil);
        });

        // Expose quit/shutdown on the CoreForward table for convenience and
        // also expose them as globals so older Lua scripts that call
        // `quit()` or `shutdown()` continue to work.
        coreTable.set_function("quit", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts;
            quit_lua();
            return sol::make_object(sv, true);
        });
        coreTable.set_function("shutdown", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts;
            shutdown_lua();
            return sol::make_object(sv, true);
        });

        // Backwards-compatible globals
        try {
            lua["quit"] = &quit_lua;
            lua["shutdown"] = &shutdown_lua;
        } catch (...) {}

        // Provide a convenience global `getStage()` function for older Lua scripts
        try {
            lua["getStage"] = [](sol::this_state ts) {
                sol::state_view sv = ts;
                DisplayObject h = Core::getInstance().getStageHandle();
                if (h.isValid() && h.get()) return sol::make_object(sv, h);
                return sol::make_object(sv, DisplayObject());
            };
        } catch (...) {}

    // Expose CoreForward (explicit) and make the global `Core` point to
    // the forwarding table so scripts can use the table-based API
    // consistently. The forwarding table dispatches to the Core
    // singleton internally so both dot- and colon-call styles are
    // supported.
    lua["CoreForward"] = coreTable;
    lua["Core"] = coreTable;

    } // End Core::_registerDisplayObject()


} // namespace SDOM



