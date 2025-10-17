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
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onEvent(const Event& event) override;  // Called when an event occurs
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onRender() override;   // Called to render the display object
        virtual bool onUnitTest() override; // Unit test method

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