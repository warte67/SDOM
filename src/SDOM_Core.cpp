// SDOM_Core.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_SDL_Utils.hpp>

namespace SDOM
{

    Core::Core() : IDataObject()
    {
        // Constructor implementation
    }

    Core::~Core()
    {
        // Destructor implementation
    }

    void Core::configure(const CoreConfig& config)
    {
        config_ = config;   
    }

    void Core::configureFromJson(const std::string& jsonStr)
    {
        nlohmann::json j = nlohmann::json::parse(jsonStr);
        CoreConfig config;
        config.windowWidth = j["core"].value("windowWidth", 1280.0f);
        config.windowHeight = j["core"].value("windowHeight", 720.0f);
        config.pixelWidth  = j["core"].value("pixelWidth", 2.0f);
        config.pixelHeight = j["core"].value("pixelHeight", 2.0f);
        config.preserveAspectRatio = j["core"].value("preserveAspectRatio", true);
        config.allowTextureResize = j["core"].value("allowTextureResize", true);

        config.rendererLogicalPresentation = SDL_Utils::rendererLogicalPresentationFromString(
            j["core"].value("rendererLogicalPresentation", "SDL_LOGICAL_PRESENTATION_LETTERBOX"));
        config.windowFlags = SDL_Utils::windowFlagsFromString(
            j["core"].value("windowFlags", "SDL_WINDOW_RESIZABLE"));
        config.pixelFormat = SDL_Utils::pixelFormatFromString(
            j["core"].value("pixelFormat", "SDL_PIXELFORMAT_RGBA8888"));        
        if (j["core"].contains("color")) {
            config.color.r = j["core"]["color"].value("r", 0);
            config.color.g = j["core"]["color"].value("g", 0);
            config.color.b = j["core"]["color"].value("b", 0);
            config.color.a = j["core"]["color"].value("a", 255);
        } else {
            config.color.r = 0;
            config.color.g = 0;
            config.color.b = 0;
            config.color.a = 255;
        }
        configure(config);
    }

    void Core::configureFromJsonFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            // Handle error: file not found or cannot be opened
            ERROR("Could not open configuration file: " + filename);
            return;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        configureFromJson(buffer.str());
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
            window_ = SDL_CreateWindow("Title", config_.windowWidth, config_.windowHeight, config_.windowFlags);
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

            // For now, just drop out immediately because
            // we do not yet have an active SDL Window nor a renderer.
            // bIsRunning_ = false;
            
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
                    // if (eventManager_)
                    // {                    
                    //     // Dispatch the event only to the stage with the matching window ID
                    //     eventManager_->Queue_SDL_Event(*pair.second, event);
                    //     eventManager_->DispatchQueuedEvents(*pair.second);
                    //     // handle TAB keypress
                    //     if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_TAB) 
                    //     {
                    //         if (event.key.mod & SDL_KMOD_SHIFT) 
                    //         {
                    //             // Shift + Tab: Move focus to the previous object
                    //             handleTabKeyPressReverse(*pair.second);
                    //         } 
                    //         else 
                    //         {
                    //             // Tab: Move focus to the next object
                    //             handleTabKeyPress(*pair.second);
                    //         }
                    //     }
                    //     // handle ESC keypress
                    //     if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) 
                    //     {
                    //         keyboardFocusedObject_.reset(); // Clear keyboard focus
                    //     }
                    // }

                    // TODO:  add virtual methods to IDisplayObject::onSDL_Event() to specifically 
                    //      dispatch raw SDL_Events to?

                }
                
                // frame timing
                static Uint64 lastTime = SDL_GetPerformanceCounter();
                Uint64 currentTime = SDL_GetPerformanceCounter();
                float fElapsedTime = static_cast<float>(currentTime - lastTime) / SDL_GetPerformanceFrequency();

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

                // factory_->detachOrphans(); // Detach orphaned display objects
                // factory_->attachFutureChildren(); // Attach future children

            }  // END: while (SDL_PollEvent(&event)) 
        }
        catch (const SDOM::Exception& e) 
        {
            // Handle exceptions gracefully
            int terminalWidth = 0, terminalHeight = 0;
            CLR::get_terminal_size(terminalWidth, terminalHeight);

            std::string line1 = "Exception Caught: " + std::string(e.what());
            std::string line2 = "File: " + e.getFile();
            std::string line3 = "Line: " + std::to_string(e.getLine());

            size_t maxWidth = std::max(line1.size(), std::max(line2.size(), line3.size()));

            if ((size_t)terminalWidth > maxWidth) 
            { // Add padding for the border
                std::string border(maxWidth + 2, '-'); // ASCII horizontal line
                std::cout << CLR::BROWN << "+" << border << "+\n"
                        << CLR::BROWN << "| " << CLR::RED << "Exception Caught: " << CLR::WHITE << e.what() << std::string(maxWidth - line1.size(), ' ') << CLR::BROWN << " |\n"
                        << CLR::BROWN << "| " << CLR::RED << "File: " << CLR::YELLOW << e.getFile() << std::string(maxWidth - line2.size(), ' ') << CLR::BROWN << " |\n"
                        << CLR::BROWN << "| " << CLR::RED << "Line: " << CLR::YELLOW << std::to_string(e.getLine()) << std::string(maxWidth - line3.size(), ' ') << CLR::BROWN << " |\n"
                        << CLR::BROWN << "+" << border << "+" << CLR::RESET << std::endl;
            } 
            else 
            {
                std::cout << CLR::RED << "Exception caught: " 
                        << CLR::WHITE << e.what()
                        << CLR::RED << "\nFile: " 
                        << CLR::BROWN << e.getFile() 
                        << CLR::RED << "\nLine: " 
                        << CLR::BROWN << e.getLine() 
                        << CLR::RESET << std::endl;            
            }
            std::string errStr = "Exception Caught: " + std::string(e.what()) + "\nFile: " + e.getFile() + "\nLine: " + std::to_string(e.getLine());
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
        bool ret = true;
        // Initialize the Core
        // ...

        if (fnOnInit)
            ret &= fnOnInit();

        return ret;
    }

    void Core::onQuit()
    {
        if (fnOnQuit)
            fnOnQuit();
        shutdown_SDL();
    }

    void Core::onRender()
    {
        // Render the stage
        if (fnOnRender)
            fnOnRender();
        else
        {
            SDL_Renderer* renderer = getRenderer();
            if (renderer==nullptr)
                ERROR("Stage::onRender() Error: Renderer is null.");
            SDL_Color color = getColor();
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderClear(renderer);
        }
    }

    void Core::onEvent(Event& event)
    {
        if (fnOnEvent)
            fnOnEvent(event);
    }

    void Core::onUpdate(float fElapsedTime)
    {
        if (fnOnUpdate)
            fnOnUpdate(fElapsedTime);
    }

    bool Core::onUnitTest()
    {
        bool allTestsPassed = true;

        // Run Core-specific unit tests here
        // ...

        // Run registered unit test function if available
        if (fnOnUnitTest) {
            allTestsPassed &= fnOnUnitTest();
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



} // namespace SDOM