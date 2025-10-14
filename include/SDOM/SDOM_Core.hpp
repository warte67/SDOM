#pragma once

// #include <SDOM/SDOM.hpp>
#include <atomic>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>
// #include <SDOM/SDOM_DisplayHandle.hpp>

namespace SDOM
{
    class Factory;
    class Event;
    class EventType;
    class EventManager;
    class Stage;
    class IDisplayObject;
    class DisplayHandle;
    class IAssetObject;
    class AssetHandle;

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
        // Return true when the run completed successfully (no failing unit tests or runtime errors).
        bool run();
        bool run(const CoreConfig& config);          // Convenience overload to configure (from C++) and run
        bool run(const sol::table& config);      // Convenience overload to configure and run
        bool run(const std::string& configFile); // Convenience overload to load config from Lua file

        // Poll and dispatch pending SDL events once (test helper)
        void pumpEventsOnce();
        // Signal the main loop to stop. Provide a clear, non-generic message
        // so test output indicates this was a deliberate shutdown requested
        // from scripting (e.g. Lua's Core:quit()). Use the INFO logging macro
        // for consistency with the project's logging helpers.
        void isRunning(bool bRunning ) { bIsRunning_ = bRunning; }
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
        void reconfigure(const CoreConfig& config);

        // --- Callback/Hook Registration --- //
        void registerOnInit(std::function<bool()> fn) { fnOnInit = fn; }
        void registerOnQuit(std::function<void()> fn) { fnOnQuit = fn; }
        void registerOnUpdate(std::function<void(float)> fn) { fnOnUpdate = fn; }
        void registerOnEvent(std::function<void(const Event&)> fn) { fnOnEvent = fn; }
        void registerOnRender(std::function<void()> fn) { fnOnRender = fn; }
        void registerOnUnitTest(std::function<bool()> fn) { fnOnUnitTest = fn; }
        void registerOnWindowResize(std::function<void(int, int)> fn) { fnOnWindowResize = fn; }

        // --- Lua Registration Internal Helpers --- //
        void _fnOnInit(std::function<bool()> fn) { fnOnInit = fn; }
        void _fnOnQuit(std::function<void()> fn) { fnOnQuit = fn; }
        void _fnOnUpdate(std::function<void(float)> fn) { fnOnUpdate = fn; }
        void _fnOnEvent(std::function<void(const Event&)> fn) { fnOnEvent = fn; }
        void _fnOnRender(std::function<void()> fn) { fnOnRender = fn; }
        void _fnOnUnitTest(std::function<bool()> fn) { fnOnUnitTest = fn; }
        void _fnOnWindowResize(std::function<void(int, int)> fn) { fnOnWindowResize = fn; }
        std::function<bool()> _fnGetOnInit() { return fnOnInit; }
        std::function<void()> _fnGetOnQuit() { return fnOnQuit; }
        std::function<void(float)> _fnGetOnUpdate() { return fnOnUpdate; }
        std::function<void(const Event&)> _fnGetOnEvent() { return fnOnEvent; }
        std::function<void()> _fnGetOnRender() { return fnOnRender; }
        std::function<bool()> _fnGetOnUnitTest() { return fnOnUnitTest; }
        std::function<void(int, int)> _fnGetOnWindowResize() { return fnOnWindowResize; }

        // --- Stage/Root Node Management --- //
        void setRootNode(const std::string& name);
        void setRootNode(const DisplayHandle& handle);
        void setStage(const std::string& name); // Alias for backward compatibility
        Stage* getStage() const; // Alias for backward compatibility
        IDisplayObject* getRootNodePtr() const;
        DisplayHandle getRootNode() const;
        DisplayHandle getStageHandle() const { return getRootNode(); }  // Alias for backward compatibility

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
        void setWindowWidth(float width);
        void setWindowHeight(float height);
        void setPixelWidth(float width);
        void setPixelHeight(float height);
        void setPreserveAspectRatio(bool preserve);
        void setAllowTextureResize(bool allow);
        void setRendererLogicalPresentation(SDL_RendererLogicalPresentation presentation);
        void setWindowFlags(SDL_WindowFlags flags);
        void setPixelFormat(SDL_PixelFormat format);

        // --- Factory & EventManager Access --- //
        Factory& getFactory() const { return *factory_; }
        EventManager& getEventManager() const { return *eventManager_; }
        bool getIsTraversing() const { return isTraversing_; }
        void setIsTraversing(bool traversing) { isTraversing_ = traversing; }

        // --- Focus & Hover Management --- //
        void handleTabKeyPress();
        void handleTabKeyPressReverse();
        void setKeyboardFocusedObject(DisplayHandle obj);
        DisplayHandle getKeyboardFocusedObject() const;
        void clearKeyboardFocusedObject();
        void setMouseHoveredObject(DisplayHandle obj);
        DisplayHandle getMouseHoveredObject() const;

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
        DisplayHandle createDisplayObject(const std::string& typeName, const sol::table& config);
        DisplayHandle createDisplayObject(const std::string& typeName, const SDOM::IDisplayObject::InitStruct& init);
        DisplayHandle createDisplayObjectFromScript(const std::string& typeName, const std::string& luaScript);

        AssetHandle createAssetObject(const std::string& typeName, const sol::table& config);
        AssetHandle createAssetObject(const std::string& typeName, const SDOM::IAssetObject::InitStruct& init);
        AssetHandle createAssetObjectFromScript(const std::string& typeName, const std::string& luaScript);

        // --- Object Lookup --- //
        IDisplayObject* getDisplayObjectPtr(const std::string& name);
        DisplayHandle getDisplayObject(const std::string& name);
        // Backwards-compatibility: old name retained as a thin wrapper
        DisplayHandle getDisplayObjectHandle(const std::string& name) { return getDisplayObject(name); }
        bool hasDisplayObject(const std::string& name) const;

        IAssetObject* getAssetObjectPtr(const std::string& name);
        AssetHandle getAssetObject(const std::string& name);
        bool hasAssetObject(const std::string& name) const;

        // --- Display Object Management --- //
        void addDisplayObject(const std::string& name, std::unique_ptr<IDisplayObject> displayObject);
        void destroyDisplayObject(const std::string& name);

        // --- Orphan Management --- //
        int countOrphanedDisplayObjects() const;
        std::vector<DisplayHandle> getOrphanedDisplayObjects();
        void destroyOrphanedDisplayObjects();
        void detachOrphans();

        // --- Future Child Management --- //
        void attachFutureChildren();
        void addToOrphanList(const DisplayHandle& orphan);
        void addToFutureChildrenList(const DisplayHandle& child, const DisplayHandle& parent,
            bool useWorld=false, int worldX=0, int worldY=0);

        // --- Utility Methods --- //
        std::vector<std::string> listDisplayObjectNames() const;
        void clearFactory();
        void printObjectRegistry() const;

        // --- Forwarding helpers to Factory for type-level introspection --- //
        std::vector<std::string> getPropertyNamesForType(const std::string& typeName) const;
        std::vector<std::string> getCommandNamesForType(const std::string& typeName) const;
        std::vector<std::string> getFunctionNamesForType(const std::string& typeName) const;

        // --- For Testing --- //
        void setStopAfterUnitTests(bool stop) { stopAfterUnitTests_ = stop; }
        bool getStopAfterUnitTests() { return stopAfterUnitTests_; }

        // --- Input filtering for tests --- //
        // When true, real mouse input from SDL_PollEvent() is ignored so
        // synthetic events queued by unit tests are not interfered with by
        // the user's mouse movements.
        void setIgnoreRealInput(bool v) { ignoreRealInput_ = v; }
        bool getIgnoreRealInput() const { return ignoreRealInput_; }

        // --- Keyfocus gray level for testing/focus indication --- //
        float getKeyfocusGray() const { return keyfocus_gray_; }
        void setKeyfocusGray(float gray) { keyfocus_gray_ = gray; }

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
        bool ignoreRealInput_ = false; // when true, drop real SDL mouse events during tests
        float keyfocus_gray_ = 0.0f;  // used for keyfocus indication

        // Track whether SDL has been started for this Core instance. Using a
        // member instead of a function-static variable keeps state tied to
        // the instance lifetime and is easier to reason about in tests.
        bool sdlStarted_ = false;

        // For testing: stop main loop after unit tests
        bool stopAfterUnitTests_ = false; 

        // Deferred-apply support: allow worker threads to request a
        // CoreConfig change which will be applied on the main thread
        // during a safe point (start of frame / event pumping).
        std::mutex pendingConfigMutex_;
        std::atomic_bool pendingConfigRequested_{false};
        CoreConfig pendingConfig_;

        // --- DOM --- //
        DisplayHandle rootNode_; // The root of the resource tree
        DisplayHandle hoveredObject_;
        DisplayHandle keyboardFocusedObject_;

        // --- Tab Priority --- //
        struct TabPriorityComparator {
            bool operator()(const DisplayHandle& a, const DisplayHandle& b) const {
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
        std::priority_queue<DisplayHandle, std::vector<DisplayHandle>, TabPriorityComparator> tabList_;

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
        void shutdown_SDL();    // Gracefully shutdown SDL subsystems and free resources
        void refreshSDLResources(); // Initialize or refresh SDL resources (window, renderer, texture) as needed
        bool coreTests_();  // some basic initial internal Core Unit Tests

        // Request that the given config be applied on the main thread
        // at the next safe point. Thread-safe.
        void requestConfigApply(const CoreConfig& cfg);
        // Called on main thread to apply any pending config request.
        void applyPendingConfig();

    protected:
        friend Factory;

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua) override;
        sol::usertype<Core> objHandleType_;


    };

} // namespace SDOM

