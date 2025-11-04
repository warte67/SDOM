// SDOM_Core.cpp
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <filesystem>

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Version.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_SDL_Utils.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Utils.hpp> // for parseColor
#include <SDOM/SDOM_Core_Lua.hpp>
#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_ArrowButton.hpp>


namespace SDOM
{

    Core::Core() : IDataObject()
    {
        // Open base libraries plus package and io so embedded scripts can use
        // `require`/`package` and basic I/O if desired by examples.
        lua_.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::package, sol::lib::io);

        // Register Core, Factory, EventManager, Version, and SDL_Utils usertypes
        SDL_Utils::registerLuaBindings(lua_);
        factory_ = new Factory();
        eventManager_ = new EventManager();
        version_ = new Version(lua_);

        // Expose CLR constants and helpers to Lua through a single helper
        // so updates to CLR are reflected in embedded Lua states.
        CLR::exposeToLua(lua_);

        
        // register the DisplayHandle handle last so other types can use it
        DisplayHandle prototypeHandle; // Default DisplayHandle for registration
        prototypeHandle.registerLuaBindings("DisplayHandle", lua_);

        AssetHandle prototypeAssetHandle; // Default AssetHandle for registration
        prototypeAssetHandle._registerLuaBindings("AssetHandle", lua_);
        
        // Register Core usertype
        this->_registerLuaBindings("Core", lua_);       

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
        if (version_)
        {
            delete version_;
            version_ = nullptr;
        }
    }

    void Core::configure(const CoreConfig& config)
    {
        // initialize or reconfigure SDL as needed
        reconfigure(config);
        // Adopt the new configuration as current so getters reflect latest values
        // even when SDL was already started.
        config_ = config;
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
            // this->_registerLuaBindings("Core", lua_);

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
        if (TTF_WasInit()) 
        {
            Factory& factory = getFactory();
            factory.closeAllTruetypeAssets_();
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
                // this->_registerLuaBindings("Core", lua_);
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
        if (!sdlStarted_)
        {
            config_.windowWidth = config.windowWidth;
            config_.windowHeight = config.windowHeight;
            config_.windowFlags = config.windowFlags;
            config_.rendererLogicalPresentation = config.rendererLogicalPresentation;
            config_.pixelWidth = config.pixelWidth;
            config_.pixelHeight = config.pixelHeight;
            config_.pixelFormat = config.pixelFormat;
            config_.preserveAspectRatio = config.preserveAspectRatio;
            config_.allowTextureResize = config.allowTextureResize;
        }
        // Color does not require SDL resource recreation; always adopt latest.
        // This allows Lua configs or runtime updates to immediately affect the
        // window clear (letterbox) color without a full reconfigure cycle.
        config_.color = config.color;

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

        // Before destroying SDL resources, proactively notify all display objects
        // so they can release cached renderer-owned resources (textures) while
        // the old renderer is still valid. This avoids double-destroy crashes
        // when objects try to free textures after the renderer has been torn down.
        // NOTE: Objects should NOT attempt to validate textures by binding them
        // as render targets during teardown. Use SDL_GetTextureSize and/or a
        // cached renderer pointer to decide whether to drop the cache.
        if ((recreate_window || recreate_renderer || recreate_texture) && rootNode_)
        {
            if (auto* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get()))
            {
                std::function<void(IDisplayObject&)> visitUnload;
                visitUnload = [&](IDisplayObject& node)
                {
                    try { node.onUnload(); } catch(...) {}
                    for (const auto& ch : node.getChildren())
                    {
                        if (auto* c = dynamic_cast<IDisplayObject*>(ch.get()))
                            visitUnload(*c);
                    }
                };
                visitUnload(*rootObj);
            }
        }

        // destroy in reverse order
        if (recreate_texture && texture_) 
        {
            getFactory().unloadAllAssetObjects(); // unload all assets to ensure compatibility with new SDL resources
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }
        if (recreate_renderer && renderer_) 
        {
            getFactory().unloadAllAssetObjects(); // unload all assets to ensure compatibility with new SDL resources
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
        }
        if (recreate_window && window_) 
        {
            getFactory().unloadAllAssetObjects(); // unload all assets to ensure compatibility with new SDL resources
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }

        // recreate in normal order
        if (recreate_window && !window_) 
        {
            window_ = SDL_CreateWindow(getWindowTitle().c_str(), config_.windowWidth, config_.windowHeight, config_.windowFlags);
            if (!window_) 
            {
                std::string errorMsg = "SDL_CreateWindow() Error: " + std::string(SDL_GetError());
                ERROR(errorMsg);
            }
            SDL_ShowWindow(window_);     // not needed in SDL3, but included for clarity
            SDL_SyncWindow(window_);
        }
        if (recreate_renderer && !renderer_) 
        {
            renderer_ = SDL_CreateRenderer(window_, nullptr);
            if (!renderer_) {
                std::string errorMsg = "SDL_CreateRenderer() Error: " + std::string (SDL_GetError());
                ERROR(errorMsg);
            }
        }
        if (recreate_texture && !texture_) 
        {
            // compute texture size safely (avoid divide-by-zero and ensure >=1)
            int tWidth = 1;
            int tHeight = 1;
            if (config_.pixelWidth != 0.0f) tWidth = std::max(1, static_cast<int>(config_.windowWidth / config_.pixelWidth));
            if (config_.pixelHeight != 0.0f) tHeight = std::max(1, static_cast<int>(config_.windowHeight / config_.pixelHeight));
            texture_ = SDL_CreateTexture(renderer_,
                config_.pixelFormat,
                SDL_TEXTUREACCESS_TARGET,
                tWidth,
                tHeight);
            if (!texture_) 
            {
                std::string errorMsg = "SDL_CreateTexture() Error: " + std::string(SDL_GetError());
                ERROR(errorMsg);
            }
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);   
            SDL_SetRenderLogicalPresentation(renderer_, tWidth, tHeight, config.rendererLogicalPresentation);
        }

        if (recreate_window || recreate_renderer || recreate_texture) 
        {
            getFactory().reloadAllAssetObjects(); // reload all assets to ensure compatibility with new SDL resources
        }

        // NOTE: Do not force window position here; unit tests and compositors manage
        // positioning, and forcing coordinates can interfere with event generation.
        
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
        // reconfigure(cfg);


        config_ = cfg;
        refreshSDLResources();
        
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

            SDL_Event event;


            clearKeyboardFocusedObject(); // ensure no keyboard focus at start of run loop

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
                        onWindowResize(newWidth, newHeight);
                    }

                    // TEMPORARY: Use [Q] to quit globally
                        if (event.type == SDL_EVENT_KEY_DOWN) 
                            if (event.key.key == SDLK_Q) 
                                bIsRunning_ = false;

                        if (event.type == SDL_EVENT_KEY_DOWN) 
                        {
                            if (event.key.key == SDLK_F) 
                            {
                                // DEBUG_LOG("Core::run: F pressed");
                                setFullscreen(isWindowed());
                            }
                        }
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
                            keyboardFocusedObject_.reset(); //  keyboard focus
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

                // Flush any events queued programmatically (e.g., unit tests) even
                // when no real SDL events were polled this frame. This prevents
                // synthetic motion/click events from stalling until the next real
                // input arrives (e.g., when the mouse cursor is outside the window).
                if (eventManager_)
                {
                    eventManager_->DispatchQueuedEvents();
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

                // Apply any pending configuration requested from other threads
                applyPendingConfig();                

                // update timing
                lastTime = currentTime;

                factory_->detachOrphans();          // Detach orphaned display objects
                factory_->attachFutureChildren();   // Attach future children
                factory_->collectGarbage();         // Clean up any orphaned objects

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

    
    bool Core::onInit()
    {
        // Note: We do not need to run onInit() for each object here because
        // the Factory creates each object and calls onInit() as it does so.

        bool ret = true;

        // // Expose CLR constants and helpers to Lua through a single helper
        // // so updates to CLR are reflected in embedded Lua states.
        // sol::state& lua = this->getLua(); // get the Lua state
        // CLR::exposeToLua(lua);

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

        // Collect any pending events
        pumpEventsOnce();

        // Clean up any orphaned display objects
        getFactory().collectGarbage();

        // Clear the factory registry
        getFactory().clear();

        // Shutdown SDL first; Lua state will be torn down by sol::state's
        // destructor after all sol::reference objects are destroyed.
        shutdown_SDL();

        // Clean up any orphaned display objects
        getLua().collect_garbage();
        getLua().collect_garbage();
    } // END: Core::onQuit()

    void Core::onRender()
    {
        SDL_Renderer* renderer = getRenderer();
        if (!renderer)
            ERROR("Core::onRender() Error: Renderer is null.");

        // Lambda for recursive render handling using std::function
        std::function<void(IDisplayObject&)> handleRender;
        SDL_Texture* texture = texture_;
        IDisplayObject* activeRoot = dynamic_cast<IDisplayObject*>(rootNode_.get());
        handleRender = [this, &handleRender, renderer, texture, activeRoot](IDisplayObject& node) 
        {
            // Skip nested Stage subtrees when a different Stage is active.
            if (dynamic_cast<Stage*>(&node) && (&node != activeRoot))
                return;
            // render the node
            getFactory().start_render_time(node.getName());
            node.onRender();
            getFactory().stop_render_time(node.getName());

            node.setDirty(false); // clear dirty flag after rendering

            DisplayHandle rootHandle = this->getStageHandle();
            // PreRender EventListeners
            if (node.hasEventListener(EventType::OnPreRender, false))
            {
                auto preRenderEv = std::make_unique<Event>(EventType::OnPreRender, rootHandle);
                preRenderEv->setElapsedTime(this->getElapsedTime());
                preRenderEv->setRelatedTarget(rootHandle);
                eventManager_->dispatchEvent(std::move(preRenderEv), DisplayHandle(rootHandle.getName(), rootHandle.getType()));
            }

            // sort children by z-order if needed
            node.sortByZOrder();       

            // render children
            for (const auto& child : node.getChildren()) 
            {
                auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
                if (childObj) 
                {
                    // Do not traverse into other Stage subtrees
                    if (dynamic_cast<Stage*>(childObj) && (childObj != activeRoot))
                        continue;
                    // ensure the render target is set correctly before rendering
                    if (texture)
                        SDL_SetRenderTarget(renderer, texture);
                    handleRender(*childObj);
                    // Render a border if the Box has keyboard focus
                    if (childObj->isKeyboardFocused())
                    {
                        // properly flash the key focus indication border rectangle
                        SDL_FRect rect = { float(childObj->getX()), float(childObj->getY()), float(childObj->getWidth()), float(childObj->getHeight()) };
                        SDL_Color focusColor = { (Uint8)keyfocus_gray_, (Uint8)keyfocus_gray_, (Uint8)keyfocus_gray_, 128 }; // Gray color for focus
                        SDL_SetRenderDrawColor(getRenderer(), focusColor.r, focusColor.g, focusColor.b, focusColor.a); // Border color
                        SDL_RenderRect(getRenderer(), &rect);
                    }
                }
            }

            // OnRender event listener
            if (node.hasEventListener(EventType::OnRender, false))
            {
                auto renderEv = std::make_unique<Event>(EventType::OnRender, rootHandle);
                renderEv->setElapsedTime(this->getElapsedTime());
                renderEv->setRelatedTarget(rootHandle);
                eventManager_->dispatchEvent(std::move(renderEv), DisplayHandle(rootHandle.getName(), rootHandle.getType()));        
            }
        };

        SDL_SetRenderTarget(renderer, nullptr);

        // Clear the entire window to the border color
        SDL_Color color = getColor();
// color.r = 255; color.g = 0; color.b = 255; color.a = 255;
// INFO("Core::onRender: color=" << std::to_string(color.r) << "," << std::to_string(color.g) << "," << std::to_string(color.b) << "," << std::to_string(color.a));
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);

        // set the render target to the proper background texture
        if (texture_)
            SDL_SetRenderTarget(renderer, texture_); 

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
    } // END: Core::onRender()

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
        IDisplayObject* activeRootU = dynamic_cast<IDisplayObject*>(rootNode_.get());
        handleUpdate = [this, &handleUpdate, fElapsedTime, activeRootU](IDisplayObject& node) 
        {
            // Skip nested Stage subtrees when a different Stage is active.
            if (dynamic_cast<Stage*>(&node) && (&node != activeRootU))
                return;

            // Dispatch to event listeners first so they can stopPropagation if needed
            if (node.hasEventListener(EventType::OnUpdate, false))
            {
                DisplayHandle rootNode = this->getStageHandle();
                auto updateEv = std::make_unique<Event>(EventType::OnUpdate, rootNode);
                updateEv->setElapsedTime(fElapsedTime);
                updateEv->setRelatedTarget(rootNode);
                eventManager_->dispatchEvent(std::move(updateEv), rootNode);
            }
            
            // dispatch to the object::onUpdate()
            getFactory().start_update_timer(node.getName());
            node.onUpdate(fElapsedTime);
            getFactory().stop_update_timer(node.getName());

            // update children
            for (const auto& child : node.getChildren()) 
            {
                auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
                if (childObj) 
                {
                    // Do not traverse into other Stage subtrees
                    if (dynamic_cast<Stage*>(childObj) && (childObj != activeRootU))
                        continue;
                    handleUpdate(*childObj);                    
                }
            }
        };

        // Update the unit test frame counter and run tests
        {   
            static int s_iteration_frame = 0;
            static bool s_tests_complete = false;
            static int s_idle_frames = 0;
            constexpr int FRAMES_FOR_IDLE = 250;

            UnitTests& ut = UnitTests::getInstance();


            std::function<bool(IDisplayObject*)> is_any_dirty;
            is_any_dirty = [&](IDisplayObject* node) -> bool
            {
                if (!node)
                    return false;

                if (node->isDirty())
                    return true;

                for (const auto& childHandle : node->getChildren())
                {
                    auto* child = dynamic_cast<IDisplayObject*>(childHandle.get());
                    if (child && is_any_dirty(child))
                        return true;
                }

                return false;
            };


            // FRAME 0 — setup
            if (s_iteration_frame == 0)
            {
                std::cout << CLR::fg_rgb(160, 160, 255)
                        << "🔄 Beginning sequential unit test run..."
                        << CLR::RESET << std::endl;
            }
            // FRAME 1 — registration
            else if (s_iteration_frame == 1)
            {
                onUnitTest(s_iteration_frame);
            }
            // FRAME 2+ — sequential execution
            else
            {
                ut.update();

                if (!s_tests_complete && ut.all_done())
                {
                    s_tests_complete = true;
                    s_idle_frames = 0; // start counting idle frames
                    // INFO("✅ All tests complete — entering performance settling period.");
                }
            }

            // If tests are complete, wait for FRAMES_FOR_IDLE clean frames
            if (s_tests_complete)
            {
                IDisplayObject* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get());
                bool dirtyFound = is_any_dirty(rootObj);

                if (!dirtyFound)
                    ++s_idle_frames;
                else
                    s_idle_frames = 0;  // reset idle counter if anyone got messy again

                if (s_idle_frames >= FRAMES_FOR_IDLE)
                {
                    if (stopAfterUnitTests_)
                    {
                        getFactory().report_performance_stats();
                        bIsRunning_ = false;
                    }
                }
            }
            else
            {
                // still running tests, increment global frame counter
                ++s_iteration_frame;
                ut.set_frame_counter(s_iteration_frame);
            }
        } // END: update unit tests

        // Update the Keyfocus Gray
        static float delta = 1.0f;
        float speed = 500.0f;
        keyfocus_gray_ += (fElapsedTime * speed * delta);
        if (keyfocus_gray_ >= 192.0f) 
        {
            keyfocus_gray_ = 192.0f;
            delta = -1.0f;
        }
        else if (keyfocus_gray_ <= 64.0f) 
        {
            keyfocus_gray_ = 64.0f;
            delta = 1.0f;
        }        

        // Clear last-frame metrics at the start of a new frame's object update
        // so that perf tables reflect work done during this frame. This is
        // placed after the unit-test reporting block so the "stop-after-tests"
        // perf dump still shows the previous frame's values.
        factory_->begin_frame_metrics();

        // Per-frame event manager tick: run hover watchdogs, etc.
        if (eventManager_)
            eventManager_->onFrameTick();

        // Call the users registered update function if available
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

    bool Core::onUnitTest(int frame)
    {
        std::function<bool(IDisplayObject&)> handleUnitTest;
        handleUnitTest = [&handleUnitTest, frame](IDisplayObject& node) -> bool 
        {
            bool finished = node.onUnitTest(frame);

            // Recursively process children — but only if parent’s tests finished
            for (const auto& child : node.getChildren()) 
            {
                if (auto* childObj = dynamic_cast<IDisplayObject*>(child.get()))
                {
                    // Combine results with logical AND, but do not short-circuit
                    finished = handleUnitTest(*childObj) && finished;
                }
            }

            return finished;
        };

        bool allFinished = true;

        // --- Run Core-specific tests ---
        allFinished &= coreTests_();                // Core unit tests
        allFinished &= factory_->onUnitTest(frame); // Factory tests

        // --- Run user-registered onUnitTest callback ---
        if (fnOnUnitTest)
            allFinished &= fnOnUnitTest();          // Return false if still waiting

        // --- Walk the display tree recursively ---
        if (auto* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get()))
        {
            setIsTraversing(true);
            allFinished &= handleUnitTest(*rootObj);
            setIsTraversing(false);
        }

        // Return true only when ALL tests (including re-entrant ones) are done
        return allFinished;
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

        // Notify all display objects so cached renderer-owned resources can be
        // proactively invalidated/rebuilt after device changes.
        if (rootNode_)
        {
            IDisplayObject* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get());
            if (rootObj)
            {
                std::function<void(IDisplayObject&)> visit;
                visit = [&](IDisplayObject& node)
                {
                    try { node.onWindowResize(newWidth, newHeight); } catch(...) {}
                    for (const auto& ch : node.getChildren())
                    {
                        if (auto* c = dynamic_cast<IDisplayObject*>(ch.get()))
                            visit(*c);
                    }
                };
                visit(*rootObj);
            }
        }
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
    bool Core::isFullscreen() const { if (SDL_GetWindowFlags(getWindow()) & SDL_WINDOW_FULLSCREEN) return true; return false; }
    bool Core::isWindowed() const { return !isFullscreen(); }
    void Core::setConfig(CoreConfig& config)            { config_ = config; refreshSDLResources(); }

    void Core::setWindowWidth(float width)              { 
        config_.windowWidth = width; 
        refreshSDLResources(); 
    }
    void Core::setWindowHeight(float height)            { config_.windowHeight = height; refreshSDLResources(); }
    void Core::setPixelWidth(float width)               { config_.pixelWidth = width; refreshSDLResources(); }
    void Core::setPixelHeight(float height)             { config_.pixelHeight = height; refreshSDLResources(); }
    void Core::setPreserveAspectRatio(bool preserve)    { config_.preserveAspectRatio = preserve; refreshSDLResources(); }
    void Core::setAllowTextureResize(bool allow)        { config_.allowTextureResize = allow; refreshSDLResources(); }
    void Core::setWindowFlags(SDL_WindowFlags flags)    { config_.windowFlags = flags; refreshSDLResources(); }
    void Core::setPixelFormat(SDL_PixelFormat format)   { config_.pixelFormat = format; refreshSDLResources(); }

    void Core::setFullscreen(bool fullscreen) {
        // Prefer native fullscreen toggle without tearing down GPU resources.
        // This allows SDL to emit WINDOW_ENTER/LEAVE_FULLSCREEN and RESIZED
        // events while keeping the existing renderer/texture alive.

        SDL_Window* win = getWindow();
        if (!win)
            return;

        // Track current flags
        bool currentlyFullscreen = (SDL_GetWindowFlags(win) & SDL_WINDOW_FULLSCREEN) != 0;
        if (fullscreen == currentlyFullscreen)
            return; // no-op

        if (fullscreen)
        {
            // Remember windowed size before entering fullscreen
            int w = 0, h = 0;
            if (SDL_GetWindowSize(win, &w, &h))
            {
                saved_window_width_ = static_cast<float>(w);
                saved_window_height_ = static_cast<float>(h);
            }
            // Toggle fullscreen without recreating resources
            SDL_SetWindowFullscreen(win, true);
            SDL_SyncWindow(win);
            // Update config flags to reflect new state
            config_.windowFlags |= SDL_WINDOW_FULLSCREEN;
        }
        else
        {
            // Leave fullscreen
            SDL_SetWindowFullscreen(win, false);
            SDL_SyncWindow(win);
            config_.windowFlags &= ~SDL_WINDOW_FULLSCREEN;

            // Restore prior windowed size if we have one
            if (saved_window_width_ > 0.0f && saved_window_height_ > 0.0f)
            {
                int iw = static_cast<int>(saved_window_width_);
                int ih = static_cast<int>(saved_window_height_);
                SDL_SetWindowSize(win, iw, ih);
                SDL_SyncWindow(win);
                config_.windowWidth = saved_window_width_;
                config_.windowHeight = saved_window_height_;
            }
        }
        // Do not call requestConfigApply(cfg); avoid full rebuild on toggle.
    }
    void Core::setWindowed(bool windowed) { setFullscreen(!windowed); }



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

        // After reconfigure, proactively notify all display objects that the
        // device/resources have been recreated. Give objects a chance to
        // (re)load renderer-backed caches, then broadcast logical size change.
        int logicalW = 0, logicalH = 0;
        if (texture_)
        {
            float tw = 0.0f, th = 0.0f;
            if (SDL_GetTextureSize(texture_, &tw, &th))
            {
                logicalW = static_cast<int>(tw);
                logicalH = static_cast<int>(th);
            }
        }
        if (rootNode_)
        {
            if (auto* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get()))
            {
                // First pass: onLoad on the rebuilt device
                std::function<void(IDisplayObject&)> visitLoad;
                visitLoad = [&](IDisplayObject& node)
                {
                    try { node.onLoad(); } catch(...) {}
                    for (const auto& ch : node.getChildren())
                    {
                        if (auto* c = dynamic_cast<IDisplayObject*>(ch.get()))
                            visitLoad(*c);
                    }
                };
                visitLoad(*rootObj);

                // Second pass: broadcast logical size to trigger layout/cache rebuilds
                std::function<void(IDisplayObject&)> visit;
                visit = [&](IDisplayObject& node)
                {
                    try { node.onWindowResize(logicalW, logicalH); } catch(...) {}
                    for (const auto& ch : node.getChildren())
                    {
                        if (auto* c = dynamic_cast<IDisplayObject*>(ch.get()))
                            visit(*c);
                    }
                };
                visit(*rootObj);
            }
        }
    } // END void Core::refreshSDLResources()

    bool Core::coreTests_()
    {
        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = "Core System";

        // Only register tests once — e.g., on first frame
        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "SDL_WasInit(SDL_INIT_VIDEO)", [](std::vector<std::string>& errors)
            {
                if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
                    errors.push_back("SDL was NOT initialized!");
                return true; // ✅ finished this frame
            });

            ut.add_test(objName, "SDL Texture Validity", [this](std::vector<std::string>& errors)
            {
                if (!texture_)
                    errors.push_back("SDL Texture is null!");
                return true; // ✅ finished this frame
            });

            ut.add_test(objName, "SDL Renderer Validity", [this](std::vector<std::string>& errors)
            {
                if (!renderer_)
                    errors.push_back("SDL Renderer is null!");
                return true; // ✅ finished this frame
            });

            ut.add_test(objName, "SDL Window Validity", [this](std::vector<std::string>& errors)
            {
                if (!window_)
                    errors.push_back("SDL Window is null!");
                return true; // ✅ finished this frame
            });

            registered = true;
        }

        // ✅ Return false to signal that the overall test set is still active
        // (so that UnitTests::update() continues running until all complete)
        return false;
    }



    void Core::handleTabKeyPress()
    {
        // Collect tab-able objects in a deterministic pre-order into a vector.
        std::vector<std::pair<DisplayHandle,int>> list; // (handle, originalIndex)
        int index = 0;
        std::function<void(DisplayHandle)> collect = [&](DisplayHandle node) 
        {
            if (!node.isValid()) return;
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(node.get());
            if (!obj) return;
            if (obj->isTabEnabled() && obj->getTabPriority() >= 0) 
            {
                list.emplace_back(node, index++);
            }
            for (const auto& child : obj->getChildren()) 
            {
                collect(child);
            }
        };
        collect(getStageHandle());

        if (list.empty()) 
        {
            setKeyboardFocusedObject(DisplayHandle());
            return;
        }

        // Sort by tabPriority ascending, tie-breaker = traversal order (original index)
        std::stable_sort(list.begin(), list.end(),
            [&](const auto& a, const auto& b)
            {
                IDisplayObject* oa = dynamic_cast<IDisplayObject*>(a.first.get());
                IDisplayObject* ob = dynamic_cast<IDisplayObject*>(b.first.get());
                int pa = oa ? oa->getTabPriority() : 0;
                int pb = ob ? ob->getTabPriority() : 0;
                // Prefer higher tabPriority first (so larger priorities are focused earlier)
                if (pa != pb) return pa > pb;
                // Tie-breaker: prefer later traversal items first (higher original index)
                // This preserves previous LIFO-like ordering where recently added children
                // were reached earlier when using the stack-based collection.
                return a.second > b.second;
            });

        // Build compact vector of handles
        std::vector<DisplayHandle> ordered;
        ordered.reserve(list.size());
        for (auto &p : list) ordered.push_back(p.first);

        // find current index
        int idx = -1;
        for (size_t i = 0; i < ordered.size(); ++i) 
        {
            if (ordered[i] == keyboardFocusedObject_) 
            { 
                idx = static_cast<int>(i); break; 
            }
        }
        // next is (idx+1) mod n, if idx == -1 choose 0
        DisplayHandle next = ordered[(idx + 1) % ordered.size()];
        // INFO("Tab Key Pressed: Focusing object: " << (next.isValid() ? next.get()->getName() : "<null>"));
        setKeyboardFocusedObject(next);
    } // END Core::handleTabKeyPress()


    void Core::handleTabKeyPressReverse()
    {
        // Collect tab-able objects in a deterministic pre-order into a vector.
        std::vector<std::pair<DisplayHandle,int>> list; // (handle, originalIndex)
        int index = 0;
        std::function<void(DisplayHandle)> collect = [&](DisplayHandle node) 
        {
            if (!node.isValid()) return;
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(node.get());
            if (!obj) return;
            if (obj->isTabEnabled() && obj->getTabPriority() >= 0) 
            {
                list.emplace_back(node, index++);
            }
            for (const auto& child : obj->getChildren()) 
            {
                collect(child);
            }
        };
        collect(getStageHandle());

        if (list.empty()) 
        {
            setKeyboardFocusedObject(DisplayHandle());
            return;
        }

        // Sort by tabPriority ascending, tie-breaker = traversal order (original index)
        std::stable_sort(list.begin(), list.end(),
            [&](const auto& a, const auto& b)
            {
                IDisplayObject* oa = dynamic_cast<IDisplayObject*>(a.first.get());
                IDisplayObject* ob = dynamic_cast<IDisplayObject*>(b.first.get());
                int pa = oa ? oa->getTabPriority() : 0;
                int pb = ob ? ob->getTabPriority() : 0;
                // Prefer higher tabPriority first (so larger priorities are focused earlier)
                if (pa != pb) return pa > pb;
                // Tie-breaker: prefer later traversal items first (higher original index)
                return a.second > b.second;
            });

        // Build compact vector of handles
        std::vector<DisplayHandle> ordered;
        ordered.reserve(list.size());
        for (auto &p : list) ordered.push_back(p.first);

        // find current index
        int idx = -1;
        for (size_t i = 0; i < ordered.size(); ++i) 
        {
            if (ordered[i] == keyboardFocusedObject_) 
            { 
                idx = static_cast<int>(i); break; 
            }
        }
        // previous is (idx-1 + n) mod n, if idx == -1 choose last
        int n = static_cast<int>(ordered.size());
        DisplayHandle prev = (idx == -1) ? ordered.back() : ordered[(idx - 1 + n) % n];
        // INFO("Tab Key Pressed: Focusing object: " << (prev.isValid() ? prev.get()->getName() : "<null>"));
        setKeyboardFocusedObject(prev);
    } // END Core::handleTabKeyPressReverse()

    void Core::setKeyboardFocusedObject(DisplayHandle obj)
    { keyboardFocusedObject_ = obj; }
    DisplayHandle Core::getKeyboardFocusedObject() const
    { return keyboardFocusedObject_; }
    void Core::clearKeyboardFocusedObject() 
    { setKeyboardFocusedObject(DisplayHandle(nullptr)); }
    void Core::clearMouseHoveredObject() 
    { setMouseHoveredObject(DisplayHandle(nullptr)); }

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
    // void Core::setRootNode(const DisplayHandle& handle) 
    // { 
    //     rootNode_ = handle;     
    //     if (rootNode_.isValid() && rootNode_.get()) setWindowTitle("Stage: " + rootNode_.get()->getName());
    // }
    void Core::setRootNode(const DisplayHandle& newRoot)
    {
        EventManager& em = getEventManager();

        // --- Dispatch StageClosed on the current root ----------------------------
        if (rootNode_.isValid() && rootNode_ != newRoot)
        {
            std::unique_ptr<Event> closedEvent =
                std::make_unique<Event>(EventType::StageClosed, rootNode_);
            closedEvent->setRelatedTarget(rootNode_); // optional, for clarity
            em.dispatchEvent(std::move(closedEvent), rootNode_);
        }

        // --- Swap root node ------------------------------------------------------
        rootNode_ = newRoot;

        // --- Dispatch StageOpened on the new root -------------------------------
        if (rootNode_.isValid())
        {
            std::unique_ptr<Event> openedEvent =
                std::make_unique<Event>(EventType::StageOpened, rootNode_);
            openedEvent->setRelatedTarget(rootNode_);
            em.dispatchEvent(std::move(openedEvent), rootNode_);

            if (rootNode_.get())
                setWindowTitle("Stage: " + rootNode_.get()->getName());
        }
        else
        {
            setWindowTitle("No Active Stage");
        }
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


    void Core::destroyDisplayObject(const std::string& name) {
        getFactory().destroyDisplayObject(name);
    }

    void Core::destroyAssetObject(const std::string& name) {
        getFactory().destroyAssetObject(name);
    }


    int Core::countOrphanedDisplayObjects() const {
        return getFactory().countOrphanedDisplayObjects();
    }
    std::vector<DisplayHandle> Core::getOrphanedDisplayObjects() {
        return getFactory().getOrphanedDisplayObjects();
    }
    void Core::detachOrphans() {
        getFactory().detachOrphans();
    }
    void Core::collectGarbage() { 
        getFactory().collectGarbage(); 
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

    std::vector<std::string> Core::getDisplayObjectNames() const {
        return getFactory().getDisplayObjectNames();
    }
    void Core::clearFactory() {
        getFactory().clear();
    }
    void Core::printObjectRegistry() const {
        getFactory().printObjectRegistry();
    }


    // --- Event helpers --- //
    
    // Test helper: poll and dispatch any pending SDL events once. This mirrors
    // the event handling inside Core::run but returns immediately after one
    // pass. Useful for unit tests that push synthetic SDL events.
    void Core::pumpEventsOnce()
    {
        SDL_Event event;
        // Apply any pending configuration requested from other threads
        applyPendingConfig();
        // verify that eventManager_ is valid
        if (!eventManager_) return;

        // std::cout << "Core::pumpEventsOnce(): Dispatching queued event. Remaining queue size: " << eventManager_->getEventQueueSize() << std::endl;
        while (SDL_PollEvent(&event))
        {
            if (this->getIgnoreRealInput()) {
                if (event.type == SDL_EVENT_MOUSE_MOTION || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP || event.type == SDL_EVENT_MOUSE_WHEEL
                    || event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP || event.type == SDL_EVENT_TEXT_INPUT || event.type == SDL_EVENT_TEXT_EDITING
                    || event.type == SDL_EVENT_WINDOW_FOCUS_GAINED || event.type == SDL_EVENT_WINDOW_FOCUS_LOST
                    || event.type == SDL_EVENT_WINDOW_MOUSE_ENTER || event.type == SDL_EVENT_WINDOW_MOUSE_LEAVE) {
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

    void Core::pushMouseEvent(const sol::object& args)
    {
		Core* c = &Core::getInstance();
		// Expect a table with { x=<stage-x>, y=<stage-y>, type="down"|"up", button=<int> }
		if (!args.is<sol::table>()) return;
		sol::table t = args.as<sol::table>();
		if (!t["x"].valid() || !t["y"].valid()) return;
		float sx = t["x"].get<float>();
		float sy = t["y"].get<float>();
		std::string type = "down";
		if (t["type"].valid()) type = t["type"].get<std::string>();
		int button = 1;
		if (t["button"].valid()) button = t["button"].get<int>();

		// Convert stage/render coords to window coords using SDL_RenderCoordinatesToWindow
		float winX = 0.0f, winY = 0.0f;
		SDL_Renderer* renderer = c->getRenderer();
		if (renderer) 
		{
			SDL_RenderCoordinatesToWindow(renderer, sx, sy, &winX, &winY);
			LUA_INFO("pushMouseEvent_lua: SDL_RenderCoordinatesToWindow stage:(" << sx << "," << sy << ") -> window:(" << winX << "," << winY << ") type:" << type << " button:" << button);
		} 
		else 
		{
			// Fallback: simple scaling (may fail in tiled/letterboxed)
			const Core::CoreConfig& cfg = c->getConfig();
			winX = sx * cfg.pixelWidth;
			winY = sy * cfg.pixelHeight;
			LUA_INFO("pushMouseEvent_lua: Fallback scaling stage:(" << sx << "," << sy << ") -> window:(" << winX << "," << winY << ") type:" << type << " button:" << button);
		}

		// Debug logging for synthetic mouse events
		LUA_INFO("[pushMouseEvent_lua] stage:(" << sx << "," << sy << ") -> window:(" << winX << "," << winY << ") type:" << type << " button:" << button);

        Uint32 winID = 0;
        if (c->getWindow()) winID = SDL_GetWindowID(c->getWindow());

		SDL_Event ev;
		std::memset(&ev, 0, sizeof(ev));
		if (type == "up") {
			ev.type = SDL_EVENT_MOUSE_BUTTON_UP;
			ev.button.windowID = winID;
			ev.button.which = 0;
			ev.button.button = button;
			ev.button.clicks = 1;
			ev.button.x = winX;
			ev.button.y = winY;
			ev.motion.windowID = winID;
			ev.motion.which = 0;
			ev.motion.x = winX;
			ev.motion.y = winY;
		} else if (type == "motion") {
			ev.type = SDL_EVENT_MOUSE_MOTION;
			ev.motion.windowID = winID;
			ev.motion.which = 0;
			ev.motion.x = winX;
			ev.motion.y = winY;
		} else {
			ev.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
			ev.button.windowID = winID;
			ev.button.which = 0;
			ev.button.button = button;
			ev.button.clicks = 1;
			ev.button.x = winX;
			ev.button.y = winY;
			ev.motion.windowID = winID;
			ev.motion.which = 0;
			ev.motion.x = winX;
			ev.motion.y = winY;
		}
		// SDL_PushEvent(&ev);
		c->getEventManager().Queue_SDL_Event(ev);        
    } // END void Core::pushMouseEvent()

    void Core::pushKeyboardEvent(const sol::object& args)
    {
		Core* c = &Core::getInstance();
		if (!args.is<sol::table>()) return;
		sol::table t = args.as<sol::table>();
		if (!t["key"].valid()) return;
		int key = t["key"].get<int>();
		std::string type = "down";
		if (t["type"].valid()) type = t["type"].get<std::string>();
		int mod = 0;
		if (t["mod"].valid()) mod = t["mod"].get<int>();

		SDL_Event ev;
		std::memset(&ev, 0, sizeof(ev));
		if (type == "up") ev.type = SDL_EVENT_KEY_UP;
		else ev.type = SDL_EVENT_KEY_DOWN;

		ev.key.windowID = c->getWindow() ? SDL_GetWindowID(c->getWindow()) : 0;
		ev.key.timestamp = SDL_GetTicks();
		ev.key.repeat = 0;
		ev.key.mod = mod;
		ev.key.key = key;

		// SDL_PushEvent(&ev);
		c->getEventManager().Queue_SDL_Event(ev);
    } // END void Core::pushKeyboardEvent()



    // --- Version Helpers --- //

    std::string Core::getVersionString() const { return version_->toString(); }
    std::string Core::getVersionFullString() const { return version_->fullString(); }
    int Core::getVersionMajor() const { return version_->getMajor(); }
    int Core::getVersionMinor() const { return version_->getMinor(); } 
    int Core::getVersionPatch() const { return version_->getPatch(); }
    std::string Core::getVersionCodename() const { return version_->getCodename(); }
    std::string Core::getVersionBuild() const { return version_->getBuild(); }
    std::string Core::getVersionBuildDate() const { return version_->getBuildDate(); } 
    std::string Core::getVersionCommit() const { return version_->getCommit(); }
    std::string Core::getVersionBranch() const { return version_->getBranch(); } 
    std::string Core::getVersionCompiler() const { return version_->getCompiler(); }
    std::string Core::getVersionPlatform() const { return version_->getPlatform(); }



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

        // Register Core userdata and ensure userdata lookups delegate to the
        // authoritative per-type table. Provide IDataObject as the base so
        // sol2 registers the proper base class information.
        
        // SDOM::IDataObject::ensure_sol_table(lua, typeName);        
        sol::usertype<Core> objHandleType = SDOM::IDataObject::register_usertype_with_table<Core, SDOM::IDataObject>(lua, typeName);

        this->objHandleType_ = objHandleType;   // Save usertype
        // Use the authoritative per-type table as the Core forwarding table
        // so userdata and table-based calls resolve to the same functions.
        sol::table coreTable = SDOM::IDataObject::ensure_sol_table(lua, typeName);
        // Publish an explicit CoreForward entry so callers that restore the
        // Core global after running config scripts can find the forwarding
        // table. Also ensure the global `Core` initially points to this
        // forwarding table if nothing else has set it.
        try {
            lua["CoreForward"] = coreTable;
            sol::object maybeCore = lua.globals().raw_get_or("Core", sol::lua_nil);
            if (!maybeCore.valid() || maybeCore == sol::lua_nil) {
                lua["Core"] = coreTable;
            }
        } catch(...) {}

            

        // --- Register Event types and EventType table (best-effort) --- //
        
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

        // --- Expose IconIndex & IconButton constants early --- //
        try {
            auto make_lua_key = [](const std::string &s) -> std::string {
                std::string out;
                std::size_t i = 0;
                while (i < s.size()) {
                    std::size_t j = s.find('_', i);
                    if (j == std::string::npos) j = s.size();
                    std::string part = s.substr(i, j - i);
                    if (!part.empty()) part[0] = static_cast<char>(std::toupper(part[0]));
                    if (!out.empty()) out += "_";
                    out += part;
                    i = j + 1;
                }
                return out;
            };

            sol::table iconIndexTable = lua.create_table();
            for (const auto &p : SDOM::icon_index_to_string) {
                int idx = p.first;
                const std::string &name = p.second;
                std::string luaKey = make_lua_key(name);
                iconIndexTable.set(luaKey, idx);
            }

            sol::object existing = lua["IconIndex"];
            if (!existing.valid() || existing == sol::lua_nil) {
                lua["IconIndex"] = iconIndexTable;
            } else {
                sol::table t = lua["IconIndex"];
                for (const auto &p : SDOM::icon_index_to_string) {
                    std::string k = make_lua_key(p.second);
                    sol::object cur = t.raw_get<sol::object>(k);
                    if (!cur.valid() || cur == sol::lua_nil) t.set(k, p.first);
                }
            }

            // Also populate a global IconButton table to expose constants
            sol::object maybeClass = lua["IconButton"];
            sol::table classTable;
            if (!maybeClass.valid() || maybeClass == sol::lua_nil) {
                classTable = lua.create_table();
                lua["IconButton"] = classTable;
            } else {
                classTable = maybeClass.as<sol::table>();
            }
            for (const auto &p : SDOM::icon_index_to_string) {
                std::string k = make_lua_key(p.second);
                sol::object cur2 = classTable.raw_get<sol::object>(k);
                if (!cur2.valid() || cur2 == sol::lua_nil) classTable.set(k, p.first);
            }

            // Add "direction" for ArrowButton
            sol::table arrowDirTable = lua.create_table();
            for (const auto& p : SDOM::ArrowButton::arrow_direction_to_string) {
                std::string k = p.second;
                arrowDirTable.set(k, static_cast<int>(p.first));
            }
            lua["ArrowDirection"] = arrowDirTable;

        } catch(...) {
            // Non-fatal: registration is best-effort
        }

        // ---------------------------------------- //
        // --- Register Core Functions with Lua --- //
        // ---------------------------------------- //


        // --- Main Loop & Event Dispatch --- //        
        core_bind_noarg("quit", SDOM::quit, objHandleType, coreTable, lua);
        core_bind_noarg("shutdown", SDOM::shutdown, objHandleType, coreTable, lua);
        // bind_noarg("run", run_lua, objHandleType, coreTable, lua);
        core_bind_table("configure", core_configure_lua, objHandleType, coreTable, lua);
        // Also expose a top-level global `configure(table)` convenience
        // function that forwards to the Core singleton.  The Core method
        // bound into `Core` expects a userdata first-argument (the Core
        // forwarder), so we must expose a separate free function that
        // accepts a table and calls the singleton directly.
        try {
            // Expose a top-level `configure(table)` that forwards to our
            // configure_lua wrapper so the 'resources' preprocessing runs.
            lua.set_function("configure", [](const sol::table& t) {
                SDOM::core_configure_lua(t);
            });
        } catch(...) {}
        core_bind_string("configureFromFile", core_configureFromFile_lua, objHandleType, coreTable, lua);        

	    // --- Callback/Hook Registration --- //
        core_bind_string_function_forwarder("registerOn", core_registerOn_lua, objHandleType, coreTable, lua); // custom handling above

	    // --- Stage/Root Node Management --- //
        SDOM::core_bind_name_or_handle("setRootNodeByName", setRootNodeByName_lua, setRootNode_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_name_or_handle("setRootNode", setRootNodeByName_lua, setRootNode_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_name_or_handle("setRoot", setRootNodeByName_lua, setRootNode_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_name_or_handle("setStageByName", setRootNodeByName_lua, setRootNode_lua, objHandleType, coreTable, lua); // alias of setRootNode()
        SDOM::core_bind_name_or_handle("setStage", setRootNodeByName_lua, setRootNode_lua, objHandleType, coreTable, lua); // alias of setRoot()
        SDOM::core_bind_return_displayobject("getRoot", getRoot_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_displayobject("getRootHandle", getRoot_lua, objHandleType, coreTable, lua);   // alias of getRoot()
        SDOM::core_bind_return_displayobject("getStage", getStage_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_displayobject("getStageHandle", getStage_lua, objHandleType, coreTable, lua); // alias of getStage()

        // --- Factory & EventManager Access --- //
        SDOM::core_bind_return_bool("getIsTraversing", getIsTraversing_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_bool_arg("setIsTraversing", setIsTraversing_lua, objHandleType, coreTable, lua);

        // --- Object Creation --- //
        SDOM::core_bind_string_table_return_do("createDisplayObject", createDisplayObject_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_string_return_do("getDisplayObject", getDisplayObject_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_string_return_bool("hasDisplayObject", hasDisplayObject_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_string_table_return_asset("createAssetObject", createAssetObject_lua, objHandleType, coreTable, lua);

        // Alias for historical/shortcut usage: expose Core:createAsset -> createAssetObject
        SDOM::core_bind_string_table_return_asset("createAsset", createAssetObject_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_string_return_asset("getAssetObject", getAssetObject_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_string_return_bool("hasAssetObject", hasAssetObject_lua, objHandleType, coreTable, lua);

	    // --- Focus & Hover Management --- //
        SDOM::core_bind_noarg("doTabKeyPressForward", doTabKeyPressForward_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_noarg("doTabKeyPressReverse", doTabKeyPressReverse_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_noarg("handleTabKeyPress", doTabKeyPressForward_lua, objHandleType, coreTable, lua); // alias of doTabKeyPressForward()
        SDOM::core_bind_noarg("handleTabKeyPressReverse", doTabKeyPressReverse_lua, objHandleType, coreTable, lua); // alias of doTabKeyPressReverse()
        SDOM::core_bind_do_arg("setKeyboardFocusedObject", setKeyboardFocusedObject_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_noarg("clearKeyboardFocusedObject", clearKeyboardFocusedObject_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_displayobject("getKeyboardFocusedObject", getKeyboardFocusedObject_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_do_arg("setMouseHoveredObject", setMouseHoveredObject_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_displayobject("getMouseHoveredObject", getMouseHoveredObject_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_noarg("clearMouseHoveredObject", clearMouseHoveredObject_lua, objHandleType, coreTable, lua);

        // --- Window/Renderer/Texture/Config (read-only accessors) --- //
        SDOM::core_bind_return_float("getPixelWidth", [](){ return Core::getInstance().getPixelWidth(); }, objHandleType, coreTable, lua);
        SDOM::core_bind_return_float("getPixelHeight", [](){ return Core::getInstance().getPixelHeight(); }, objHandleType, coreTable, lua);
        SDOM::core_bind_return_int("getWindowFlags", [](){ return static_cast<int>(Core::getInstance().getWindowFlags()); }, objHandleType, coreTable, lua);
        SDOM::core_bind_return_int("getPixelFormat", [](){ return static_cast<int>(Core::getInstance().getPixelFormat()); }, objHandleType, coreTable, lua);
        // Additional config getters
        SDOM::core_bind_return_float("getWindowWidth", [](){ return Core::getInstance().getWindowWidth(); }, objHandleType, coreTable, lua);
        SDOM::core_bind_return_float("getWindowHeight", [](){ return Core::getInstance().getWindowHeight(); }, objHandleType, coreTable, lua);
        SDOM::core_bind_return_bool("getPreserveAspectRatio", [](){ return Core::getInstance().getPreserveAspectRatio(); }, objHandleType, coreTable, lua);
        SDOM::core_bind_return_bool("getAllowTextureResize", [](){ return Core::getInstance().getAllowTextureResize(); }, objHandleType, coreTable, lua);
        SDOM::core_bind_return_int("getRendererLogicalPresentation", [](){ return static_cast<int>(Core::getInstance().getRendererLogicalPresentation()); }, objHandleType, coreTable, lua);

        // Setter bindings for pixel metrics and format/flags (accept numeric args)
        objHandleType["setPixelWidth"] = [](Core& /*core*/, float w) { Core::getInstance().setPixelWidth(w); return true; };
        coreTable.set_function("setPixelWidth", [](sol::this_state, sol::object /*self*/, sol::object v){
            try {
                if (v.is<float>()) Core::getInstance().setPixelWidth(v.as<float>());
                else if (v.is<double>()) Core::getInstance().setPixelWidth(static_cast<float>(v.as<double>()));
                else if (v.is<int>()) Core::getInstance().setPixelWidth(static_cast<float>(v.as<int>()));
            } catch(...) {}
        });

        objHandleType["setPixelHeight"] = [](Core& /*core*/, float h) { Core::getInstance().setPixelHeight(h); return true; };
        coreTable.set_function("setPixelHeight", [](sol::this_state, sol::object /*self*/, sol::object v){
            try {
                if (v.is<float>()) Core::getInstance().setPixelHeight(v.as<float>());
                else if (v.is<double>()) Core::getInstance().setPixelHeight(static_cast<float>(v.as<double>()));
                else if (v.is<int>()) Core::getInstance().setPixelHeight(static_cast<float>(v.as<int>()));
            } catch(...) {}
        });

        objHandleType["setWindowFlags"] = [](Core& /*core*/, int flags) { Core::getInstance().setWindowFlags(static_cast<SDL_WindowFlags>(flags)); return true; };
        coreTable.set_function("setWindowFlags", [](sol::this_state, sol::object /*self*/, sol::object v){
            try {
                if (v.is<int>()) Core::getInstance().setWindowFlags(static_cast<SDL_WindowFlags>(v.as<int>()));
                else if (v.is<double>()) Core::getInstance().setWindowFlags(static_cast<SDL_WindowFlags>(static_cast<int>(v.as<double>())));
            } catch(...) {}
        });

        objHandleType["setPixelFormat"] = [](Core& /*core*/, int fmt) { Core::getInstance().setPixelFormat(static_cast<SDL_PixelFormat>(fmt)); return true; };
        coreTable.set_function("setPixelFormat", [](sol::this_state, sol::object /*self*/, sol::object v){
            try {
                if (v.is<int>()) Core::getInstance().setPixelFormat(static_cast<SDL_PixelFormat>(v.as<int>()));
                else if (v.is<double>()) Core::getInstance().setPixelFormat(static_cast<SDL_PixelFormat>(static_cast<int>(v.as<double>())));
            } catch(...) {}
        });

        // Additional config setters
        objHandleType["setWindowWidth"] = [](Core& /*core*/, float w) { Core::getInstance().setWindowWidth(w); return true; };
        coreTable.set_function("setWindowWidth", [](sol::this_state, sol::object /*self*/, sol::object v){
            try {
                if (v.is<float>()) Core::getInstance().setWindowWidth(v.as<float>());
                else if (v.is<double>()) Core::getInstance().setWindowWidth(static_cast<float>(v.as<double>()));
                else if (v.is<int>()) Core::getInstance().setWindowWidth(static_cast<float>(v.as<int>()));
            } catch(...) {}
        });

        objHandleType["setWindowHeight"] = [](Core& /*core*/, float h) { Core::getInstance().setWindowHeight(h); return true; };
        coreTable.set_function("setWindowHeight", [](sol::this_state, sol::object /*self*/, sol::object v){
            try {
                if (v.is<float>()) Core::getInstance().setWindowHeight(v.as<float>());
                else if (v.is<double>()) Core::getInstance().setWindowHeight(static_cast<float>(v.as<double>()));
                else if (v.is<int>()) Core::getInstance().setWindowHeight(static_cast<float>(v.as<int>()));
            } catch(...) {}
        });

        objHandleType["setPreserveAspectRatio"] = [](Core& /*core*/, bool p) { Core::getInstance().setPreserveAspectRatio(p); return true; };
        coreTable.set_function("setPreserveAspectRatio", [](sol::this_state, sol::object /*self*/, sol::object v){
            try {
                if (v.is<bool>()) Core::getInstance().setPreserveAspectRatio(v.as<bool>());
                else if (v.is<int>()) Core::getInstance().setPreserveAspectRatio(v.as<int>() != 0);
            } catch(...) {}
        });

        objHandleType["setAllowTextureResize"] = [](Core& /*core*/, bool a) { Core::getInstance().setAllowTextureResize(a); return true; };
        coreTable.set_function("setAllowTextureResize", [](sol::this_state, sol::object /*self*/, sol::object v){
            try {
                if (v.is<bool>()) Core::getInstance().setAllowTextureResize(v.as<bool>());
                else if (v.is<int>()) Core::getInstance().setAllowTextureResize(v.as<int>() != 0);
            } catch(...) {}
        });

        objHandleType["setRendererLogicalPresentation"] = [](Core& /*core*/, int p) { Core::getInstance().setRendererLogicalPresentation(static_cast<SDL_RendererLogicalPresentation>(p)); return true; };
        coreTable.set_function("setRendererLogicalPresentation", [](sol::this_state, sol::object /*self*/, sol::object v){
            try {
                if (v.is<int>()) Core::getInstance().setRendererLogicalPresentation(static_cast<SDL_RendererLogicalPresentation>(v.as<int>()));
                else if (v.is<double>()) Core::getInstance().setRendererLogicalPresentation(static_cast<SDL_RendererLogicalPresentation>(static_cast<int>(v.as<double>())));
            } catch(...) {}
        });

	    // --- Window Title & Timing --- //
        SDOM::core_bind_return_string("getWindowTitle", getWindowTitle_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_string("setWindowTitle", setWindowTitle_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_float("getElapsedTime", getElapsedTime_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_float("getDeltaTime", getElapsedTime_lua, objHandleType, coreTable, lua); // alias of getElapsedTime()

	    // --- Event helpers (exposed to Lua) --- //
        SDOM::core_bind_noarg("pumpEventsOnce", pumpEventsOnce_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_object_arg("pushMouseEvent", pushMouseEvent_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_object_arg("pushKeyboardEvent", pushKeyboardEvent_lua, objHandleType, coreTable, lua);

	    // --- Orphan / Future Child Management --- //
        // Accept handle/name/table for destroyDisplayObject for consistency
        SDOM::core_bind_object_arg("destroyDisplayObject", destroyDisplayObject_any_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_object_arg("destroyAssetObject", destroyAssetObject_any_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_int("countOrphanedDisplayObjects", countOrphanedDisplayObjects_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_vector_do("getOrphanedDisplayObjects", getOrphanedDisplayObjects_lua, objHandleType, coreTable, lua);
        // SDOM::core_bind_noarg("destroyOrphanedDisplayObjects", destroyOrphanedDisplayObjects_lua, objHandleType, coreTable, lua);
        // SDOM::core_bind_noarg("destroyOrphanedObjects", destroyOrphanedDisplayObjects_lua, objHandleType, coreTable, lua); // alias of destroyOrphanedDisplayObjects()
        // SDOM::core_bind_noarg("destroyOrphans", destroyOrphanedDisplayObjects_lua, objHandleType, coreTable, lua); // alias of destroyOrphanedDisplayObjects()
        SDOM::core_bind_noarg("collectGarbage", collectGarbage_lua, objHandleType, coreTable, lua);

        // --- Factory Utilities --- //
        SDOM::core_bind_noarg("clearFactory", clearFactory_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_string_return_asset("findAssetByFilename", findAssetByFilename_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_object_return_asset("findSpriteSheetByParams", findSpriteSheetByParams_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_noarg("unloadAllAssetObjects", unloadAllAssetObjects_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_noarg("reloadAllAssetObjects", reloadAllAssetObjects_lua, objHandleType, coreTable, lua);

        // --- Utility Methods --- //
        SDOM::core_bind_return_vector_string("getDisplayObjectNames", getDisplayObjectNames_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_noarg("printObjectRegistry", printObjectRegistry_lua, objHandleType, coreTable, lua);

        // --- Version --- //
        SDOM::core_bind_return_string("getVersionString", getVersionString_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_string("getVersionFullString", getVersionFullString_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_int("getVersionMajor", getVersionMajor_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_int("getVersionMinor", getVersionMinor_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_int("getVersionPatch", getVersionPatch_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_string("getVersionCodename", getVersionCodename_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_string("getVersionBuild", getVersionBuild_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_string("getVersionBuildDate", getVersionBuildDate_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_string("getVersionCommit", getVersionCommit_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_string("getVersionBranch", getVersionBranch_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_string("getVersionCompiler", getVersionCompiler_lua, objHandleType, coreTable, lua);
        SDOM::core_bind_return_string("getVersionPlatform", getVersionPlatform_lua, objHandleType, coreTable, lua);

    } // End Core::_registerDisplayObject()


} // namespace SDOM
