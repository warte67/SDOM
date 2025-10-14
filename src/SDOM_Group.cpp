// SDOM_Group.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_TruetypeFont.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_Group.hpp>



namespace SDOM
{

    Group::Group(const InitStruct& init) : IPanelObject(init)
    {
        // std::cout << "Box constructed with InitStruct: " << getName() 
        //           << " at address: " << this << std::endl;

        if (init.type != TypeName) {
            ERROR("Error: Group constructed with incorrect type: " + init.type);
        }
        // Group Specific
        text_ = init.text;
        base_index_ = init.base_index;
        font_size_ = init.font_size; 
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        font_resource_ = init.font_resource;

        setTabEnabled(false); // Groups are not tab-enabled by default
        setClickable(false); // Groups are not clickable by default

    }


    Group::Group(const sol::table& config) : IPanelObject(config)
    {
        // std::cout << "Box constructed with Lua config: " << getName() 
        //         << " at address: " << this << std::endl;            

        std::string type = config["type"].valid() ? config["type"].get<std::string>() : "";

        // INFO("Group::Group(const sol::table& config) -- name: " << getName() 
        //         << " type: " << type << " typeName: " << TypeName << std::endl 
        // ); // END INFO()

        if (type != TypeName) {
            ERROR("Error: Group constructed with incorrect type: " + type);
        }
        
        InitStruct init;

        // Apply Lua-provided text property to the Group
        if (config["text"].valid()) {
            try {
                text_ = config["text"].get<std::string>();
            } catch (...) {
                // defensive: ignore non-string values
            }
        }       
        // Apply Lua-provided font properties to the Group (accept multiple key variants)
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

        // label color - accept snake_case/table { r=.., g=.., b=.., a=.. } or camelCase 'labelColor'
        try {
            if (config["label_color"].valid() && config["label_color"].get_type() == sol::type::table) {
                sol::table lc = config["label_color"];
                label_color_.r = lc["r"].get_or((int)label_color_.r);
                label_color_.g = lc["g"].get_or((int)label_color_.g);
                label_color_.b = lc["b"].get_or((int)label_color_.b);
                label_color_.a = lc["a"].get_or((int)label_color_.a);
            } else if (config["labelColor"].valid() && config["labelColor"].get_type() == sol::type::table) {
                sol::table lc = config["labelColor"];
                label_color_.r = lc["r"].get_or((int)label_color_.r);
                label_color_.g = lc["g"].get_or((int)label_color_.g);
                label_color_.b = lc["b"].get_or((int)label_color_.b);
                label_color_.a = lc["a"].get_or((int)label_color_.a);
            } else if (config["foreground_color"].valid() && config["foreground_color"].get_type() == sol::type::table) {
                sol::table lc = config["foreground_color"];
                label_color_.r = lc["r"].get_or((int)label_color_.r);
                label_color_.g = lc["g"].get_or((int)label_color_.g);
                label_color_.b = lc["b"].get_or((int)label_color_.b);
                label_color_.a = lc["a"].get_or((int)label_color_.a);
            } else if (config["foregroundColor"].valid() && config["foregroundColor"].get_type() == sol::type::table) {
                sol::table lc = config["foregroundColor"];
                label_color_.r = lc["r"].get_or((int)label_color_.r);
                label_color_.g = lc["g"].get_or((int)label_color_.g);
                label_color_.b = lc["b"].get_or((int)label_color_.b);
                label_color_.a = lc["a"].get_or((int)label_color_.a);
            }
        } catch (...) { /* ignore malformed color tables */ }

        // non-lua configurable essential init values
        base_index_ = init.base_index; // default to GroupUp
        
        setTabEnabled(false); // Groups are not tab-enabled by default
        setClickable(false); // Groups are not clickable by default
    }


    // --- Virtual Methods --- //
    bool Group::onInit()
    {
        // create the label object
        if (!labelObject_)
        {
            Label::InitStruct init;
            init.name = getName() + "_label";
            init.type = "Label";
            init.x = getX() + 8;
            init.y = (getY() - 2);
            init.width = getWidth() - 16;
            init.height = 12;   
            init.alignment = LabelAlign::TOP_LEFT;
            init.anchorLeft = AnchorPoint::TOP_LEFT;
            init.anchorTop = AnchorPoint::TOP_LEFT; 
            init.anchorRight = AnchorPoint::TOP_LEFT;
            init.anchorBottom = AnchorPoint::TOP_LEFT;
            init.resourceName = font_resource_name_;
            // init.color = label_color_;
            init.outlineColor = {0, 0, 0, 255};
            init.outlineThickness = 1;
            init.outline = true;
            init.dropshadow = true;
            init.foregroundColor = label_color_;
            init.color = label_color_;
            init.isClickable = false;
            init.tabEnabled = false;
            init.text = text_;
            init.fontSize = font_size_;
            init.fontWidth = font_width_;
            init.fontHeight = font_height_;
            labelObject_ = getFactory().create("Label", init);
            addChild(labelObject_);

            // Center the label vertically within the upper bounds of the Group
            Label* lbl = labelObject_.as<Label>();
            if (!lbl) {
                ERROR("Error: Group::onInit() -- failed to cast labelObject_ to Label*");
                return false;   
            }    
            // SpriteSheet* objSS = spriteSheetAsset_.as<SpriteSheet>();
            // if (!objSS) {
            //     ERROR("Error: Group::onInit() -- failed to cast fontAsset_ to SpriteSheet*");
            //     return false;   
            // }

            // just use +4 for now to nudge it a bit
            labelObject_->setY((getY() + 4 ) - (lbl->getGlyphHeight() / 2));


        }

        return SUPER::onInit();
    } // END: bool Group::onInit()


    void Group::onQuit()
    {
        SUPER::onQuit();
    } // END: void Group::onQuit()


    void Group::onRender()
    {
        SUPER::onRender();

    } // END: void Group::onRender()


    void Group::onUpdate(float fElapsedTime)
    {
    } // END: void Group::onUpdate(float fElapsedTime)


    void Group::onEvent(const Event& event)
    {
        // only target phase
        if (event.getPhase() != Event::Phase::Target) 
        {
            return;
        }

    } // END: void Group::onEvent(const Event& event)


    // --- Lua Registration --- //

    void Group::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Group";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // Augment the single shared DisplayHandle handle usertype
        sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);

        // Helper to check if a property/command is already registered
        auto absent = [&](const char* name) -> bool 
        {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        // expose 'name' property for Group (maps to getName / setName on the display object)
        if (absent("name"))
        {
            handle.set("name", sol::property(
                // getter
                [](SDOM::DisplayHandle h) -> std::string 
                {
                    if (h.isValid()) return h->getName();
                    return std::string();
                },
                // setter
                [](SDOM::DisplayHandle h, const std::string& v) 
                {
                    if (h.isValid()) h->setName(v);
                }
            ));
        }

        // expose 'label' property that returns the attached label DisplayHandle (read-only)
        if (absent("label"))
        {
            handle.set("label", sol::property(
                // getter
                [](SDOM::DisplayHandle h) -> SDOM::DisplayHandle 
                {
                    if (!h.isValid()) return SDOM::DisplayHandle();
                    // ensure this is a Group before casting
                    if (h->getType() != Group::TypeName) return SDOM::DisplayHandle();
                    Group* btn = static_cast<Group*>(h.get());
                    return btn->getLabelObject();
                }
            ));
        }

        // --- additional Group-specific bindings can go here --- //

    } // END: void Group::_registerLuaBindings(const std::string& typeName, sol::state_view lua)



} // END: namespace SDOM