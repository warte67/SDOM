// SDOM_IRangeControl_scaffold.hpp
#pragma once

#include <SDOM/SDOM_IRangeControl.hpp>

/***** NOTE: 
* This is a scaffold class for IRangeControl. It provides basic structure and can be expanded with 
* additional functionality as needed.  It is not intended to be a fully functional range control on 
* its own.  It serves as a starting point for further development and customization.
*     
* It is still required to register the new DisplayObject type in the Factory either directly or during 
* the initialization of your own application that uses the SDOM framework.
* 
* Direct within the Factory::onInit() method:
*     ...
*     registerDomType("IRangeControl_scaffold", TypeCreators{
*         IRangeControl_scaffold::CreateFromLua,
*         IRangeControl_scaffold::CreateFromInitStruct
*     });
* 
* Or directly within your application initialization code:
*     Application::onInit() {
*         ...
*         Factory& factory = getFactory();
*         factory.registerDomType("IRangeControl_scaffold", TypeCreators{
*             IRangeControl_scaffold::CreateFromLua,
*             IRangeControl_scaffold::CreateFromInitStruct
*         });
*         ...
* 
*****/


namespace SDOM
{
    // --- Enums and Tables --- //

    // --- Slider Control --- //
    class IRangeControl_scaffold : public IRangeControl 
    {
        using SUPER = IRangeControl;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "IRangeControl_scaffold";

        // --- Initialization Struct --- //
        struct InitStruct : IRangeControl::InitStruct
        {
            InitStruct() : IRangeControl::InitStruct()
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // Icon Color
                tabEnabled = false;
                isClickable = true;
                // from IRangeControl
                min = 0.0f;      // Minimum value (0.0% - 100.0%)
                max = 100.0f;    // Maximum value (0.0% - 100.0%)
                value = 0.0f;    // Current value (0.0% - 100.0%)
                orientation = Orientation::Horizontal;
                std::string icon_resource = "internal_icon_8x8"; 
            }
            // Additional IRangeControl_scaffold Parameters as needed:
            // e.g., float step = 1.0f; // Step size for each increment/decrement
        }; // END: InitStruct
        
    protected:
        // --- Protected Constructors --- //
            // NOTE: when implementing a Lua constructor for a derived class you should
            // forward the derived class InitStruct() into the base-class Lua parser
            // so that derived defaults are applied when the Lua table omits keys.
            // Example:
            //   DerivedControl::DerivedControl(const sol::table& config)
            //     : IRangeControl(config, DerivedControl::InitStruct()) { }
            // This ensures color/anchor/font defaults from DerivedControl::InitStruct
            // are used by the IDisplayObject/IRangeControl parsing code.
            IRangeControl_scaffold(const InitStruct& init);  
            IRangeControl_scaffold(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new IRangeControl_scaffold(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& IRangeControl_scaffoldInit = static_cast<const IRangeControl_scaffold::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new IRangeControl_scaffold(IRangeControl_scaffoldInit));
        }

        // --- Default Constructor and Destructor --- //
        IRangeControl_scaffold() = default;
        virtual ~IRangeControl_scaffold() = default;

        // --- Virtual Methods --- //
        // Initialization: parse config and resolve handles (no GPU work here)
        virtual bool onInit() override;
        // Device lifecycle: base IRangeControl handles assets and cache drop safely.
        // Override if you need to extend the behavior; always call SUPER first.
        virtual bool onLoad() override;
        virtual void onUnload() override;
        // Shutdown
        virtual void onQuit() override;
        // Per-frame hooks
        virtual void onEvent(const Event& event) override;
        virtual void onUpdate(float fElapsedTime) override;
        virtual void onRender() override;
        // Unit test hook
        virtual bool onUnitTest(int frame) override;

        // onWindowResize contract for cached-texture widgets
        //
        // Display objects that cache renderer-owned resources (e.g., SDL_Texture*)
        // must invalidate and rebuild those resources whenever the SDL device
        // is recreated or when logical size/format changes. Core guarantees to
        // broadcast onWindowResize(logicalW, logicalH) in two cases:
        //   1) In response to SDL window resize events
        //   2) Immediately after Core reconfigures SDL (e.g., when setters like
        //      setPixelFormat/setWindowFlags trigger a device rebuild)
        //
        // To remain robust across device rebuilds, overrides should:
        //   - Prefer releasing GPU resources in onUnload() and (optionally)
        //     pre-warm in onLoad().
        //   - For onWindowResize, drop-only any cached SDL_Texture pointer
        //     (do NOT call SDL_DestroyTexture here) and let onRender() lazily rebuild.
        //   - Reset any tracked width/height/pixel format used for change detection
        //   - Clear any cached renderer pointer used to track the texture owner
        //   - Mark the object dirty so the next onRender() rebuilds the cache
        //
        // Tip: use SDOM::drop_invalid_cached_texture(tex, currentRenderer, ownerRenderer)
        // in your onRender() to proactively drop stale caches when the renderer
        // changes (e.g., fullscreen toggle) or when SDL reports the texture invalid.
        //
        // Derived controls that add additional cached resources should extend this
        // override and call SUPER::onWindowResize().
        virtual void onWindowResize(int logicalWidth, int logicalHeight) override;

        // --- Public Accessors --- //
        // Add custom getters here

        // --- Public Mutators --- //
        // Add custom setters here

    protected:
        // --- Protected Data Members --- //
        // Add custom data members here
        
        // --- Protected Virtual Methods --- //
        virtual void _onValueChanged(float oldValue, float newValue);

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);   
    }; // END: class IRangeControl_scaffold

} // END: namespace SDOM
