// SDOM_Checkbox.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
// #include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_TruetypeFont.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_Checkbox.hpp>



namespace SDOM
{

    Checkbox::Checkbox(const InitStruct& init) : IPanelObject(init)
    {
        // std::cout << "Box constructed with InitStruct: " << getName() 
        //           << " at address: " << this << std::endl;

        if (init.type != TypeName) {
            ERROR("Error: Checkbox constructed with incorrect type: " + init.type);
        }
        // Checkbox Specific
        text_ = init.text;
        base_index_ = init.base_index;
        font_size_ = init.font_size; 
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        font_resource_ = init.font_resource;
        icon_resource_ = init.icon_resource;
        use_border_ = init.border;
        label_color_ = init.label_color;
        border_color_ = init.border_color;
        isChecked_ = init.isChecked;
        icon_index_ = init.icon_index;
        icon_width_ = init.icon_width;
        icon_height_ = init.icon_height;

        setTabEnabled(false); // Checkboxs are not tab-enabled by default
        setClickable(true); // Checkboxs are not clickable by default
    }


    Checkbox::Checkbox(const sol::table& config) : IPanelObject(config)
    {
        // std::cout << "Box constructed with Lua config: " << getName() 
        //         << " at address: " << this << std::endl;            

        std::string type = config["type"].valid() ? config["type"].get<std::string>() : "";

        // INFO("Checkbox::Checkbox(const sol::table& config) -- name: " << getName() 
        //         << " type: " << type << " typeName: " << TypeName << std::endl 
        // ); // END INFO()

        if (type != TypeName) {
            ERROR("Error: Checkbox constructed with incorrect type: " + type);
        }
        
        InitStruct init;

        // Apply Lua-provided text property to the Checkbox
        if (config["text"].valid()) {
            try {
                text_ = config["text"].get<std::string>();
            } catch (...) {
                // defensive: ignore non-string values
            }
        }       
        // Apply Lua-provided font properties to the Checkbox (accept multiple key variants).
        // If a property is NOT present in the config, leave it as -1 to indicate
        // "unspecified" so the BitmapFont defaults helper can populate it.
        font_size_ = config["font_size"].valid() ? config["font_size"].get<int>() : init.font_size;
        font_width_ = config["font_width"].valid() ? config["font_width"].get<int>() : init.font_width;
        font_height_ = config["font_height"].valid() ? config["font_height"].get<int>() : init.font_height;
        icon_index_ = config["icon_index"].valid() ? config["icon_index"].get<IconIndex>() : init.icon_index;
        icon_width_ = config["icon_width"].valid() ? config["icon_width"].get<int>() : init.icon_width;
        icon_height_ = config["icon_height"].valid() ? config["icon_height"].get<int>() : init.icon_height;
        isChecked_ = config["is_checked"].valid() ? config["is_checked"].get<bool>() : init.isChecked;

        use_border_ = config["border"].valid() ? config["border"].get<bool>() : init.border;

        // INFO("Checkbox::Checkbox() - font_size: " << font_size_ 
        //         << " font_width: " << font_width_ 
        //         << " font_height: " << font_height_ 
        //         << " use_border: " << (use_border_ ? "true" : "false")
        // );

        // font resource name - accept either 'font_resource' or 'font_resource_name'
        if (config["font_resource"].valid()) {
            try { font_resource_ = config["font_resource"].get<std::string>(); }
            catch (...) { /* ignore */ }
        } else if (config["font_resource_name"].valid()) {
            try { font_resource_ = config["font_resource_name"].get<std::string>(); }
            catch (...) { /* ignore */ }
        }     
        // icon resource name - accept either 'icon_resource' or 'icon_resource_name'
        if (config["icon_resource"].valid()) {
            try { icon_resource_ = config["icon_resource"].get<std::string>(); }
            catch (...) { /* ignore */ }
        } else if (config["icon_resource_name"].valid()) {
            try { icon_resource_ = config["icon_resource_name"].get<std::string>(); }
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

            if (config["border_color"].valid() && config["border_color"].get_type() == sol::type::table) {
                sol::table bc = config["border_color"];
                border_color_.r = bc["r"].get_or((int)border_color_.r);
                border_color_.g = bc["g"].get_or((int)border_color_.g);
                border_color_.b = bc["b"].get_or((int)border_color_.b);
                border_color_.a = bc["a"].get_or((int)border_color_.a);
            } else if (config["borderColor"].valid() && config["borderColor"].get_type() == sol::type::table) {
                sol::table bc = config["borderColor"];
                border_color_.r = bc["r"].get_or((int)border_color_.r);
                border_color_.g = bc["g"].get_or((int)border_color_.g);
                border_color_.b = bc["b"].get_or((int)border_color_.b);
                border_color_.a = bc["a"].get_or((int)border_color_.a);
            }
        } catch (...) { /* ignore malformed color tables */ }

        // non-lua configurable essential init values
        base_index_ = init.base_index; // default to CheckboxUp
        
        setTabEnabled(false); // Checkboxs are not tab-enabled by default
        setClickable(true); // Checkboxs are not clickable by default
    }


    // --- Virtual Methods --- //
    bool Checkbox::onInit()
    {
        // create the IconButton object
        if (!iconButtonObject_)
        {
            IconButton::InitStruct init;
            init.name = getName() + "_iconbutton";
            init.type = "IconButton";
            init.icon_resource = icon_resource_;
            init.x = getX();
            init.y = getY();
            init.width = 8;
            init.height = 8;   
            init.isClickable = false;
            init.tabEnabled = false;
            init.color = getColor(); // use the color of the Checkbox to color the icon
            init.icon_index = isChecked_ ? IconIndex::Checked_Checkbox : IconIndex::Empty_Checkbox;
            iconButtonObject_ = getFactory().create("IconButton", init);
            addChild(iconButtonObject_);
        }

        // create the label object
        if (!labelObject_)
        {
            Label::InitStruct init;
            init.name = getName() + "_label";
            init.type = "Label";
            init.x = getX() + 12;
            init.y = getY() + 2;
            init.width = getWidth() - 12;
            init.height = 12;   
            init.alignment = LabelAlign::TOP_LEFT;
            init.anchorLeft = AnchorPoint::TOP_LEFT;
            init.anchorTop = AnchorPoint::TOP_LEFT; 
            init.anchorRight = AnchorPoint::TOP_LEFT;
            init.anchorBottom = AnchorPoint::TOP_LEFT;
            init.resourceName = font_resource_;
            init.border = use_border_;
            // init.color = label_color_;
            init.outlineColor = {0, 0, 0, 255};
            init.outlineThickness = 1;
            init.outline = true;
            init.borderThickness = 1;
            // init.dropshadow = true;
            init.foregroundColor = label_color_;
            init.borderColor = border_color_;
            init.color = label_color_;  // label color
            init.isClickable = false;
            init.tabEnabled = false;
            init.text = text_;
            init.fontSize = font_size_;
            init.fontWidth = font_width_;
            init.fontHeight = font_height_;

            // Fill missing font metrics from referenced BitmapFont asset so
            // 8x12 bitmap fonts render at native size instead of being
            // scaled down to 8x8.
            IFontObject::applyBitmapFontDefaults(getFactory(), init.resourceName,
                                                init.fontSize, init.fontWidth, init.fontHeight);

            // Debug: report what we will pass into the Label so we can trace
            // whether defaults were applied correctly.
            // INFO("Checkbox::onInit() - Label init metrics for '" + init.name + "': resource='" + init.resourceName + "' fontSize=" + std::to_string(init.fontSize) + " fontWidth=" + std::to_string(init.fontWidth) + " fontHeight=" + std::to_string(init.fontHeight));
            labelObject_ = getFactory().create("Label", init);
            addChild(labelObject_);
        }
        // center the icon vertically within the Checkbox height
        if (iconButtonObject_.isValid())
        {
            IconButton* iconBtn = iconButtonObject_.as<IconButton>();
            if (iconBtn)
            {
                int iconY = getY() + (getHeight() / 2 - icon_height_ / 2);
                iconBtn->setY(iconY);
                // Ensure the child IconButton reflects the current checked state
                iconBtn->setIconIndex(isChecked_ ? IconIndex::Checked_Checkbox : IconIndex::Empty_Checkbox);
            }
        }
        return SUPER::onInit();
    } // END: bool Checkbox::onInit()


    void Checkbox::onQuit()
    {
        SUPER::onQuit();
    } // END: void Checkbox::onQuit()


    void Checkbox::onRender()
    {
        // SUPER::onRender();

        // // temporary: render a border around the bounds of the checkbox
        // SDL_FRect rect = { float(this->getX()), float(this->getY()), float(this->getWidth()), float(this->getHeight()) };
        // SDL_Color focusColor = { 255,255,255, 64 }; // Gray color for focus
        // SDL_SetRenderDrawColor(getRenderer(), focusColor.r, focusColor.g, focusColor.b, focusColor.a); // Border color
        // SDL_RenderRect(getRenderer(), &rect);

        Label* label = labelObject_.as<Label>();
        if (!label) { ERROR("Error: Checkbox::onRender() - missing label object for: " + getName()); return; }

        // Debug info: report presence of internal icon and label children
        try {
            std::ostringstream dbg;
            dbg << "[DBG] Checkbox::onRender() - name='" << getName() << "' hasLabel=" << (label ? "yes" : "no");
            if (iconButtonObject_.isValid()) dbg << " iconChild='" << iconButtonObject_.getName() << "'";
            else dbg << " iconChild=none";
            LUA_INFO(dbg.str());
        } catch(...) {}

        // use the border settings from the Label Object
        FontStyle& fontStyle = label->getDefaultStyle();
        SDL_Color borderColor = fontStyle.borderColor;
        if (fontStyle.border) 
        {
            SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(getRenderer(), borderColor.r, borderColor.g, borderColor.b, borderColor.a);
            SDL_FRect rect = { 
                static_cast<float>(getX()), 
                static_cast<float>(getY()), 
                static_cast<float>(getWidth()), 
                static_cast<float>(getHeight()) };
            SDL_RenderRect(getRenderer(), &rect);
        }

    } // END: void Checkbox::onRender()


    void Checkbox::onUpdate(float fElapsedTime)
    {
    } // END: void Checkbox::onUpdate(float fElapsedTime)


    void Checkbox::onEvent(const Event& event)
    {
        // only target phase
        if (event.getPhase() != Event::Phase::Target) { return; }
        if (event.getType() == EventType::MouseClick) 
        { 
            // isChecked_ = !isChecked_;
            setChecked(!isChecked());
            // dispatch appropriate events
            // ...
            // ToDo:  Need new EventTypes
            //      EventType::Checked
            //      EventType::Unchecked
            //      EventType::Indeterminate (the third checkbox state)
        }

    } // END: void Checkbox::onEvent(const Event& event)

    bool Checkbox::isChecked() const
    {
        return isChecked_;
    } // END: bool isChecked() const


    void Checkbox::setChecked(bool checked)
    {
        if (isChecked_ == checked) return;   // no-op if unchanged
        // save the previous value and update to the new value
        bool previous = isChecked_;
        isChecked_ = checked;

        // Update internal IconButton child to use the new icon index immediately
        if (iconButtonObject_.isValid())
        {
            IconButton* iconBtn = iconButtonObject_.as<IconButton>();
            if (iconBtn)
            {
                iconBtn->setIconIndex(isChecked_ ? IconIndex::Checked_Checkbox : IconIndex::Empty_Checkbox);
                // mark child dirty so it redraws with new icon
                iconBtn->setDirty(true);
            }
        }

        // Mark for redraw / layout update as appropriate
        setDirty(true); // or whatever your IDisplayObject API uses

        // Dispatch EventType::StateChanged
        EventManager& evtManager = getCore().getEventManager();
        DisplayHandle thisObj = getFactory().getDisplayObject(getName());
        if (!thisObj.isValid())
        {
            ERROR("Error: Checkbox::setChecked() - cannot find self object for: " + getName());
            return;
        }
        auto stateChanged = std::make_unique<Event>(EventType::StateChanged, thisObj, getCore().getElapsedTime());
        DisplayHandle target = getFactory().getDisplayObject(getName());
        stateChanged->setTarget(target);
        stateChanged->setElapsedTime(getCore().getElapsedTime()); // Set elapsed time for the event

        // optional convenience payload
        stateChanged->setPayloadValue("previous_checked", previous);
        stateChanged->setPayloadValue("checked", isChecked_);

        // Dispatch the event to the event manager
        evtManager.addEvent(std::move(stateChanged));

    } // END: void Checkbox::setChecked(bool checked)





    // --- Lua Registration --- //

    void Checkbox::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Checkbox";
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

        // expose 'name' property for Checkbox (maps to getName / setName on the display object)
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
                    // ensure this is a Checkbox before casting
                    if (h->getType() != Checkbox::TypeName) return SDOM::DisplayHandle();
                    Checkbox* btn = static_cast<Checkbox*>(h.get());
                    return btn->getLabelObject();
                }
            ));
        }

        // --- additional Checkbox-specific bindings can go here --- //

    } // END: void Checkbox::_registerLuaBindings(const std::string& typeName, sol::state_view lua)



} // END: namespace SDOM