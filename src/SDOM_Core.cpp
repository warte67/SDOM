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

namespace SDOM
{

    Core::Core() : IDataObject()
    {
        lua_.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string);
        factory_ = new Factory();
        eventManager_ = new EventManager();

        // _registerLua_Usertype(getLua());
        // _registerLua_Commands(getLua());

        // DomHandle().registerLua_All(getLua());
        // ResHandle().registerLua_All(getLua());
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
        
        // --- Lua UserType Registration --- //
        Core& core = Core::getInstance();
        core._registerLua_Usertype(core.getLua());
        core._registerLua_Commands(core.getLua());

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
            // int terminalWidth = 0, terminalHeight = 0;
            // CLR::get_terminal_size(terminalWidth, terminalHeight);
            // std::cout << std::endl; // Add a blank line before the error box
            // std::string line1 = "Exception Caught: " + std::string(e.what());
            // std::string line2 = "File: " + e.getFile();
            // std::string line3 = "Line: " + std::to_string(e.getLine());
            // size_t maxWidth = std::max(line1.size(), std::max(line2.size(), line3.size()));
            // if ((size_t)terminalWidth > maxWidth) 
            // { // Add padding for the border
            //     std::string border(maxWidth + 2, '-'); // ASCII horizontal line
            //     std::cout << CLR::BROWN << "+" << border << "+\n"
            //             << CLR::BROWN << "| " << CLR::RED << "Exception Caught: " << CLR::WHITE << e.what() << std::string(maxWidth - line1.size(), ' ') << CLR::BROWN << " |\n"
            //             << CLR::BROWN << "| " << CLR::RED << "File: " << CLR::YELLOW << e.getFile() << std::string(maxWidth - line2.size(), ' ') << CLR::BROWN << " |\n"
            //             << CLR::BROWN << "| " << CLR::RED << "Line: " << CLR::YELLOW << std::to_string(e.getLine()) << std::string(maxWidth - line3.size(), ' ') << CLR::BROWN << " |\n"
            //             << CLR::BROWN << "+" << border << "+" << CLR::RESET << std::endl;
            // } 
            // else 
            // {
            //     std::cout << CLR::RED << "Exception caught: " 
            //             << CLR::WHITE << e.what()
            //             << CLR::RED << "\nFile: " 
            //             << CLR::BROWN << e.getFile() 
            //             << CLR::RED << "\nLine: " 
            //             << CLR::BROWN << e.getLine() 
            //             << CLR::RESET << std::endl;            
            // }
            // std::string errStr = "Exception Caught: " + std::string(e.what()) + "\nFile: " + e.getFile() + "\nLine: " + std::to_string(e.getLine());

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

    bool Core::onInit()
    {
        // Note: We do not need to run onInit() for each object here because
        // the Factory creates each object and calls onInit() as it does so.

        // Factory onInit() is called from Core::configure(const CoreConfig& config)

            // sequenceDiagram
            //     participant Main
            //     participant Core
            //     participant Factory
                
            //     Main->>Core: getCore() // Core singleton created
            //     Core->>Factory: Factory constructed (no registration yet)
            //     Core->>Core: Core::configure(config)
            //     Core->>Factory: factory_->onInit()
            //     Factory->>Factory: registerDomType("Stage", ...)
            //     Factory->>Factory: Register other built-in types
            //     Core->>Core: configureFromLua(luaTable)
            //     Core->>Factory: factory_->create(type, obj) (recursive resource creation)
            //     Core->>Core: setRootNode("mainStage")
            //     Core->>Core: run()
            //     Core->>Core: startup_SDL()
            //     Core->>Core: onInit()
            //     Core->>Factory: (optional) factory_->onInit() (if not already called)
            //     Core->>Core: Event loop, rendering, updates, etc.
            //     Core->>Core: onQuit()
            //     Core->>Factory: Factory cleanup        
        
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
    void Core::printFactoryRegistry() const {
        getFactory().printRegistry();
    }

    void Core::initFactoryFromLua(const sol::table& lua) {
        getFactory().initFromLua(lua);
    }
    void Core::processFactoryResource(const sol::table& resource) {
        getFactory().processResource(resource);
    }

    // --- Lua UserType Registration --- //
    void Core::_registerLua_Usertype(sol::state_view lua)
    {
        SUPER::_registerLua_Usertype(lua);

        lua.new_usertype<Core>("Core",
            // --- Main Loop & Event Dispatch ---
            "quit", &Core::quit,
            "shutdown", &Core::shutdown,

            // --- Stage/Root Node Management ---
            "setRootNode", sol::resolve<void(const std::string&)>(&Core::setRootNode),
            "setRootNodeByHandle", sol::resolve<void(const DomHandle&)>(&Core::setRootNode),
            "setStage", &Core::setStage,
            "getRootNode", &Core::getRootNode,
            "getStageHandle", &Core::getStageHandle,

            // --- SDL Resource Accessors ---
            "getColor", &Core::getColor,
            "setColor", static_cast<void(Core::*)(const SDL_Color&)>(&Core::setColor),

            // --- Configuration Accessors ---
            "getPreserveAspectRatio", &Core::getPreserveAspectRatio,
            "getAllowTextureResize", &Core::getAllowTextureResize,
            "setPreserveAspectRatio", static_cast<void(Core::*)(bool)>(&Core::setPreserveAspectRatio),
            "setAllowTextureResize", static_cast<void(Core::*)(bool)>(&Core::setAllowTextureResize),

            // --- Focus & Hover Management ---
            "handleTabKeyPress", &Core::handleTabKeyPress,
            "handleTabKeyPressReverse", &Core::handleTabKeyPressReverse,
            "setKeyboardFocusedObject", &Core::setKeyboardFocusedObject,
            "getKeyboardFocusedObject", &Core::getKeyboardFocusedObject,
            "setMouseHoveredObject", &Core::setMouseHoveredObject,
            "getMouseHoveredObject", &Core::getMouseHoveredObject,

            // --- Window Title & Timing ---
            "getWindowTitle", &Core::getWindowTitle,
            "setWindowTitle", &Core::setWindowTitle,
            "getElapsedTime", &Core::getElapsedTime,

            // --- Factory Wrappers ---
            "createDisplayObject", sol::resolve<DomHandle(const std::string&, const sol::table&)>(&Core::createDisplayObject),
            "getDisplayHandle", &Core::getDisplayHandle,
            "getFactoryStageHandle", &Core::getFactoryStageHandle,
            "destroyDisplayObject", &Core::destroyDisplayObject,

            // --- Orphan Management ---
            "countOrphanedDisplayObjects", &Core::countOrphanedDisplayObjects,
            "getOrphanedDisplayObjects", &Core::getOrphanedDisplayObjects,
            "destroyOrphanedDisplayObjects", &Core::destroyOrphanedDisplayObjects,
            "detachOrphans", &Core::detachOrphans,

            // --- Future Child Management ---
            "attachFutureChildren", &Core::attachFutureChildren,
            "addToOrphanList", &Core::addToOrphanList,
            "addToFutureChildrenList", &Core::addToFutureChildrenList,

            // --- Utility Methods ---
            "listDisplayObjectNames", &Core::listDisplayObjectNames,
            "clearFactory", &Core::clearFactory,
            "printFactoryRegistry", &Core::printFactoryRegistry,

            // --- Lua Integration ---
            "initFactoryFromLua", &Core::initFactoryFromLua,
            "processFactoryResource", &Core::processFactoryResource
        );

        lua["Core"] = this;
    }


    // --- Lua Command Registration --- //
    void Core::_registerLua_Commands(sol::state_view lua)
    { 
        SUPER::_registerLua_Commands(lua);

        // --- Main Loop & Event Dispatch ---
        registerCommand("quit", [this](IDataObject&, sol::object, sol::state_view) { quit(); });
        registerCommand("shutdown", [this](IDataObject&, sol::object, sol::state_view) { shutdown(); });

        // --- Stage/Root Node Management ---
        registerCommand("setRootNode", [this](IDataObject&, sol::object val, sol::state_view) {
            if (val.is<std::string>()) setRootNode(val.as<std::string>());
            // Optionally: handle DomHandle if needed
        });
        registerCommand("setStage", [this](IDataObject&, sol::object val, sol::state_view) {
            if (val.is<std::string>()) setStage(val.as<std::string>());
        });
        registerCommand("getRootNode", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getRootNode());
        });
        registerCommand("getStageHandle", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getStageHandle());
        });

        // --- SDL Resource Accessors ---
        registerCommand("getColor", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getColor());
        });
        registerCommand("setColor", [this](IDataObject&, sol::object val, sol::state_view) {
            if (val.is<SDL_Color>()) setColor(val.as<SDL_Color>());
            // Optionally: handle table conversion to SDL_Color
        });

        // --- Configuration Accessors ---
        registerCommand("getPreserveAspectRatio", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getPreserveAspectRatio());
        });
        registerCommand("getAllowTextureResize", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getAllowTextureResize());
        });
        registerCommand("setPreserveAspectRatio", [this](IDataObject&, sol::object val, sol::state_view) {
            setPreserveAspectRatio(val.as<bool>());
        });
        registerCommand("setAllowTextureResize", [this](IDataObject&, sol::object val, sol::state_view) {
            setAllowTextureResize(val.as<bool>());
        });

        // --- Focus & Hover Management ---
        registerCommand("handleTabKeyPress", [this](IDataObject&, sol::object, sol::state_view) { handleTabKeyPress(); });
        registerCommand("handleTabKeyPressReverse", [this](IDataObject&, sol::object, sol::state_view) { handleTabKeyPressReverse(); });
        registerCommand("setKeyboardFocusedObject", [this](IDataObject&, sol::object val, sol::state_view) {
            setKeyboardFocusedObject(val.as<DomHandle>());
        });
        registerCommand("getKeyboardFocusedObject", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getKeyboardFocusedObject());
        });
        registerCommand("setMouseHoveredObject", [this](IDataObject&, sol::object val, sol::state_view) {
            setMouseHoveredObject(val.as<DomHandle>());
        });
        registerCommand("getMouseHoveredObject", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getMouseHoveredObject());
        });

        // --- Window Title & Timing ---
        registerCommand("getWindowTitle", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getWindowTitle());
        });
        registerCommand("setWindowTitle", [this](IDataObject&, sol::object val, sol::state_view) {
            setWindowTitle(val.as<std::string>());
        });
        registerCommand("getElapsedTime", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getElapsedTime());
        });

        // --- Factory Wrappers ---
        registerCommand("createDisplayObject", [this](IDataObject&, sol::object val, sol::state_view lua) {
            // Expecting a table with typeName and config
            if (val.is<sol::table>()) {
                sol::table tbl = val.as<sol::table>();
                std::string typeName = tbl["typeName"];
                sol::table config = tbl["config"];
                return sol::make_object(lua, createDisplayObject(typeName, config));
            }
            return sol::make_object(lua, DomHandle());
        });
        registerCommand("createDisplayObjectFromScript", [this](IDataObject&, sol::object val, sol::state_view lua) {
            if (val.is<sol::table>()) {
                sol::table tbl = val.as<sol::table>();
                std::string typeName = tbl["typeName"];
                std::string script = tbl["script"];
                return sol::make_object(lua, createDisplayObjectFromScript(typeName, script));
            }
            return sol::make_object(lua, DomHandle());
        });
        registerCommand("getDisplayHandle", [this](IDataObject&, sol::object val, sol::state_view lua) {
            return sol::make_object(lua, getDisplayHandle(val.as<std::string>()));
        });
        registerCommand("getFactoryStageHandle", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getFactoryStageHandle());
        });
        registerCommand("destroyDisplayObject", [this](IDataObject&, sol::object val, sol::state_view) {
            destroyDisplayObject(val.as<std::string>());
        });

        // --- Orphan Management ---
        registerCommand("getOrphanedDisplayObjects", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, getOrphanedDisplayObjects());
        });
        registerCommand("destroyOrphanedDisplayObjects", [this](IDataObject&, sol::object, sol::state_view) {
            destroyOrphanedDisplayObjects();
        });

        // --- Utility Methods ---
        registerCommand("listDisplayObjectNames", [this](IDataObject&, sol::object, sol::state_view lua) {
            return sol::make_object(lua, listDisplayObjectNames());
        });
        registerCommand("printFactoryRegistry", [this](IDataObject&, sol::object, sol::state_view) {
            printFactoryRegistry();
        });
    } // END: void Core::_registerLua_Commands(sol::state_view lua)
} // namespace SDOM