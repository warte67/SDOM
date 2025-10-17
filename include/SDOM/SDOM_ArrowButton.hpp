// SDOM_ArrowButton.hpp
#pragma once

#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_IconIndex.hpp>


namespace SDOM
{

    class ArrowButton : public IconButton
    {
        using SUPER = SDOM::IconButton; 

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "ArrowButton";

        // --- Enums and Tables --- //
        enum class ArrowDirection : int { Left, Right, Up, Down };
        inline static const std::map<ArrowDirection, std::string> arrow_direction_to_string = {
            { ArrowDirection::Left, "left" },
            { ArrowDirection::Right, "right" },
            { ArrowDirection::Up, "up" },
            { ArrowDirection::Down, "down" }
        };     
        inline static const std::map<std::string, ArrowDirection> string_to_arrow_direction = {
            { "left", ArrowDirection::Left },
            { "right", ArrowDirection::Right },
            { "up", ArrowDirection::Up },
            { "down", ArrowDirection::Down }
        };   
        enum class ArrowState : int { Raised, Depressed };
        static constexpr IconIndex ArrowIconTable[4][2] = {
            // ArrowDirection: Left, Right, Up, Down
            // ArrowState: Raised, Depressed
            { IconIndex::Arrow_Left_Raised,   IconIndex::Arrow_Left_Depressed   }, // Left
            { IconIndex::Arrow_Right_Raised,  IconIndex::Arrow_Right_Depressed  }, // Right
            { IconIndex::Arrow_Up_Raised,     IconIndex::Arrow_Up_Depressed     }, // Up
            { IconIndex::Arrow_Down_Raised,   IconIndex::Arrow_Down_Depressed   }  // Down
        };
        // --- Initialization Struct --- //
        struct InitStruct : IconButton::InitStruct
        {
            InitStruct() : IconButton::InitStruct() 
            { 
                // from IDisplayObject and IconButton
                x = 0;
                y = 0;
                width = 8;             
                height = 8;            
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // IconButton color
                tabEnabled = false;
                isClickable = true;
                std::string icon_resource = "internal_icon_8x8";
                icon_width = 8;
                icon_height = 8;
            }
            ArrowDirection direction = ArrowDirection::Up;
        }; // END: InitStruct
        
    protected:
        // --- Protected Constructors --- //
        ArrowButton(const InitStruct& init);  
        ArrowButton(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new ArrowButton(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& ArrowButtonInit = static_cast<const ArrowButton::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new ArrowButton(ArrowButtonInit));
        }

        // --- Default Constructor and Destructor --- //
        ArrowButton() = default;
        virtual ~ArrowButton() = default;

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onEvent(const Event& event) override;  // Called when an event occurs
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onRender() override;   // Called to render the display object
        virtual bool onUnitTest() override; // Called to perform unit tests on the object

        // --- Public Accessors --- //
        ArrowDirection getDirection() const { return direction_; }
        void setDirection(ArrowDirection dir) { direction_ = dir; setDirty(true); }
        ArrowState getArrowState() const { return arrowState_; }
        IconIndex getIconIndex() const { return ArrowIconTable[static_cast<int>(direction_)][static_cast<int>(arrowState_)]; }

    private:
        ArrowState arrowState_ = ArrowState::Raised; // internal use only

    protected:
        // --- Protected Data Members --- //
        ArrowDirection direction_ = ArrowDirection::Up;

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);
    }; // END: class ArrowButton

} // END: namespace SDOM