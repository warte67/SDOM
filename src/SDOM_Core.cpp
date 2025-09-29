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

namespace SDOM
{

    Core::Core() : IDataObject()
    {
        lua_.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string);
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
        config_ = config;  

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

        // Debug: List all display objects in the factory
        std::cout << "Factory display objects after configuration:\n";
        for (const auto& pair : factory_->displayObjects_) {
            std::cout << "Display Object name: " << pair.first
                    << ", RawType: " << typeid(*pair.second).name()
                    << ", Type: " << pair.second->getType() << std::endl;
        }
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

        sol::state lua;
        lua.open_libraries(sol::lib::base);

        // Load and execute the Lua file
        lua.script(buffer.str());

        sol::table configTable = lua["config"];
        if (!configTable.valid()) {
            ERROR("Lua config file did not produce a valid 'config' table.");
            return;
        }
        configureFromLua(configTable);
    }

    void Core::startup_SDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO)) 
        {
            ERROR("SDL_Init Error: " + std::string(SDL_GetError()));
        }

        // create the main window
        if (!window_)
        {
            window_ = SDL_CreateWindow(getWindowTitle().c_str(), config_.windowWidth, config_.windowHeight, config_.windowFlags);
            if (!window_) 
            {
                std::string errorMsg = "SDL_CreateWindow() Error: " + std::string(SDL_GetError());
                ERROR(errorMsg);
            }
        }
        SDL_ShowWindow(window_);     // not needed in SDL3, but included for clarity

        // create the renderer
        if (!renderer_)
        {
            renderer_ = SDL_CreateRenderer(window_, NULL);
            if (!renderer_) {
                std::string errorMsg = "SDL_CreateRenderer() Error: " + std::string(SDL_GetError());
                ERROR(errorMsg);
            }
        }

        // create the stage texture
        if (!texture_)
        {
            texture_ = SDL_CreateTexture(renderer_, 
                config_.pixelFormat, 
                SDL_TEXTUREACCESS_TARGET, 
                config_.windowWidth / config_.pixelWidth, 
                config_.windowHeight / config_.pixelHeight);
            if (!texture_) {
                std::string errorMsg = "SDL_CreateTexture() Error: " + std::string(SDL_GetError());
                ERROR(errorMsg);
            }
        }
        SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);   
        int tWidth = static_cast<int>(config_.windowWidth / config_.pixelWidth);
        int tHeight = static_cast<int>(config_.windowHeight / config_.pixelHeight);
        SDL_SetRenderLogicalPresentation(renderer_, tWidth, tHeight, config_.rendererLogicalPresentation);
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

    void Core::run()
    {
        // std::cout << "Core::run()" << std::endl;

        try
        {
            // Ensure SDL is initialized
            startup_SDL();

            // register and initialize the factory object (after creating SDL resources)
            onInit();   // for now just call onInit(). Later Factory will call onInit() 
                        // for each object as it creates them.

            Stage* rootStage = dynamic_cast<Stage*>(rootNode_.get());
            if (!rootStage)
            {
                ERROR("Core::run() Error: Root node is null or not a valid Stage.");
                return;
            }
            else
            {
                std::cout << "Core::run() Root node is a valid Stage: " << rootStage->getName() << std::endl;
                rootStage->printTree();
            }

            // Now run user tests after initialization
            bool testsPassed = onUnitTest();
            if (!testsPassed) 
            {
                ERROR("Unit tests failed. Aborting run.");
            }

            SDL_Event event;
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

                    // POSSIBLE TODO:  add virtual methods to IDisplayObject::onSDL_Event() to specifically 
                    //      dispatch raw SDL_Events to?

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

        // Call the users registered init function if available
        if (fnOnInit)
            ret &= fnOnInit();

        return ret;
    }


    void Core::onQuit()
    {
        // Lambda for recursive quit handling using std::function
        std::function<void(IDisplayObject&)> handleQuit;
        handleQuit = [&handleQuit](IDisplayObject& node) {
            node.onQuit();
            for (const auto& child : node.getChildren()) {
                auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
                if (childObj) {
                    handleQuit(*childObj);
                }
            }
        };

        // Call the users registered quit function if available
        if (fnOnQuit)
            fnOnQuit();

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
        // Lambda for recursive render handling using std::function
        std::function<void(IDisplayObject&)> handleRender;
        handleRender = [&handleRender](IDisplayObject& node) {
            node.onRender();
            for (const auto& child : node.getChildren()) {
                auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
                if (childObj) {
                    handleRender(*childObj);
                }
            }
        };

        SDL_Renderer* renderer = getRenderer();
        if (!renderer)
            ERROR("Core::onRender() Error: Renderer is null.");

        // Set render target to window (nullptr means default window target)
        SDL_SetRenderTarget(renderer, nullptr);

        // Clear the entire window to the border color
        SDL_Color color = getColor();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);

        // Point the render target back to the main texture
        if (texture_)
            SDL_SetRenderTarget(renderer, texture_); // set the render target to the proper background texture

        // Call recursive render on the root node (if it exists)
        IDisplayObject* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get());
        if (rootObj)
        {
            setIsTraversing(true);
            handleRender(*rootObj);
            setIsTraversing(false);
        }
    }

    void Core::onEvent(Event& event)
    {
        // Lambda for recursive event handling using std::function
        std::function<void(IDisplayObject&)> handleEvent;
        handleEvent = [&handleEvent, event](IDisplayObject& node) {
            node.onEvent(event);
            for (const auto& child : node.getChildren()) {
                auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
                if (childObj) {
                    handleEvent(*childObj);
                }
            }
        };

        if (fnOnEvent)
            fnOnEvent(event);
        // Call recursive event on the root node (if it exists)
        IDisplayObject* rootObj = dynamic_cast<IDisplayObject*>(rootNode_.get());
        if (rootObj)
        {
            setIsTraversing(true);
            handleEvent(*rootObj);
            setIsTraversing(false);
        }
    }

    void Core::onUpdate(float fElapsedTime)
    {
        // Lambda for recursive update handling using std::function
        std::function<void(IDisplayObject&)> handleUpdate;
        handleUpdate = [&handleUpdate, fElapsedTime](IDisplayObject& node) {
            node.onUpdate(fElapsedTime);
            for (const auto& child : node.getChildren()) {
                auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
                if (childObj) {
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
        // Determine which resources need to be recreated
        bool recreateWindow = false;
        bool recreateRenderer = false;
        bool recreateTexture = false;
        // Window recreation conditions
        if (config_.windowWidth != prevConfig.windowWidth ||
            config_.windowHeight != prevConfig.windowHeight ||
            config_.windowFlags != prevConfig.windowFlags)
        {
            recreateWindow = true;
            recreateRenderer = true;
            recreateTexture = true;
        }
        // Renderer recreation conditions
        if (config_.rendererLogicalPresentation != prevConfig.rendererLogicalPresentation)
        {
            recreateRenderer = true;
            recreateTexture = true;
        }
        // Texture recreation conditions
        if (config_.pixelWidth != prevConfig.pixelWidth ||
            config_.pixelHeight != prevConfig.pixelHeight ||
            config_.pixelFormat != prevConfig.pixelFormat ||
            config_.preserveAspectRatio != prevConfig.preserveAspectRatio ||
            config_.allowTextureResize != prevConfig.allowTextureResize)
        {
            recreateTexture = true;
        }
        // Destroy and recreate resources as needed
        if (recreateTexture && texture_) {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }
        if (recreateRenderer && renderer_) {
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
        }
        if (recreateWindow && window_) {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }
        // Recreate resources in order
        if (recreateWindow) {
            window_ = SDL_CreateWindow(getWindowTitle().c_str(), config_.windowWidth, config_.windowHeight, config_.windowFlags);
            if (!window_) ERROR("SDL_CreateWindow() Error: " + std::string(SDL_GetError()));
        }
        if (recreateRenderer) {
            renderer_ = SDL_CreateRenderer(window_, NULL);
            if (!renderer_) ERROR("SDL_CreateRenderer() Error: " + std::string(SDL_GetError()));
        }
        if (recreateTexture) {
            // Set texture width and height based on pixel size; 
            //     ensure no divide-by-zero errors
            int tWidth  = (config_.pixelWidth  != 0.0f)
                ? static_cast<int>(config_.windowWidth  / config_.pixelWidth)
                : 1;
            int tHeight = (config_.pixelHeight != 0.0f)
                ? static_cast<int>(config_.windowHeight / config_.pixelHeight)
                : 1;

            texture_ = SDL_CreateTexture(
                renderer_,
                config_.pixelFormat,
                SDL_TEXTUREACCESS_TARGET,
                tWidth,
                tHeight
            );
            if (!texture_)
                ERROR("SDL_CreateTexture() Error: " +
                    std::string(SDL_GetError()));

            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);
            SDL_SetRenderLogicalPresentation(
                renderer_,
                tWidth,
                tHeight,
                config_.rendererLogicalPresentation
            );
        } // END if (recreateTexture)

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
    DomHandle Core::getDisplayHandle(const std::string& name) {
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
        std::string typeNameLocal = "Core";
        std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName << CLR::RESET << std::endl;
        // 1. Create and save usertype table (no constructor)
    sol::usertype<Core> objHandleType = lua.new_usertype<Core>(typeName, sol::no_constructor);
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
            factory_->registerLuaFunction(typeName, "getDisplayHandle",
                [](IDataObject& obj, sol::object args, sol::state_view lua) -> sol::object {
                    Core& core = static_cast<Core&>(obj);
                    if (args.is<std::string>()) return sol::make_object(lua, getDisplayHandle_lua(core, args.as<std::string>()));
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
            factory_->registerLuaCommand(typeName, "clearFactory",
                [](IDataObject& obj, sol::object /*args*/, sol::state_view /*lua*/) {
                    Core& core = static_cast<Core&>(obj);
                    clearFactory_lua(core);
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
        // Note: some Lua callsites use the colon syntax (Core:method()). To make
        // those robust, also expose a global `Core` table that forwards to the
        // singleton. This avoids userdata/pointer mismatch issues.
        lua[typeName] = this; // keep a userdata binding

        // Expose EventType constants to Lua as a table (so Lua code can use EventType.MouseClick)
        try {
            sol::usertype<EventType> et_ut = lua.new_usertype<EventType>("EventType", sol::no_constructor);
            sol::table etbl = lua.create_table();
            etbl["MouseClick"] = sol::make_object(lua, std::ref(EventType::MouseClick));
            etbl["MouseButtonDown"] = sol::make_object(lua, std::ref(EventType::MouseButtonDown));
            etbl["MouseButtonUp"] = sol::make_object(lua, std::ref(EventType::MouseButtonUp));
            etbl["MouseMove"] = sol::make_object(lua, std::ref(EventType::MouseMove));
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
        coreTable.set_function("getDisplayHandle", [](sol::this_state ts, sol::object /*self*/, const std::string& name) {
            sol::state_view sv = ts;
            DomHandle h = Core::getInstance().getDisplayHandle(name);
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
                DomHandle h = Core::getInstance().getDisplayHandle(typeName);
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
        coreTable.set_function("getCommandNamesForType", [](sol::this_state ts, sol::object /*self*/, const std::string& typeName) {
            sol::state_view sv = ts;
            std::string query = typeName;
            if (Core::getInstance().hasDisplayObject(typeName)) {
                DomHandle h = Core::getInstance().getDisplayHandle(typeName);
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
                DomHandle h = Core::getInstance().getDisplayHandle(typeName);
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
        lua["Core"] = coreTable;
    }


} // namespace SDOM



