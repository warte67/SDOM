// SDOM_Frame.hpp
#pragma once

#include <SDOM/SDOM_IPanelObject.hpp>

namespace SDOM
{

    class Frame : public IPanelObject
    {
        using SUPER = SDOM::IPanelObject; 

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "Frame";

        // --- Initialization Struct --- //
        struct InitStruct : public IPanelObject::InitStruct
        {
            InitStruct()
                : IPanelObject::InitStruct()
            {
                //
                // Override defaults for Frame
                //
                name        = TypeName;
                type        = TypeName;

                // Frame color (purple-ish default)
                color       = {96, 0, 96, 255};

                // UI behavior overrides
                isClickable = false;
                tabEnabled  = false;

                //
                // IPanelObject-specific overrides for Frame
                //
                base_index    = PanelBaseIndex::Frame;

                icon_resource = "internal_icon_8x8";
                icon_width    = 8;
                icon_height   = 8;

                font_resource = "internal_font_8x8";
                font_width    = 8;
                font_height   = 8;
            }

            // No additional JSON fields → inherit JSON parser safely.
        }; // END: InitStruct

    protected:
        // --- Constructors --- //
        Frame(const InitStruct& init);  

    public:

        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const Frame::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Frame(init));
        }

        static std::unique_ptr<IDisplayObject> CreateFromJson(const nlohmann::json& j)
        {
            Frame::InitStruct init;
            Frame::InitStruct::from_json(j, init); // inherited fields handled internally
            return std::unique_ptr<IDisplayObject>(new Frame(init));
        }


        Frame() = default;
        ~Frame() override = default;     

        // --- Virtual Methods --- //
        bool onInit() override;     // Called when the display object is initialized
        void onRender() override;   // Called to render the display object
        void onQuit() override;     // Called when the display object is being destroyed
        void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        void onEvent(const Event& event) override;  // Called when an event occurs
        bool onUnitTest(int frame) override;        // Called to run unit tests

    protected:

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;      

    }; // END: class Frame : public IPanelObject

} // END namespace SDOM