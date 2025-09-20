// SDOM_Core.hpp

#pragma once

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDataObject.hpp>

namespace SDOM
{
    
    /**
     * @class Core
     * @brief The central singleton framework object for SDOM.
     *
     * Core manages the main SDL loop, error logging/trapping, the Factory, and the EventManager.
     * It orchestrates the lifecycle and event flow of the application, and inherits from IDataObject
     * to support JSON configuration and serialization.
     *
     * Core is marked as final and cannot be subclassed.
     */
    class Core final : public IDataObject
    {
        class Event;
        class EventType;
        class Stage;

        public:
            struct CoreConfig 
            {
                float windowWidth = 800.0f;
                float windowHeight = 600.0f;
                float pixelWidth = 2.0f;
                float pixelHeight = 2.0f;
                bool preserveAspectRatio = true;
                bool allowTextureResize = true;
                SDL_RendererLogicalPresentation rendererLogicalPresentation = SDL_LOGICAL_PRESENTATION_LETTERBOX;
                SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE;
                SDL_PixelFormat pixelFormat = SDL_PIXELFORMAT_RGBA8888;
                SDL_Color color; // background color
            };
        
            // Singleton accessor
            static Core& getInstance() { static Core instance; return instance; }

            // IDataObject overrides
            virtual bool onInit() override;
            virtual void onQuit() override;
            virtual bool onUnitTest() override;

            // Simulated Core only "overrides"
            void onRender();
            void onEvent(Event& event);
            void onUpdate(float fElapsedTime);
            void onWindowResize(int newWidth, int newHeight);

            /**
             * @brief Runs the main application loop.
             */
            void run();

            // Configuration & Initialization
            void configure(const CoreConfig& config);
            void configureFromJson(const std::string& json);
            void configureFromJsonFile(const std::string& filename);

            // Callback/Hook Registration
            void registerOnInit(std::function<bool()> fn) { fnOnInit = fn; }
            void registerOnQuit(std::function<void()> fn) { fnOnQuit = fn; }
            void registerOnUpdate(std::function<void(float)> fn) { fnOnUpdate = fn; }
            void registerOnEvent(std::function<void(const Event&)> fn) { fnOnEvent = fn; }
            void registerOnRender(std::function<void()> fn) { fnOnRender = fn; }
            void registerOnUnitTest(std::function<bool()> fn) { fnOnUnitTest = fn; }
            void registerOnWindowResize(std::function<void(int, int)> fn) { fnOnWindowResize = fn; }

            // // Stage Management
            // void registerStage(const std::string& name, std::shared_ptr<Stage> stage);
            // void setStage(const std::string& name);

            SDL_Window* getWindow() const { return window_; }
            SDL_Renderer* getRenderer() const { return renderer_; }
            SDL_Texture* getTexture() const { return texture_; }
            SDL_Color getColor() const { return config_.color; }

        private:
            Core();
            ~Core();
            Core(const Core&) = delete;
            Core& operator=(const Core&) = delete;
            Core(Core&&) = delete;
            Core& operator=(Core&&) = delete;

            SDL_Window* window_ = nullptr;
            SDL_Renderer* renderer_ = nullptr;
            SDL_Texture* texture_ = nullptr;
            SDL_Color color_ = { 0, 0, 0, 255 }; // Default BLACK background color

            bool bIsRunning_ = true;
            CoreConfig config_;
            // float orig_aspect_ratio_ = 0.0f;

            // Callback hooks
            std::function<bool()> fnOnInit = nullptr;
            std::function<void()> fnOnQuit = nullptr;
            std::function<void(float)> fnOnUpdate = nullptr;
            std::function<void(const Event&)> fnOnEvent = nullptr;
            std::function<void()> fnOnRender = nullptr;
            std::function<bool()> fnOnUnitTest = nullptr;
            std::function<void(int, int)> fnOnWindowResize = nullptr;

            // private helpers
            void startup_SDL();
            void shutdown_SDL();
    };

} // namespace SDOM