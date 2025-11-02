// IRangeControl.hpp
#pragma once

#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>

namespace SDOM
{
    // --- Enums and Tables --- //
    enum class Orientation { Horizontal, Vertical };
    inline static const std::map<Orientation, std::string> orientation_to_string = {
        { Orientation::Horizontal, "horizontal" },
        { Orientation::Vertical, "vertical" }
    };
    inline static const std::map<std::string, Orientation> string_to_orientation = {
        { "horizontal", Orientation::Horizontal },
        { "vertical", Orientation::Vertical }
    };   



    // --- IRangeControl Interface --- //

    class IRangeControl : public IDisplayObject 
    {
        using SUPER = IDisplayObject;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "IRangeControl";


        // --- Initialization Struct --- //
        struct InitStruct : IDisplayObject::InitStruct
        {
            InitStruct() : IDisplayObject::InitStruct()
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // Icon Color
                tabEnabled = false;
                isClickable = true;
            }
            // Additional IRangeControl Parameters
            float min = 0.0f;      // Minimum value (0.0% - 100.0%)
            float max = 100.0f;    // Maximum value (0.0% - 100.0%)
            float value = 0.0f;    // Current value (0.0% - 100.0%)
            Orientation orientation = Orientation::Horizontal;
            std::string icon_resource = "internal_icon_8x8"; 

        }; // END: InitStruct
        
    protected:
        // --- Protected Constructors --- //
        IRangeControl(const InitStruct& init);  
        IRangeControl(const sol::table& config);
        // Defaults-aware Lua constructor which accepts derived InitStruct defaults
        IRangeControl(const sol::table& config, const InitStruct& defaults);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new IRangeControl(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& IRangeControlInit = static_cast<const IRangeControl::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new IRangeControl(IRangeControlInit));
        }

        // --- Default Constructor and Destructor --- //
        IRangeControl() = default;
        virtual ~IRangeControl() = default;

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual bool onLoad() override;     // Allocate/refresh GPU resources after device rebuild
        virtual void onUnload() override;   // Release GPU resources before device teardown
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onEvent(const Event& event) override;  // Called when an event occurs
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onRender() override;   // Called to render the display object
        virtual bool onUnitTest(int frame) override; // Unit test method
        virtual void onWindowResize(int logicalWidth, int logicalHeight) override;


        // --- Public Accessors --- //
        float getMin() const;
        float getMax() const;
        float getValue() const;
        Orientation getOrientation() const;
        std::string getIconResource() const;


        // --- Public Mutators --- //
        void setMin(float v);
        void setMax(float v);
        void setValue(float v);
        SpriteSheet* getSpriteSheetPtr() const { return icon_sprite_sheet_; }

    protected:
        // --- Protected Data Members --- //
        float min_ = 0.0f;      // Minimum value (0.0% - 100.0%)
        float max_ = 100.0f;    // Maximum value (0.0% - 100.0%)
        float value_ = 0.0f;    // Current value (0.0% - 100.0%)
        Orientation orientation_ = Orientation::Horizontal;
        std::string icon_resource_ = "internal_icon_8x8"; // default icon resource name
        SpriteSheet* icon_sprite_sheet_ = nullptr; // SpriteSheet for the icon

        // When setMin/setMax update bounds and call setValue, store previous bounds here
        float pending_old_min_;
        float pending_old_max_;

        // --- Protected Virtual Methods --- //
        void setOrientation(Orientation o);

        // --- Protected Virtual Methods --- //
        virtual void _onValueChanged(float oldValue, float newValue);

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);        

        // --- Cached rendering --- //
        SDL_Texture* cachedTexture_ = nullptr;
        SDL_Renderer* cached_renderer_ = nullptr; // renderer that created cachedTexture_
        int current_width_ = 0;
        int current_height_ = 0;
        SDL_PixelFormat current_pixel_format_ = SDL_PIXELFORMAT_UNKNOWN;
        bool rebuildRangeTexture_(int width, int height, SDL_PixelFormat fmt);
    }; // END: class IRangeControl

} // END: namespace SDOM
