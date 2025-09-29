#pragma once

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_DomHandle.hpp>
#include <SDOM/SDOM_ResHandle.hpp>

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
     */
    class Core final : public IDataObject
    {
        using SUPER = IDataObject;

    public:
        // --- Configuration Struct --- //
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

        // --- Singleton Access --- //
        static Core& getInstance() { static Core instance; return instance; }

        // --- Lifecycle (IDataObject overrides) --- //
        virtual bool onInit() override;
        virtual void onQuit() override;
        virtual bool onUnitTest() override;

        // --- Main Loop & Event Dispatch --- //
        void run();

        // Poll and dispatch pending SDL events once (test helper)
        void pumpEventsOnce();
        // Signal the main loop to stop. Provide a clear, non-generic message
        // so test output indicates this was a deliberate shutdown requested
        // from scripting (e.g. Lua's Core:quit()). Use the INFO logging macro
        // for consistency with the project's logging helpers.
        void quit() { INFO("Core: shutdown requested. Performing graceful shutdown..."); bIsRunning_ = false; }
        void shutdown() { quit(); }

        void onRender();
        void onEvent(Event& event);
        void onUpdate(float fElapsedTime);
        void onWindowResize(int newWidth, int newHeight);

        // --- Configuration & Initialization --- //
        void configure(const CoreConfig& config);
        void configureFromLua(const sol::table& config);
        void configureFromLuaFile(const std::string& filename);

        // --- Callback/Hook Registration --- //
        void registerOnInit(std::function<bool()> fn) { fnOnInit = fn; }
        void registerOnQuit(std::function<void()> fn) { fnOnQuit = fn; }
        void registerOnUpdate(std::function<void(float)> fn) { fnOnUpdate = fn; }
        void registerOnEvent(std::function<void(const Event&)> fn) { fnOnEvent = fn; }
        void registerOnRender(std::function<void()> fn) { fnOnRender = fn; }
        void registerOnUnitTest(std::function<bool()> fn) { fnOnUnitTest = fn; }
        void registerOnWindowResize(std::function<void(int, int)> fn) { fnOnWindowResize = fn; }

        // --- Stage/Root Node Management --- //
        void setRootNode(const std::string& name);
        void setRootNode(const DomHandle& handle);
        void setStage(const std::string& name); // Alias for backward compatibility
        Stage* getStage() const; // Alias for backward compatibility
        IDisplayObject* getRootNodePtr() const;
        DomHandle getRootNode() const;
        DomHandle getStageHandle() const { return getRootNode(); }  // Alias for backward compatibility

        // --- SDL Resource Accessors --- //
        SDL_Window* getWindow() const       { return window_; }
        SDL_Renderer* getRenderer() const   { return renderer_; }
        SDL_Texture* getTexture() const     { return texture_; }
        SDL_Color getColor() const          { return config_.color; }
        void setColor(const SDL_Color& color) { config_.color = color; }

        // --- Configuration Accessors --- //
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
        void setWindowWidth(float width);// = 800.0f);
        void setWindowHeight(float height);// = 600.0f);
        void setPixelWidth(float width);// = 2.0f);
        void setPixelHeight(float height);// = 2.0f);
        void setPreserveAspectRatio(bool preserve);// = true);
        void setAllowTextureResize(bool allow);// = true);
        void setRendererLogicalPresentation(SDL_RendererLogicalPresentation presentation);// = SDL_LOGICAL_PRESENTATION_LETTERBOX);
        void setWindowFlags(SDL_WindowFlags flags);// = SDL_WINDOW_RESIZABLE);
        void setPixelFormat(SDL_PixelFormat format);// = SDL_PIXELFORMAT_RGBA8888);

        // --- Factory & EventManager Access --- //
        Factory& getFactory() const { return *factory_; }
        bool getIsTraversing() const { return isTraversing_; }
        void setIsTraversing(bool traversing) { isTraversing_ = traversing; }

        // --- Focus & Hover Management --- //
        void handleTabKeyPress();
        void handleTabKeyPressReverse();
        void setKeyboardFocusedObject(DomHandle obj);
        DomHandle getKeyboardFocusedObject() const;
        void setMouseHoveredObject(DomHandle obj);
        DomHandle getMouseHoveredObject() const;

        // --- Window Title & Timing --- //
        std::string getWindowTitle() const { return windowTitle_; }
        void setWindowTitle(const std::string& title) 
        {
            windowTitle_ = title;
            if (window_)
                SDL_SetWindowTitle(window_, windowTitle_.c_str());
        }  
        float getElapsedTime() const { return fElapsedTime_; }  

        // --- Lua State Access --- //
        sol::state& getLua() { return lua_; }

        // --- Factory Wrappers --- //

        // --- Object Creation --- //
        DomHandle createDisplayObject(const std::string& typeName, const sol::table& config);
        DomHandle createDisplayObject(const std::string& typeName, const SDOM::IDisplayObject::InitStruct& init);
        DomHandle createDisplayObjectFromScript(const std::string& typeName, const std::string& luaScript);

        // --- Object Lookup --- //
        IDisplayObject* getDisplayObject(const std::string& name);
        IResourceObject* getResourceObject(const std::string& name);
        DomHandle getDisplayHandle(const std::string& name);
        DomHandle getFactoryStageHandle();
        bool hasDisplayObject(const std::string& name) const;

        // --- Display Object Management --- //
        void addDisplayObject(const std::string& name, std::unique_ptr<IDisplayObject> displayObject);
        void destroyDisplayObject(const std::string& name);

        // --- Orphan Management --- //
        int countOrphanedDisplayObjects() const;
        std::vector<DomHandle> getOrphanedDisplayObjects();
        void destroyOrphanedDisplayObjects();
        void detachOrphans();

        // --- Future Child Management --- //
        void attachFutureChildren();
        void addToOrphanList(const DomHandle orphan);
        void addToFutureChildrenList(const DomHandle child, const DomHandle parent,
            bool useWorld=false, int worldX=0, int worldY=0);

        // --- Utility Methods --- //
        std::vector<std::string> listDisplayObjectNames() const;
        void clearFactory();
        void printObjectRegistry() const;

        // --- Forwarding helpers to Factory for type-level introspection --- //
        std::vector<std::string> getPropertyNamesForType(const std::string& typeName) const;
        std::vector<std::string> getCommandNamesForType(const std::string& typeName) const;
        std::vector<std::string> getFunctionNamesForType(const std::string& typeName) const;

    private:
        // --- Singleton Enforcement --- //
        Core();
        ~Core();
        Core(const Core&) = delete;
        Core& operator=(const Core&) = delete;
        Core(Core&&) = delete;
        Core& operator=(Core&&) = delete;

        // --- SDL Resources --- //
        SDL_Window* window_ = nullptr;
        SDL_Renderer* renderer_ = nullptr;
        SDL_Texture* texture_ = nullptr;
        std::string windowTitle_ = "SDOM Application";
        SDL_Color color_ = { 0, 0, 0, 255 }; // Default BLACK background color

        // --- State --- //
        bool bIsRunning_ = true;
        bool isTraversing_ = false;
        float fElapsedTime_;             // Elapsed time since the last update

        // --- DOM --- //
        DomHandle rootNode_; // The root of the resource tree
        DomHandle hoveredObject_;
        DomHandle keyboardFocusedObject_;

        // --- Tab Priority --- //
        struct TabPriorityComparator {
            bool operator()(const DomHandle& a, const DomHandle& b) const {
                // If either handle is invalid, treat invalid handles as lower priority
                // so they end up at the back of the ordering. Avoid throwing an error
                // here because handles may become invalid between insertion and
                // comparison (e.g., when objects are destroyed during traversal).
                if (!a.isValid() && !b.isValid()) return false;
                if (!a.isValid()) return true;  // a < b
                if (!b.isValid()) return false; // a >= b
                return a->getTabPriority() < b->getTabPriority();
            }
        };
        std::priority_queue<DomHandle, std::vector<DomHandle>, TabPriorityComparator> tabList_;

        // --- Configuration --- //
        CoreConfig config_;

        // --- Lua State --- //
        sol::state lua_;

        // --- Subsystems --- //
        inline static Factory* factory_ = nullptr; 
        inline static EventManager* eventManager_ = nullptr;

        // --- Callback Hooks --- //
        std::function<bool()> fnOnInit = nullptr;
        std::function<void()> fnOnQuit = nullptr;
        std::function<void(float)> fnOnUpdate = nullptr;
        std::function<void(const Event&)> fnOnEvent = nullptr;
        std::function<void()> fnOnRender = nullptr;
        std::function<bool()> fnOnUnitTest = nullptr;
        std::function<void(int, int)> fnOnWindowResize = nullptr;

        // --- Private Helpers --- //
        void startup_SDL();
        void shutdown_SDL();
        void refreshSDLResources();
        bool coreTests_();

    protected:
        friend Factory;
    // --- Lua Registration --- //
    virtual void _registerLua(const std::string& typeName, sol::state_view lua);
        sol::usertype<Core> objHandleType_;
    };

} // namespace SDOM

