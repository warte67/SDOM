// SDOM_Core.hpp

#pragma once

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_ResourceHandle.hpp>

namespace SDOM
{
    class Factory;
    class Event;
    class EventType;
    class EventManager;
    class Stage;
    class IDisplayObject;

    
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
        
            /**
             * @brief Returns the singleton instance of Core.
             * 
             * @details
             * Core is designed as a singleton to manage the main application loop,
             * resource factory, event dispatch, and configuration. This accessor`
             * ensures only one instance exists throughout the application's lifetime.
             * 
             * @return Reference to the single Core instance.
             */            
            static Core& getInstance() { static Core instance; return instance; }

            // ===== Virtual Overrides from IDataObject =====
            virtual bool onInit() override;
            virtual void onQuit() override;
            virtual bool onUnitTest() override;

            // ===== Core-Driven DOM Dispatch Methods =====
            // These methods are called by Core to drive the lifecycle and event flow of IDisplayObject trees.
            // They are not virtual because Core orchestrates the recursion and dispatch, rather than being a display object itself.
            void onRender();
            void onEvent(Event& event);
            void onUpdate(float fElapsedTime);
            void onWindowResize(int newWidth, int newHeight);

            /**
             * @brief Runs the main application loop.
             */
            void run();

            void quit() { bIsRunning_ = false; }
            void shutdown() { quit(); }

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

            // ===== Stage Management =====
            void setRootNode(const std::string& name);
            void setRootNode(const ResourceHandle& handle);
            void setStage(const std::string& name); // Alias for backward compatibility
            Stage* getStage() const; // Alias for backward compatibility
            IDisplayObject* getRootNodePtr() const;
            ResourceHandle getRootNode() const;

            // ===== SDL Resource Accessors =====
            SDL_Window* getWindow() const       { return window_; }
            SDL_Renderer* getRenderer() const   { return renderer_; }
            SDL_Texture* getTexture() const     { return texture_; }
            SDL_Color getColor() const          { return config_.color; }
            void setColor(const SDL_Color& color = { 0, 0, 0, 255 }) { config_.color = color; }

            // ==== Configuration Accessors =====
            CoreConfig& getConfig();
            float getWindowWidth() const;
            float getWindowHeight() const;
            float getPixelWidth() const;
            float getPixelHeight() const;
            bool getPreserveAspectRatio() const;
            bool getAllowTextureResize() const;
            SDL_RendererLogicalPresentation getRendererLogicalPresentation() const;
            SDL_WindowFlags getWindowFlags() const;
            SDL_PixelFormat getPixelFormat() const;

            void setConfig(CoreConfig& config);
            void setWindowWidth(float width = 800.0f);
            void setWindowHeight(float height = 600.0f);
            void setPixelWidth(float width = 2.0f);
            void setPixelHeight(float height = 2.0f);
            void setPreserveAspectRatio(bool preserve = true);
            void setAllowTextureResize(bool allow = true);
            void setRendererLogicalPresentation(SDL_RendererLogicalPresentation presentation = SDL_LOGICAL_PRESENTATION_LETTERBOX);
            void setWindowFlags(SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE);
            void setPixelFormat(SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA8888);

            Factory& getFactory() { return *factory_; }

            bool getIsTraversing() const { return isTraversing_; }
            void setIsTraversing(bool traversing) { isTraversing_ = traversing; }



            // void handleTabKeyPress(Stage& stage);
            // void handleTabKeyPressReverse(Stage& stage);
            void setKeyboardFocusedObject(ResourceHandle obj);
            ResourceHandle getKeyboardFocusedObject() const;
            void setMouseHoveredObject(ResourceHandle obj);
            ResourceHandle getMouseHoveredObject() const;


            std::string getWindowTitle() const { return windowTitle_; }
            void setWindowTitle(const std::string& title) 
            {
                windowTitle_ = title;
                if (window_)
                    SDL_SetWindowTitle(window_, windowTitle_.c_str());
            }  

            float getElapsedTime() const { return fElapsedTime_; }  

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
            std::string windowTitle_ = "SDOM Application";
            SDL_Color color_ = { 0, 0, 0, 255 }; // Default BLACK background color

            bool bIsRunning_ = true;
            bool isTraversing_ = false;
            float fElapsedTime_;             // Elapsed time since the last update


            ResourceHandle rootNode_; // The root of the resource tree

            ResourceHandle hoveredObject_;
            ResourceHandle keyboardFocusedObject_;
            // struct TabPriorityComparator {
            //     bool operator()(const ResourceHandle& a, const ResourceHandle& b) const {
            //         auto* aObj = dynamic_cast<IDisplayObject*>(a.get());
            //         auto* bObj = dynamic_cast<IDisplayObject*>(b.get());
            //         if (!aObj || !bObj) return false; // Handle invalid handles
            //         return aObj->getTabPriority() < bObj->getTabPriority(); // Higher priority first
            //     }
            // };
            // std::priority_queue<ResourceHandle, std::vector<ResourceHandle>, TabPriorityComparator> tabList_;


            CoreConfig config_;

            // Core Sub-Systems
            inline static Factory* factory_ = nullptr; 
            inline static EventManager* eventManager_ = nullptr;

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
            void refreshSDLResources();

            bool coreTests_();
    };

} // namespace SDOM