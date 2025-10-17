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
        struct InitStruct : IPanelObject::InitStruct
        {
            InitStruct() : IPanelObject::InitStruct() 
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // panel color
                isClickable = false;
                tabEnabled = false;
                // from IPanelObject
                base_index = PanelBaseIndex::Frame; 
                icon_resource = "internal_icon_8x8"; // Default to internal 8x8 sprite sheet
                icon_width = 8;        // default icon width is 8
                icon_height = 8;       // default icon height is 8
                font_resource = "internal_font_8x8"; // Default to internal 8x8 font 
                font_width = 8;        // default font width is 8
                font_height = 8;       // default font height is 8
            }
            // no additional members from Frame        
        }; // END: InitStruct
    protected:
        // --- Constructors --- //
        Frame(const InitStruct& init);  
        Frame(const sol::table& config);

    public:

        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new Frame(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& frameInit = static_cast<const Frame::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Frame(frameInit));
        }

        Frame() = default;
        virtual ~Frame() = default;     

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onRender() override;   // Called to render the display object
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onEvent(const Event& event) override;  // Called when an event occurs
        virtual bool onUnitTest() override; // Called to perform unit tests on the object

    protected:

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END: class Frame : public IPanelObject

} // END namespace SDOM