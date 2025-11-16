// IRangeControl.hpp
#pragma once

#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <algorithm>
#include <cctype>

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
        struct InitStruct : public IDisplayObject::InitStruct
        {
            InitStruct()
                : IDisplayObject::InitStruct()
            {
                //
                // From IDisplayObject
                //
                name = TypeName;
                type = TypeName;

                color      = {96, 0, 96, 255};  // default control tint
                tabEnabled = false;
                isClickable = true;

                //
                // IRangeControl-specific defaults
                //
                min         = 0.0f;
                max         = 100.0f;
                value       = 0.0f;
                orientation = Orientation::Horizontal;

                icon_resource = "internal_icon_8x8";   // NOTE: fixed — no local variable shadowing!
            }

            //
            // IRangeControl-specific fields
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

                if (j.contains("min"))   out.min   = j["min"].get<float>();
                if (j.contains("max"))   out.max   = j["max"].get<float>();
                if (j.contains("value")) out.value = j["value"].get<float>();

                if (auto it = j.find("orientation"); it != j.end())
                {
                    if (it->is_string())
                    {
                        std::string key = it->get<std::string>();
                        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                        auto mapIt = string_to_orientation.find(key);
                        if (mapIt != string_to_orientation.end())
                        {
                            out.orientation = mapIt->second;
                        }
                    }
                    else if (it->is_number_integer())
                    {
                        out.orientation = static_cast<Orientation>(it->get<int>());
                    }
                }

                if (j.contains("icon_resource"))
                    out.icon_resource = j["icon_resource"].get<std::string>();
            }
        }; // END: InitStruct
        
        
    protected:
        // --- Protected Constructors --- //
        IRangeControl(const InitStruct& init);  

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject>
        CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const IRangeControl::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new IRangeControl(init));
        }

        static std::unique_ptr<IDisplayObject>
        CreateFromJson(const nlohmann::json& j)
        {
            IRangeControl::InitStruct init;
            IRangeControl::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new IRangeControl(init));
        }


        // --- Default Constructor and Destructor --- //
        IRangeControl() = default;
        ~IRangeControl() override = default;

        // --- Virtual Methods --- //
        bool onInit() override;     // Called when the display object is initialized
        bool onLoad() override;     // Allocate/refresh GPU resources after device rebuild
        void onUnload() override;   // Release GPU resources before device teardown
        void onQuit() override;     // Called when the display object is being destroyed
        void onEvent(const Event& event) override;  // Called when an event occurs
        void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        void onRender() override;   // Called to render the display object
        bool onUnitTest(int frame) override; // Unit test method
        void onWindowResize(int logicalWidth, int logicalHeight) override;


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

        // --- Cached rendering --- //
        SDL_Texture* cachedTexture_ = nullptr;
        SDL_Renderer* cached_renderer_ = nullptr; // renderer that created cachedTexture_
        int current_width_ = 0;
        int current_height_ = 0;
        SDL_PixelFormat current_pixel_format_ = SDL_PIXELFORMAT_UNKNOWN;
        bool rebuildRangeTexture_(int width, int height, SDL_PixelFormat fmt);

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         

    }; // END: class IRangeControl

} // END: namespace SDOM
