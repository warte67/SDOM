// SDOM_Button.cpp

// SDOM_Button.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_Button.hpp>



namespace SDOM
{

    Button::Button(const InitStruct& init) : IPanelObject(init)
    {
        // std::cout << "Box constructed with InitStruct: " << getName() 
        //           << " at address: " << this << std::endl;

        if (init.type != TypeName) {
            ERROR("Error: Button constructed with incorrect type: " + init.type);
        }
        // Button Specific
        text_ = init.text;
        base_index_ = init.base_index;
        font_size_ = init.font_size; 
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        font_resource_ = init.font_resource;
    }


    Button::Button(const sol::table& config) : IPanelObject(config)
    {
        // std::cout << "Box constructed with Lua config: " << getName() 
        //         << " at address: " << this << std::endl;            

        std::string type = config["type"].valid() ? config["type"].get<std::string>() : "";

        // INFO("Button::Button(const sol::table& config) -- name: " << getName() 
        //         << " type: " << type << " typeName: " << TypeName << std::endl 
        // ); // END INFO()

        if (type != TypeName) {
            ERROR("Error: Button constructed with incorrect type: " + type);
        }
        
        InitStruct init;

        // Apply Lua-provided text property to the Button
        if (config["text"].valid()) {
            try {
                text_ = config["text"].get<std::string>();
            } catch (...) {
                // defensive: ignore non-string values
            }
        }       
        // Apply Lua-provided font properties to the Button (accept multiple key variants)
        try {
            if (config["font_size"].valid()) font_size_ = config["font_size"].get<int>();
        } catch (...) { /* ignore invalid types */ }

        try {
            if (config["font_width"].valid()) font_width_ = config["font_width"].get<int>();
        } catch (...) { /* ignore invalid types */ }

        try {
            if (config["font_height"].valid()) font_height_ = config["font_height"].get<int>();
        } catch (...) { /* ignore invalid types */ }

        // font resource name - accept either 'font_resource' or 'font_resource_name'
        if (config["font_resource"].valid()) {
            try { font_resource_name_ = config["font_resource"].get<std::string>(); }
            catch (...) { /* ignore */ }
        } else if (config["font_resource_name"].valid()) {
            try { font_resource_name_ = config["font_resource_name"].get<std::string>(); }
            catch (...) { /* ignore */ }
        }     

        // non-lua configurable essential init values
        base_index_ = init.base_index; // default to ButtonUp
    }


    // --- Virtual Methods --- //
    bool Button::onInit()
    {
        // create the label object
        if (!labelObject_)
        {
            Label::InitStruct init;
            init.name = getName() + "_label";
            init.type = "Label";
            init.x = getX() + 4;
            init.y = getY() + 2;
            init.width = getWidth() - 8;
            init.height = getHeight() - 4;
            init.alignment = LabelAlign::CENTER;
            init.anchorLeft = AnchorPoint::TOP_LEFT;
            init.anchorTop = AnchorPoint::TOP_LEFT; 
            init.anchorRight = AnchorPoint::BOTTOM_RIGHT;
            init.anchorBottom = AnchorPoint::BOTTOM_RIGHT;
            init.resourceName = font_resource_name_;
            init.text = text_;
            init.fontSize = font_size_;
            init.fontWidth = font_width_;
            init.fontHeight = font_height_;
            labelObject_ = getFactory().create("Label", init);
            addChild(labelObject_);
        }

        return SUPER::onInit();
    } // END: bool Button::onInit()


    void Button::onQuit()
    {
        SUPER::onQuit();
    } // END: void Button::onQuit()


    void Button::onRender()
    {
        SUPER::onRender();
    } // END: void Button::onRender()


    void Button::onUpdate(float fElapsedTime)
    {
        (void)fElapsedTime;
    } // END: void Button::onUpdate(float fElapsedTime)


    void Button::onEvent(const Event& event)
    {
        // only target phase
        if (event.getPhase() != Event::Phase::Target) 
        {
            return;
        }
        if (event.getType() == EventType::MouseEnter)
        {
            base_index_ = PanelBaseIndex::ButtonUpSelected;
            // std::cout << "Button " << getName() << " is hovered." << std::endl;
            setState(ButtonState::Hovered);
            setDirty();
        }
        if (event.getType() == EventType::MouseLeave)
        {
            base_index_ = PanelBaseIndex::ButtonUp;
            // std::cout << "Button " << getName() << " is not hovered." << std::endl;
            setState(ButtonState::Normal);
            setDirty();
        }
        if (event.getType() == EventType::MouseButtonDown)
        {
            base_index_ = PanelBaseIndex::ButtonDown;
            // std::cout << "Button " << getName() << " is pressed." << std::endl;
            setState(ButtonState::Pressed);
            setDirty();
        }
        if (event.getType() == EventType::MouseButtonUp)
        {
            base_index_ = PanelBaseIndex::ButtonDownSelected;
            // std::cout << "Button " << getName() << " is released." << std::endl;
            setState(ButtonState::Hovered);
            setDirty();
        }
    } // END: void Button::onEvent(const Event& event)


    // --- Lua Registration --- //
    void Button::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Button";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // Augment the single shared DisplayHandle handle usertype
        sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);

        // Helper to check if a property/command is already registered
        auto absent = [&](const char* name) -> bool {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        // expose 'name' property for Button (maps to getName / setName on the display object)
        if (absent("name"))
        {
            handle.set("name", sol::property(
                // getter
                [](SDOM::DisplayHandle h) -> std::string {
                    if (h.isValid()) return h->getName();
                    return std::string();
                },
                // setter
                [](SDOM::DisplayHandle h, const std::string& v) {
                    if (h.isValid()) h->setName(v);
                }
            ));
        }

        // --- additional Button-specific bindings can go here --- //

    } // END: void Button::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM