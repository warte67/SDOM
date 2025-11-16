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
        struct InitStruct : public IRangeControl::InitStruct
        {
            InitStruct()
                : IRangeControl::InitStruct()
            {
                //
                // From IDisplayObject
                //
                name       = TypeName;
                type       = TypeName;

                isClickable = true;
                tabEnabled  = false;

                //
                // Range defaults
                //
                min         = 0.0f;
                max         = 100.0f;
                value       = 0.0f;
                orientation = Orientation::Horizontal;
                
                // assign to member, not a local variable
                icon_resource = "internal_icon_8x8";
            }

            //
            // Range control members
            //
            float min         = 0.0f;
            float max         = 100.0f;
            float value       = 0.0f;
            Orientation orientation = Orientation::Horizontal;
            std::string icon_resource = "internal_icon_8x8";

            // ---------------------------------------------------------------------
            // JSON loader (inheritance-safe)
            // ---------------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                IDisplayObject::InitStruct::from_json(j, out);

                if (j.contains("min"))
                    out.min = j["min"].get<float>();

                if (j.contains("max"))
                    out.max = j["max"].get<float>();

                if (j.contains("value"))
                    out.value = j["value"].get<float>();

                if (j.contains("orientation"))
                    out.orientation = static_cast<Orientation>( j["orientation"].get<int>() );

                if (j.contains("icon_resource"))
                    out.icon_resource = j["icon_resource"].get<std::string>();
            }
        };

        
    protected:
        // --- Protected Constructors --- //
        IRangeControl_scaffold(const InitStruct& init);  

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject>
        CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const IRangeControl_scaffold::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new IRangeControl_scaffold(init));
        }

        static std::unique_ptr<IDisplayObject>
        CreateFromJson(const nlohmann::json& j)
        {
            IRangeControl_scaffold::InitStruct init;
            IRangeControl_scaffold::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new IRangeControl_scaffold(init));
        }

        // --- Default Constructor and Destructor --- //
        IRangeControl_scaffold() = default;
        ~IRangeControl_scaffold() override = default;

        // --- Virtual Methods --- //
        // Initialization: parse config and resolve handles (no GPU work here)
        bool onInit() override;
        // Device lifecycle: base IRangeControl handles assets and cache drop safely.
        // Override if you need to extend the behavior; always call SUPER first.
        bool onLoad() override;
        void onUnload() override;
        // Shutdown
        void onQuit() override;
        // Per-frame hooks
        void onEvent(const Event& event) override;
        void onUpdate(float fElapsedTime) override;
        void onRender() override;
        // Unit test hook
        bool onUnitTest(int frame) override;

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
        void onWindowResize(int logicalWidth, int logicalHeight) override;

        // --- Public Accessors --- //
        // Add custom getters here

        // --- Public Mutators --- //
        // Add custom setters here

    protected:
        // --- Protected Data Members --- //
        // Add custom data members here
        
        // --- Protected Virtual Methods --- //
        void _onValueChanged(float oldValue, float newValue) override;

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         

        
    }; // END: class IRangeControl_scaffold

} // END: namespace SDOM
