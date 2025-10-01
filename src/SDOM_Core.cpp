// SDOM_Core.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_SDL_Utils.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>

// #include <SDOM/SDOM_Handle.hpp>
#include <SDOM/SDOM_DomHandle.hpp>
#include <SDOM/SDOM_ResHandle.hpp>

#include <SDOM/SDOM_Utils.hpp> // for parseColor
#include <SDOM/lua_Core.hpp>
#include <algorithm>

namespace SDOM
{

    Core::Core() : IDataObject()
    {
        // Open base libraries plus package and io so embedded scripts can use
        // `require`/`package` and basic I/O if desired by examples.
        lua_.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::package, sol::lib::io);
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
        std::function<void(sol::table, DomHandle)> createResourceRecursive;
        createResourceRecursive = [&](sol::table obj, DomHandle parent) {
            std::string type = obj["type"].get_or(std::string(""));
            DomHandle handle;
            if (!type.empty()) {
                handle = factory_->create(type, obj);
                if (parent && handle) {
                    parent->addChild(handle);
                }
            }
            if (obj["children"].valid() && obj["children"].is<sol::table>()) {
                sol::table children = obj["children"];
                for (auto& kv : children) {
                    sol::table child = kv.second.as<sol::table>();
                    createResourceRecursive(child, handle);
                }
            }
        };

        // Parse children and create resources
        if (coreObj["children"].valid() && coreObj["children"].is<sol::table>()) {
            sol::table children = coreObj["children"];
            for (auto& kv : children) {
                sol::table child = kv.second.as<sol::table>();
                createResourceRecursive(child, nullptr);
            }
        }

        // Set the "mainStage" as the root node
        std::string rootStageName = "mainStage";
        if (coreObj["rootStage"].valid())
            rootStageName = coreObj["rootStage"].get<std::string>();
        rootNode_ = factory_->getDomHandle(rootStageName);
        setWindowTitle("Stage: " + rootStageName);
    }

    void Core::configureFromLuaFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
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
            this->_registerLua("Core", lua_);

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

    // --- Core forwarding helpers to Factory ---
    std::vector<std::string> Core::getPropertyNamesForType(const std::string& typeName) const
    {
        if (factory_) return factory_->getPropertyNamesForType(typeName);
        return {};
    }

    std::vector<std::string> Core::getCommandNamesForType(const std::string& typeName) const
    {
        if (factory_) return factory_->getCommandNamesForType(typeName);
        return {};
    }

    std::vector<std::string> Core::getFunctionNamesForType(const std::string& typeName) const
    {
        if (factory_) return factory_->getFunctionNamesForType(typeName);
        return {};
    }

    void Core::run(const sol::table& config)
    {
        configureFromLua(config);
        run();
    }

    void Core::run(const std::string& configFile)
    {
        // If configFile refers to an actual file on disk, load it.
        // Otherwise treat the string as raw Lua script and execute it
        // in this Core's Lua state, then pull the `config` table.
        if (!configFile.empty()) {
            // Portable file-exists check using ifstream to avoid std::filesystem
            std::ifstream infile(configFile);
            if (infile.good()) {
                configureFromLuaFile(configFile);
            } else {
                // Ensure Core usertype is registered in this lua state so the
                // script can reference `Core` (register callbacks, call Core:run, etc.)
                this->_registerLua("Core", lua_);
                // Treat the string as a raw Lua script
                try {
                    lua_.script(configFile);
                    sol::table configTable = lua_["config"];
                    if (configTable.valid()) {
                        configureFromLua(configTable);
                    } else {
                        ERROR("Lua script did not produce a valid 'config' table.");
                    }
                } catch (const sol::error& e) {
                    ERROR(std::string("Error executing Lua script for configuration: ") + e.what());
                }
            }
        }
        // Now run normally
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

        if (recreate_window || recreate_renderer || recreate_texture) {
            DEBUG_LOG("Core::reconfigure(): significant SDL resource reinitialization requested");
        }        

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
                        DomHandle rootHandle = getStageHandle();
                        auto ev = std::make_unique<Event>(EventType::OnEvent, rootHandle);
                        // std::string evtString = SDL_Utils::eventTypeToString(static_cast<SDL_EventType>(event.type));
                        // DEBUG_LOG("Core::run() Dispatching OnEvent to rootNode_ for SDL_Event type " + evtString);
                        // relatedTarget can be the stage handle to give context to listeners
                        ev->setRelatedTarget(rootHandle);
                        // Dispatch to event listeners only to avoid duplicating node->onEvent() calls
                        eventManager_->dispatchEvent(std::move(ev), rootHandle);
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

                // // MOVED to Core::onUpdate()
                // // Dispatch EventType::OnUpdate to rootNode_ so global listeners can perform actions before updating
                // // and have a chance to stopPropagation before onUpdate() is called.
                // if (eventManager_ && rootNode_)
                // {
                //     DomHandle rootHandle = this->getStageHandle();
                //     auto updateEv = std::make_unique<Event>(EventType::OnUpdate, rootHandle);
                //     updateEv->setElapsedTime(fElapsedTime);
                //     updateEv->setRelatedTarget(rootHandle);
                //     eventManager_->dispatchEvent(std::move(updateEv), rootHandle);
                // }

                // Send Updates
                onUpdate(fElapsedTime);

                // render the stage and its children
                onRender();

                // // MOVED To Core::onRender()
                // // Dispatch EventType::OnRender to rootNode_ so global listeners can perform actions after rendering
                // if (eventManager_ && rootNode_)
                // {
                //     DomHandle rootHandle = this->getStageHandle();
                //     auto renderEv = std::make_unique<Event>(EventType::OnRender, rootHandle);
                //     renderEv->setElapsedTime(fElapsedTime);
                //     renderEv->setRelatedTarget(rootHandle);
                //     eventManager_->dispatchEvent(std::move(renderEv), rootHandle);
                // }

                // present this stage
                if (renderer_ && texture_) 
                {
                    SDL_SetRenderTarget(renderer_, nullptr); // Reset to default target
                    SDL_RenderTexture(renderer_, texture_, NULL, NULL);
                    SDL_RenderPresent(renderer_);
                }

                // update timing
                lastTime = currentTime;

                factory_->detachOrphans(); // Detach orphaned display objects
                factory_->attachFutureChildren(); // Attach future children

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

            if (eventManager_)
            {
                eventManager_->Queue_SDL_Event(event);
                eventManager_->DispatchQueuedEvents();
            }
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
            DomHandle rootHandle;
            if (rootNode_)
            {
                // create a DomHandle referring to the root node
                rootHandle = DomHandle(rootNode_);
            }

            auto quitEvent = std::make_unique<Event>(EventType::OnQuit, rootHandle);
            // relatedTarget can be the root/stage handle too (useful to listeners)
            quitEvent->setRelatedTarget(rootHandle);

            // Dispatch via the central dispatch so global events reach both nodes and event-listeners
            eventManager_->dispatchEvent(std::move(quitEvent), rootHandle);
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

            DomHandle rootHandle = this->getStageHandle();
            // PreRender EventListeners
            if (node.hasEventListeners(EventType::OnPreRender, false))
            {
                auto preRenderEv = std::make_unique<Event>(EventType::OnPreRender, rootHandle);
                preRenderEv->setElapsedTime(this->getElapsedTime());
                preRenderEv->setRelatedTarget(rootHandle);
                eventManager_->dispatchEvent(std::move(preRenderEv), rootHandle);
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
                eventManager_->dispatchEvent(std::move(renderEv), rootHandle);        
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
                DomHandle rootNode = this->getStageHandle();
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
        testResult = UnitTests::run("Core", "SDL_WasInit(SDL_INIT_VIDEO)", []() { return (SDL_WasInit(SDL_INIT_VIDEO) != 0); });
        if (!testResult) { std::cout << CLR::indent() << "SDL was NOT initialized!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // SDL_Test: SDL Texture
        testResult = UnitTests::run("Core", "SDL Texture Validity", [this]() { return (texture_ != nullptr); });
        if (!testResult) { std::cout << CLR::indent() << "SDL Texture is null!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // SDL_Test: SDL Renderer
        testResult = UnitTests::run("Core", "SDL Renderer Validity", [this]() { return (renderer_ != nullptr); });
        if (!testResult) { std::cout << CLR::indent() << "SDL Renderer is null!" << CLR::RESET << std::endl; }
        allTestsPassed &= testResult;

        // SDL_Test: SDL Window
        testResult = UnitTests::run("Core", "SDL Window Validity", [this]() { return (window_ != nullptr); });
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
        auto populateTabList = [this](auto& populateTabListRef, DomHandle node) -> void 
        {
            if (!node.isValid()) return;

            // Resolve the DomHandle to an IDisplayObject*
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
        DomHandle node = getStageHandle();
        populateTabList(populateTabList, node);

        // Find the current object (keyboardFocusedObject_) with key focus within the tabList_
        DomHandle currentFocus = keyboardFocusedObject_;
        DomHandle nextFocus;

        bool foundCurrentFocus = false;
        std::vector<DomHandle> tempList;

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
        if (!nextFocus && !tempList.empty()) {
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
        auto populateTabList = [this](auto& populateTabListRef, DomHandle node) -> void 
        {
            if (!node.isValid()) return;

            // Resolve the DomHandle to an IDisplayObject*
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
        DomHandle node = getStageHandle();
        populateTabList(populateTabList, node);

        // Find the current object (keyboardFocusedObject_) with key focus within the tabList_
        DomHandle currentFocus = keyboardFocusedObject_;
        DomHandle previousFocus;

        bool foundCurrentFocus = false;
        std::vector<DomHandle> tempList;

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
        if (!foundCurrentFocus && !tempList.empty()) {
            previousFocus = tempList.back();
        }

        // Restore tabList_
        for (const auto& item : tempList) {
            tabList_.push(item);
        }

        // Update the current object (keyboardFocusedObject_) to the previous entry in the tabList_
        setKeyboardFocusedObject(previousFocus);    // keyboardFocusedObject_ = previousFocus;
    } // END:void Core::handleTabKeyPressReverse(Stage& stage)

    void Core::setKeyboardFocusedObject(DomHandle obj)
    { keyboardFocusedObject_ = obj; }
    DomHandle Core::getKeyboardFocusedObject() const
    { return keyboardFocusedObject_; }
    void Core::setMouseHoveredObject(DomHandle obj)
    { hoveredObject_ = obj; }
    DomHandle Core::getMouseHoveredObject() const
    { return hoveredObject_; }

    void Core::setRootNode(const std::string& name)
    {
        DomHandle stageHandle = factory_->getDomHandle(name);
        if (stageHandle && dynamic_cast<Stage*>(stageHandle.get()))
        {
            rootNode_ = stageHandle;
            setWindowTitle("Stage: " + rootNode_->getName());
        }
    }
    void Core::setRootNode(const DomHandle& handle) 
    { 
        rootNode_ = handle;     
        setWindowTitle("Stage: " + rootNode_->getName());
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
    DomHandle Core::getRootNode() const 
    {
        return rootNode_; 
    }



    // --- Factory Wrappers Implementation --- //

    DomHandle Core::createDisplayObject(const std::string& typeName, const sol::table& config) {
        return getFactory().create(typeName, config);
    }
    DomHandle Core::createDisplayObject(const std::string& typeName, const IDisplayObject::InitStruct& init) {
        return getFactory().create(typeName, init);
    }
    DomHandle Core::createDisplayObjectFromScript(const std::string& typeName, const std::string& luaScript) {
        return getFactory().create(typeName, luaScript);
    }

    IDisplayObject* Core::getDisplayObject(const std::string& name) {
        return getFactory().getDomObj(name);
    }
    IResourceObject* Core::getResourceObject(const std::string& name) {
        return getFactory().getResObj(name);
    }
    DomHandle Core::getDisplayObjectHandle(const std::string& name) {
        return getFactory().getDomHandle(name);
    }
    DomHandle Core::getFactoryStageHandle() {
        return getFactory().getStageHandle();
    }
    bool Core::hasDisplayObject(const std::string& name) const {
        DomHandle handle = factory_->getDomHandle(name);
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
    std::vector<DomHandle> Core::getOrphanedDisplayObjects() {
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
    void Core::addToOrphanList(const DomHandle orphan) {
        getFactory().addToOrphanList(orphan);
    }
    void Core::addToFutureChildrenList(const DomHandle child, const DomHandle parent,
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
        // Avoid duplicate registration: if the typeName is already present in
        // the Lua state, assume registration has already been performed.
        try {
            sol::object existing = lua[typeName];
            if (existing.valid() && !existing.is<sol::nil_t>()) {
                return;
            }
        } catch (...) {
            // ignore and continue with registration
        }
        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Core";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName << CLR::RESET << std::endl;
        }
        
        // 1. Create and save usertype table (no constructor)
        sol::usertype<Core> objHandleType = lua.new_usertype<Core>(typeName,
            sol::no_constructor, sol::base_classes, sol::bases<IDataObject>());
        // Bind direct methods on the Core usertype to ensure proper colon-call
        // signatures for Lua callers. These use the thin wrappers defined in
        // lua_Core.cpp which forward to the Core singleton or member methods.
        objHandleType["quit"] = [](Core& core) {
            quit_lua(core);
            return true;
        };
        objHandleType["shutdown"] = [](Core& core) {
            shutdown_lua(core);
            return true;
        };
        objHandleType["createDisplayObject"] = [](Core& core, const std::string& typeName, const sol::table& cfg) {
            return createDisplayObject_lua(typeName, cfg);
        };

        objHandleType["getDisplayObjectHandle"] = &getDisplayObjectHandle_lua;
        objHandleType["getStageHandle"] = &getStageHandle_lua;
        objHandleType["hasDisplayObject"] = &hasDisplayObject_lua;
        objHandleType["destroyDisplayObject"] = &destroyDisplayObject_lua;
        objHandleType["pumpEventsOnce"] = [](Core& core) { pumpEventsOnce_lua(core); };

        // Allow both method (colon) and function (dot) call styles in Lua:
        // - Core:run() => passes the Core userdata as first arg (calls run on that instance)
        // - Core.run() => no args; call the singleton's run() for convenience
        // Expose run to Lua with multiple signatures:
        // - Core:run()                (method, no args)
        // - Core:run(table)           (method, table config)
        // - Core:run(string)          (method, filename or script)
        // - Core.run()                (function-style, forwards to singleton)
        // - Core.run(table)           (function-style, singleton)
        // - Core.run(string)          (function-style, singleton)
        objHandleType["run"] = sol::overload(
            [](Core& core) { run_lua(core); },
            [](Core& core, const sol::table& cfg) { core.run(cfg); },
            [](Core& core, const std::string& s) { core.run(s); },
            []() { Core::getInstance().run(); },
            [](const sol::table& cfg) { Core::getInstance().run(cfg); },
            [](const std::string& s) { Core::getInstance().run(s); }
        );

        objHandleType["pushMouseEvent"] = [](Core& core, const sol::object& args) { pushMouseEvent_lua(core, args); };
        objHandleType["pushKeyboardEvent"] = [](Core& core, const sol::object& args) { pushKeyboardEvent_lua(core, args); };

        // Configuration helpers: support both method (Core:configure(tbl)) and
        // function-style calls (Core.configure(tbl)) by providing overloads.
        objHandleType["configure"] = sol::overload(
            [](Core& core, const sol::table& cfg) { core.configureFromLua(cfg); },
            [](const sol::table& cfg) { Core::getInstance().configureFromLua(cfg); }
        );
        objHandleType["configureFromFile"] = sol::overload(
            [](Core& core, const std::string& filename) { core.configureFromLuaFile(filename); },
            [](const std::string& filename) { Core::getInstance().configureFromLuaFile(filename); }
        );
        // Callback / Hook registration bindings
        objHandleType["registerOnInit"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) {
                registerOnInit_lua([f]() -> bool {
                    sol::protected_function pf = f;
                    sol::protected_function_result r = pf();
                    if (!r.valid()) {
                        sol::error err = r;
                        ERROR(std::string("Lua registerOnInit error: ") + err.what());
                        return false;
                    }
                    try { return r.get<bool>(); } catch (...) { return false; }
                });
            },
            [](const sol::function& f) {
                registerOnInit_lua([f]() -> bool {
                    sol::protected_function pf = f;
                    sol::protected_function_result r = pf();
                    if (!r.valid()) {
                        sol::error err = r;
                        ERROR(std::string("Lua registerOnInit error: ") + err.what());
                        return false;
                    }
                    try { return r.get<bool>(); } catch (...) { return false; }
                });
            }
        );
        objHandleType["registerOnQuit"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) {
                registerOnQuit_lua([f]() { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnQuit error: ") + err.what()); } });
            },
            [](const sol::function& f) {
                registerOnQuit_lua([f]() { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnQuit error: ") + err.what()); } });
            }
        );
        objHandleType["registerOnUpdate"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) {
                registerOnUpdate_lua([f](float dt) { sol::protected_function pf = f; sol::protected_function_result r = pf(dt); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUpdate error: ") + err.what()); } });
            },
            [](const sol::function& f) {
                registerOnUpdate_lua([f](float dt) { sol::protected_function pf = f; sol::protected_function_result r = pf(dt); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUpdate error: ") + err.what()); } });
            }
        );
        objHandleType["registerOnEvent"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) {
                registerOnEvent_lua([f](const Event& e) { sol::protected_function pf = f; sol::protected_function_result r = pf(e); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnEvent error: ") + err.what()); } });
            },
            [](const sol::function& f) {
                registerOnEvent_lua([f](const Event& e) { sol::protected_function pf = f; sol::protected_function_result r = pf(e); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnEvent error: ") + err.what()); } });
            }
        );
        objHandleType["registerOnRender"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) {
                registerOnRender_lua([f]() { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnRender error: ") + err.what()); } });
            },
            [](const sol::function& f) {
                registerOnRender_lua([f]() { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnRender error: ") + err.what()); } });
            }
        );
        objHandleType["registerOnUnitTest"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) {
                registerOnUnitTest_lua([f]() -> bool { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUnitTest error: ") + err.what()); return false; } try { return r.get<bool>(); } catch (...) { return false; } });
            },
            [](const sol::function& f) {
                registerOnUnitTest_lua([f]() -> bool { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUnitTest error: ") + err.what()); return false; } try { return r.get<bool>(); } catch (...) { return false; } });
            }
        );
        objHandleType["registerOnWindowResize"] = sol::overload(
            [](Core& /*core*/, const sol::function& f) {
                registerOnWindowResize_lua([f](int w, int h) { sol::protected_function pf = f; sol::protected_function_result r = pf(w, h); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnWindowResize error: ") + err.what()); } });
            },
            [](const sol::function& f) {
                registerOnWindowResize_lua([f](int w, int h) { sol::protected_function pf = f; sol::protected_function_result r = pf(w, h); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnWindowResize error: ") + err.what()); } });
            }
        );
        // Generic registration helper: Core:registerOn("Update", func) style
        objHandleType["registerOn"] = sol::overload(
            [](Core& /*core*/, const std::string& name, const sol::function& f) { registerOn_lua(name, f); },
            [](const std::string& name, const sol::function& f) { registerOn_lua(name, f); }
        );
        objHandleType["getPropertyNamesForType"] = [](Core& core, const std::string& t) { return core.getPropertyNamesForType(t); };
        objHandleType["getCommandNamesForType"] = [](Core& core, const std::string& t) { return core.getCommandNamesForType(t); };
        objHandleType["getFunctionNamesForType"] = [](Core& core, const std::string& t) { return core.getFunctionNamesForType(t); };

        // Bind commonly-used factory wrapper directly so Lua calls like
        // Core:createDisplayObject("Box", {...}) work as expected.
        objHandleType["createDisplayObject"] = sol::resolve<DomHandle(const std::string&, const sol::table&)>(&Core::createDisplayObject);
        this->objHandleType_ = objHandleType; // Save in IDataObject

        // 2. Do NOT call SUPER::_registerLua(typeName, lua);

        // 3. Register properties/commands specific to Core using the Factory registry
        if (factory_) {
            // Main Loop & Event Dispatch
            factory_->registerLuaCommand(typeName, "quit",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view /*lua*/) {
                    quit_lua(static_cast<Core&>(obj));
                });
            factory_->registerLuaCommand(typeName, "shutdown",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view /*lua*/) {
                    shutdown_lua(static_cast<Core&>(obj));
                });
            factory_->registerLuaCommand(typeName, "run",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view /*lua*/) {
                    run_lua(static_cast<Core&>(obj));
                });

            // Configuration from Lua table/file
            factory_->registerLuaCommand(typeName, "configure",
                [](IDataObject& obj, sol::object args, sol::state_view /*lua*/) {
                    if (args.is<sol::table>()) {
                        Core& core = static_cast<Core&>(obj);
                        core.configureFromLua(args.as<sol::table>());
                    }
                });
            factory_->registerLuaCommand(typeName, "configureFromFile",
                [](IDataObject& obj, sol::object args, sol::state_view /*lua*/) {
                    if (args.is<std::string>()) {
                        Core& core = static_cast<Core&>(obj);
                        core.configureFromLuaFile(args.as<std::string>());
                    }
                });

            // Stage / Root Node Management
            factory_->registerLuaCommand(typeName, "setRootNode",
                [](IDataObject& obj, sol::object args, sol::state_view /*lua*/) {
                    Core& core = static_cast<Core&>(obj);
                    if (args.is<std::string>()) {
                        setRootNodeByName_lua(core, args.as<std::string>());
                    } else if (args.is<DomHandle>()) {
                        setRootNodeByHandle_lua(core, args.as<DomHandle>());
                    }
                });
            factory_->registerLuaCommand(typeName, "setStage",
                [](IDataObject& obj, sol::object args, sol::state_view /*lua*/) {
                    Core& core = static_cast<Core&>(obj);
                    if (args.is<std::string>()) setStage_lua(core, args.as<std::string>());
                });

            // Traversal flag (get/set)
            factory_->registerLuaFunction(typeName, "getIsTraversing",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view lua) -> sol::object {
                    const Core& core = static_cast<const Core&>(obj);
                    return sol::make_object(lua, getIsTraversing_lua(core));
                });
            factory_->registerLuaCommand(typeName, "setIsTraversing",
                [](IDataObject& obj, sol::object args, sol::state_view /*lua*/) {
                    Core& core = static_cast<Core&>(obj);
                    if (args.is<bool>()) setIsTraversing_lua(core, args.as<bool>());
                });

            // Focus & Hover Management
            factory_->registerLuaCommand(typeName, "handleTabKeyPress",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view /*lua*/) {
                    handleTabKeyPress_lua(static_cast<Core&>(obj));
                });
            factory_->registerLuaCommand(typeName, "handleTabKeyPressReverse",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view /*lua*/) {
                    handleTabKeyPressReverse_lua(static_cast<Core&>(obj));
                });
            factory_->registerLuaFunction(typeName, "getKeyboardFocusedObject",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view lua) -> sol::object {
                    const Core& core = static_cast<const Core&>(obj);
                    return sol::make_object(lua, getKeyboardFocusedObject_lua(core));
                });
            factory_->registerLuaCommand(typeName, "setKeyboardFocusedObject",
                [](IDataObject& obj, sol::object args, sol::state_view /*lua*/) {
                    Core& core = static_cast<Core&>(obj);
                    if (args.is<DomHandle>()) setKeyboardFocusedObject_lua(core, args.as<DomHandle>());
                });
            factory_->registerLuaFunction(typeName, "getMouseHoveredObject",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view lua) -> sol::object {
                    const Core& core = static_cast<const Core&>(obj);
                    return sol::make_object(lua, getMouseHoveredObject_lua(core));
                });
            factory_->registerLuaCommand(typeName, "setMouseHoveredObject",
                [](IDataObject& obj, sol::object args, sol::state_view /*lua*/) {
                    Core& core = static_cast<Core&>(obj);
                    if (args.is<DomHandle>()) setMouseHoveredObject_lua(core, args.as<DomHandle>());
                });

            // Factory introspection helpers (expose Core wrappers)
            factory_->registerLuaFunction(typeName, "getPropertyNamesForType",
                [](IDataObject& obj, sol::object args, sol::state_view lua) -> sol::object {
                    Core& core = static_cast<Core&>(obj);
                    if (!args.is<std::string>()) return sol::make_object(lua, sol::nil);
                    auto names = core.getPropertyNamesForType(args.as<std::string>());
                    sol::table t = lua.create_table();
                    for (size_t i = 0; i < names.size(); ++i) t[i+1] = names[i];
                    return sol::make_object(lua, t);
                });

            factory_->registerLuaFunction(typeName, "getCommandNamesForType",
                [](IDataObject& obj, sol::object args, sol::state_view lua) -> sol::object {
                    Core& core = static_cast<Core&>(obj);
                    if (!args.is<std::string>()) return sol::make_object(lua, sol::nil);
                    auto names = core.getCommandNamesForType(args.as<std::string>());
                    sol::table t = lua.create_table();
                    for (size_t i = 0; i < names.size(); ++i) t[i+1] = names[i];
                    return sol::make_object(lua, t);
                });

            factory_->registerLuaFunction(typeName, "getFunctionNamesForType",
                [](IDataObject& obj, sol::object args, sol::state_view lua) -> sol::object {
                    Core& core = static_cast<Core&>(obj);
                    if (!args.is<std::string>()) return sol::make_object(lua, sol::nil);
                    auto names = core.getFunctionNamesForType(args.as<std::string>());
                    sol::table t = lua.create_table();
                    for (size_t i = 0; i < names.size(); ++i) t[i+1] = names[i];
                    return sol::make_object(lua, t);
                });

            // Window Title & Timing
            factory_->registerLuaFunction(typeName, "getWindowTitle",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view lua) -> sol::object {
                    const Core& core = static_cast<const Core&>(obj);
                    return sol::make_object(lua, getWindowTitle_lua(core));
                });
            factory_->registerLuaCommand(typeName, "setWindowTitle",
                [](IDataObject& obj, sol::object args, sol::state_view /*lua*/) {
                    Core& core = static_cast<Core&>(obj);
                    if (args.is<std::string>()) setWindowTitle_lua(core, args.as<std::string>());
                });
            factory_->registerLuaFunction(typeName, "getElapsedTime",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view lua) -> sol::object {
                    const Core& core = static_cast<const Core&>(obj);
                    return sol::make_object(lua, getElapsedTime_lua(core));
                });
            // Alias for scripts that expect a "delta_time" style name
            factory_->registerLuaFunction(typeName, "get_delta_time",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view lua) -> sol::object {
                    const Core& core = static_cast<const Core&>(obj);
                    return sol::make_object(lua, getElapsedTime_lua(core));
                });

            // Event helpers (pump/push) - expose to Lua
            factory_->registerLuaCommand(typeName, "pumpEventsOnce",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view /*lua*/) {
                    pumpEventsOnce_lua(static_cast<Core&>(obj));
                });
            factory_->registerLuaCommand(typeName, "pushMouseEvent",
                [](IDataObject& obj, sol::object args, sol::state_view /*lua*/) {
                    pushMouseEvent_lua(static_cast<Core&>(obj), args);
                });

            // Object Lookup / Factory Wrappers
            factory_->registerLuaFunction(typeName, "createDisplayObject",
                [](IDataObject& obj, sol::object args, sol::state_view lua) -> sol::object {
                    // Expect args to be either a table (with type/name) or a string+table pair.
                    // If args is a table, try to extract type and pass through. If args is a string,
                    // we cannot access the second table directly here, so fallback to calling the
                    // global Core wrapper which accepts (type, table).
                    if (args.is<sol::table>()) {
                        sol::table t = args.as<sol::table>();
                        if (t["type"].valid()) {
                            std::string typeName = t["type"].get<std::string>();
                            DomHandle h = createDisplayObject_lua(typeName, t);
                            return sol::make_object(lua, h);
                        }
                    }
                    // As a fallback, try to treat args as a string (type name) and call the global wrapper
                    if (args.is<std::string>()) {
                        // There's no direct way to get the second arg here; rely on the free-function wrapper
                        // that may be used via the global Core table instead.
                        std::string typeName = args.as<std::string>();
                        // Return invalid handle as we don't have the config table
                        return sol::make_object(lua, DomHandle());
                    }
                    return sol::make_object(lua, DomHandle());
                });

            // Also register the direct member overload for convenience (calls Core::createDisplayObject)
            factory_->registerLuaFunction(typeName, "getDisplayObjectHandle",
                [](IDataObject& obj, sol::object args, sol::state_view lua) -> sol::object {
                    Core& core = static_cast<Core&>(obj);
                    if (args.is<std::string>()) return sol::make_object(lua, getDisplayObjectHandle_lua(core, args.as<std::string>()));
                    return sol::make_object(lua, DomHandle());
                });
            factory_->registerLuaFunction(typeName, "getFactoryStageHandle",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view lua) -> sol::object {
                    Core& core = static_cast<Core&>(obj);
                    return sol::make_object(lua, getFactoryStageHandle_lua(core));
                });
            factory_->registerLuaFunction(typeName, "hasDisplayObject",
                [](IDataObject& obj, sol::object args, sol::state_view lua) -> sol::object {
                    const Core& core = static_cast<const Core&>(obj);
                    if (args.is<std::string>()) return sol::make_object(lua, hasDisplayObject_lua(core, args.as<std::string>()));
                    return sol::make_object(lua, false);
                });
            factory_->registerLuaCommand(typeName, "destroyDisplayObject",
                [](IDataObject& obj, sol::object args, sol::state_view /*lua*/) {
                    Core& core = static_cast<Core&>(obj);
                    if (args.is<std::string>()) destroyDisplayObject_lua(core, args.as<std::string>());
                });

            // Orphan / Future Child Management
            factory_->registerLuaFunction(typeName, "countOrphanedDisplayObjects",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view lua) -> sol::object {
                    const Core& core = static_cast<const Core&>(obj);
                    return sol::make_object(lua, countOrphanedDisplayObjects_lua(core));
                });
            factory_->registerLuaFunction(typeName, "getOrphanedDisplayObjects",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view lua) -> sol::object {
                    Core& core = static_cast<Core&>(obj);
                    return sol::make_object(lua, getOrphanedDisplayObjects_lua(core));
                });
            factory_->registerLuaCommand(typeName, "destroyOrphanedDisplayObjects",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view /*lua*/) {
                    Core& core = static_cast<Core&>(obj);
                    destroyOrphanedDisplayObjects_lua(core);
                });

            // Utility Methods
            factory_->registerLuaFunction(typeName, "listDisplayObjectNames",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view lua) -> sol::object {
                    const Core& core = static_cast<const Core&>(obj);
                    return sol::make_object(lua, listDisplayObjectNames_lua(core));
                });
            
            factory_->registerLuaCommand(typeName, "printObjectRegistry",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view /*lua*/) {
                    Core& core = static_cast<Core&>(obj);
                    printObjectRegistry_lua(core);
                });

            // (Introspection helpers registered above; avoid duplicate registrations)

        }

        // 4. Register properties/commands using registry (the Factory may have additional bindings)
        if (factory_)
            factory_->registerLuaPropertiesAndCommands(typeName, objHandleType_);

    // 5. Expose the singleton instance to Lua under the type name
    // Use sol::make_object to create a userdata that matches the previously
    // created usertype. Assigning the raw C++ pointer directly can result
    // in a mismatched value (sol::Core*) which breaks method dispatch.
    lua[typeName] = sol::make_object(lua, this);

        // Expose EventType constants to Lua as a table (so Lua code can use EventType.MouseClick)
        // Use centralized registration on Event to create the usertypes and
        // convenience properties/methods. This keeps event binding logic in one
        // place and avoids duplicating registration in Core.
        try {
            Event::registerLua(lua);

            // Populate EventType table from registry
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
        } catch (...) {
            // non-fatal if event type binding fails
        }

        sol::table coreTable = lua.create_table();
        // Ensure returned DomHandle values are boxed as proper Lua userdata by
        // using sol::this_state and sol::make_object so method calls (e.g. :get())
        // receive a userdata that matches the registered usertype.
        coreTable.set_function("createDisplayObject", [](sol::this_state ts, sol::object /*self*/, const std::string& typeName, const sol::table& cfg) {
            sol::state_view sv = ts;
            DomHandle h = Core::getInstance().createDisplayObject(typeName, cfg);
            return sol::make_object(sv, h);
        });
        coreTable.set_function("getDisplayObjectHandle", [](sol::this_state ts, sol::object /*self*/, const std::string& name) {
            sol::state_view sv = ts;
                DomHandle h = Core::getInstance().getDisplayObjectHandle(name);
            return sol::make_object(sv, h);
        });
        coreTable.set_function("getStageHandle", [](sol::this_state ts, sol::object /*self*/) {
            sol::state_view sv = ts;
            DomHandle h = Core::getInstance().getStageHandle();
            return sol::make_object(sv, h);
        });
        coreTable.set_function("hasDisplayObject", [](sol::this_state /*ts*/, sol::object /*self*/, const std::string& name) {
            return Core::getInstance().hasDisplayObject(name);
        });
        coreTable.set_function("destroyDisplayObject", [](sol::this_state /*ts*/, sol::object /*self*/, const std::string& name) {
            Core::getInstance().destroyDisplayObject(name);
        });
        // Expose Factory introspection helpers on the global Core table as convenience wrappers
        coreTable.set_function("getPropertyNamesForType", [](sol::this_state ts, sol::object /*self*/, const std::string& typeName) {
            sol::state_view sv = ts;
            std::string query = typeName;
            // If the caller passed an instance name, resolve it to the registered type
            if (Core::getInstance().hasDisplayObject(typeName)) {
                DomHandle h = Core::getInstance().getDisplayObjectHandle(typeName);
                if (h.isValid() && h.get()) {
                    IDisplayObject* obj = dynamic_cast<IDisplayObject*>(h.get());
                    if (obj) query = obj->getType();
                }
            }
            auto names = Core::getInstance().getPropertyNamesForType(query);
            sol::table t = sv.create_table();
            for (size_t i = 0; i < names.size(); ++i) t[i+1] = names[i];
            return sol::make_object(sv, t);
        });
        // Convenience alias used by some Lua callbacks: get_delta_time -> elapsed time per frame
        coreTable.set_function("get_delta_time", [](sol::this_state /*ts*/, sol::object /*self*/) {
            return Core::getInstance().getElapsedTime();
        });
        coreTable.set_function("getCommandNamesForType", [](sol::this_state ts, sol::object /*self*/, const std::string& typeName) {
            sol::state_view sv = ts;
            std::string query = typeName;
            if (Core::getInstance().hasDisplayObject(typeName)) {
                DomHandle h = Core::getInstance().getDisplayObjectHandle(typeName);
                if (h.isValid() && h.get()) {
                    IDisplayObject* obj = dynamic_cast<IDisplayObject*>(h.get());
                    if (obj) query = obj->getType();
                }
            }
            auto names = Core::getInstance().getCommandNamesForType(query);
            sol::table t = sv.create_table();
            for (size_t i = 0; i < names.size(); ++i) t[i+1] = names[i];
            return sol::make_object(sv, t);
        });
        coreTable.set_function("getFunctionNamesForType", [](sol::this_state ts, sol::object /*self*/, const std::string& typeName) {
            sol::state_view sv = ts;
            std::string query = typeName;
            if (Core::getInstance().hasDisplayObject(typeName)) {
                DomHandle h = Core::getInstance().getDisplayObjectHandle(typeName);
                if (h.isValid() && h.get()) {
                    IDisplayObject* obj = dynamic_cast<IDisplayObject*>(h.get());
                    if (obj) query = obj->getType();
                }
            }
            auto names = Core::getInstance().getFunctionNamesForType(query);
            sol::table t = sv.create_table();
            for (size_t i = 0; i < names.size(); ++i) t[i+1] = names[i];
            return sol::make_object(sv, t);
        });
        // Event helpers exposed on the global Core table
        coreTable.set_function("pumpEventsOnce", [](sol::this_state /*ts*/, sol::object /*self*/) {
            Core::getInstance().pumpEventsOnce();
        });
        coreTable.set_function("pushMouseEvent", [](sol::this_state ts, sol::object /*self*/, sol::object args) {
            sol::state_view sv = ts;
            // Forward to pushMouseEvent_lua using the singleton Core
            pushMouseEvent_lua(Core::getInstance(), args);
            return sol::make_object(sv, sol::nil);
        });
        coreTable.set_function("pushKeyboardEvent", [](sol::this_state ts, sol::object /*self*/, sol::object args) {
            sol::state_view sv = ts;
            pushKeyboardEvent_lua(Core::getInstance(), args);
            return sol::make_object(sv, sol::nil);
        });
        coreTable.set_function("run", [](sol::this_state ts, sol::object /*self*/, sol::optional<sol::object> maybeArg) {
            sol::state_view sv = ts;
            if (!maybeArg) {
                Core::getInstance().run();
                return sol::make_object(sv, sol::nil);
            }
            sol::object arg = *maybeArg;
            if (arg.is<sol::table>()) {
                Core::getInstance().run(arg.as<sol::table>());
                return sol::make_object(sv, sol::nil);
            }
            if (arg.is<std::string>()) {
                Core::getInstance().run(arg.as<std::string>());
                return sol::make_object(sv, sol::nil);
            }
            // Unsupported arg type
            ERROR("CoreForward.run: unsupported argument type; expected table or string");
            return sol::make_object(sv, sol::nil);
        });
        // Configuration helpers on convenience table
        coreTable.set_function("configure", [](sol::this_state ts, sol::object /*self*/, const sol::table& cfg) {
            sol::state_view sv = ts;
            Core::getInstance().configureFromLua(cfg);
            return sol::make_object(sv, sol::nil);
        });
        coreTable.set_function("configureFromFile", [](sol::this_state ts, sol::object /*self*/, const std::string& filename) {
            sol::state_view sv = ts;
            Core::getInstance().configureFromLuaFile(filename);
            return sol::make_object(sv, sol::nil);
        });
        // Callback / Hook registration on convenience table
        coreTable.set_function("registerOnInit", [](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
            sol::state_view sv = ts;
            registerOnInit_lua([f]() -> bool { sol::protected_function pf = f; sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua CoreForward.registerOnInit error: ") + err.what()); return false; } try { return r.get<bool>(); } catch (...) { return false; } });
            return sol::make_object(sv, sol::nil);
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
    // Do not overwrite the `Core` global (which should be the userdata
    // instance bound to the C++ singleton). Overwriting it with a plain
    // table hides usertype methods (e.g. Core:quit()). Expose the
    // convenience forwarder under a separate name instead.
    lua["CoreForward"] = coreTable;
    }


} // namespace SDOM



