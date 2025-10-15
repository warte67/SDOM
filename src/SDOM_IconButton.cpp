// SDOM_IconButton.hpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IconButton.hpp>



namespace SDOM
{

    IconButton::IconButton(const InitStruct& init) : IDisplayObject(init)
    {
        // std::cout << "Box constructed with InitStruct: " << getName() 
        //           << " at address: " << this << std::endl;

        if (init.type != TypeName) {
            ERROR("Error: IconButton constructed with incorrect type: " + init.type);
        }
        // IconButton Specific
        icon_resource_ = init.icon_resource;
        icon_index_ = init.icon_index;
        icon_width_ = init.icon_width;
        icon_height_ = init.icon_height;
        setX(init.x);
        setY(init.y);
        setWidth(icon_width_);
        setHeight(icon_height_);
    }


    IconButton::IconButton(const sol::table& config) : IDisplayObject(config)
    {
        // std::cout << "Box constructed with Lua config: " << getName() 
        //         << " at address: " << this << std::endl;            

        std::string type = config["type"].valid() ? config["type"].get<std::string>() : "";

        // INFO("IconButton::IconButton(const sol::table& config) -- name: " << getName() 
        //         << " type: " << type << " typeName: " << TypeName << std::endl 
        // ); // END INFO()

        if (type != TypeName) {
            ERROR("Error: IconButton constructed with incorrect type: " + type);
        }
        
        InitStruct init;

        // robust parsing for icon_index: accept string names or numeric indices
        auto parse_icon_index = [&](const sol::table& cfg, const char* key, IconIndex def) -> IconIndex 
        {
            if (!cfg[key].valid()) return def;
            sol::object obj = cfg[key];
            if (obj.is<std::string>()) 
            {
                auto opt = SDOM::icon_index_from_name(obj.as<std::string>());
                return opt.has_value() ? opt.value() : def;
            } 
            else if (obj.is<int>()) 
            {
                return static_cast<IconIndex>(obj.as<int>());
            } 
            else 
            {
                try {
                    return obj.as<IconIndex>();
                } catch (...) {
                    return def;
                }
            }
        };
        // IconButton Specific Properties
        icon_index_ = parse_icon_index(config, "icon_index", init.icon_index);
        icon_resource_ = config["icon_resource"].valid() ? config["icon_resource"].get<std::string>() : init.icon_resource;
        icon_width_ = config["icon_width"].valid() ? config["icon_width"].get<int>() : init.icon_width;
        icon_height_ = config["icon_height"].valid() ? config["icon_height"].get<int>() : init.icon_height;
        int x_pos = config["x"].valid() ? config["x"].get<int>() : init.x;
        int y_pos = config["y"].valid() ? config["y"].get<int>() : init.y;
        int width = config["width"].valid() ? config["width"].get<int>() : icon_width_;
        int height = config["height"].valid() ? config["height"].get<int>() : icon_height_;
        setX(x_pos);    
        setY(y_pos);
        setWidth(width);
        setHeight(height);        
    }


    // --- Virtual Methods --- //
    bool IconButton::onInit()
    {
        // create the label object
        if (!iconSpriteSheet_)
        {
            SpriteSheet::InitStruct init_struct;
            init_struct.name = getName() + "_IconSpriteSheet";
            init_struct.type = SpriteSheet::TypeName;
            init_struct.filename = icon_resource_;
            init_struct.spriteWidth = icon_width_;
            init_struct.spriteHeight = icon_height_;
            iconSpriteSheet_ = getFactory().createAsset("SpriteSheet", init_struct);
            if (!iconSpriteSheet_ || !iconSpriteSheet_.isValid())
            {
                ERROR("IconButton::onInit() - Failed to create or load icon SpriteSheet asset: " + icon_resource_);
                return false;
            }
        }

        return SUPER::onInit();
    } // END: bool IconButton::onInit()


    void IconButton::onQuit()
    {
        SUPER::onQuit();
    } // END: void IconButton::onQuit()


    void IconButton::onRender()
    {
        // SUPER::onRender();

        SpriteSheet* ss = iconSpriteSheet_.as<SpriteSheet>();
        if (ss && ss->isLoaded())
        {
            ss->drawSprite(
                static_cast<int>(icon_index_), // icon index
                static_cast<int>(getX()),
                static_cast<int>(getY()),
                getColor());
        }

    } // END: void IconButton::onRender()


    void IconButton::onUpdate(float fElapsedTime)
    {
    } // END: void IconButton::onUpdate(float fElapsedTime)


    void IconButton::onEvent(const Event& event)
    {
        // only target phase
        if (event.getPhase() != Event::Phase::Target) 
        {
            return;
        }
        if (event.getType() == EventType::MouseClick)
        {
            INFO("IconButton::onEvent() - Event: " << event.getTypeName() 
                    << " on IconButton: " << getName() 
                    << " at address: " << this
            ); // END INFO()
        }

    } // END: void IconButton::onEvent(const Event& event)


    // --- Lua Registration --- //

    void IconButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IconButton";
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

        // expose 'name' property for IconButton (maps to getName / setName on the display object)
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

        // --- additional IconButton-specific bindings can go here --- //

        // Helper: expose IconIndex enum and convenient IconButton constants to Lua.
        // Creates a global `IconIndex` table and also adds the same constants to the IconButton class table.
        {
            sol::table iconIndexTable = lua.create_table();

            auto make_lua_key = [](const std::string &s) -> std::string {
                // "empty_checkbox" -> "Empty_Checkbox"
                std::string out;
                std::size_t i = 0;
                while (i < s.size()) {
                    std::size_t j = s.find('_', i);
                    if (j == std::string::npos) j = s.size();
                    std::string part = s.substr(i, j - i);
                    if (!part.empty()) {
                        part[0] = static_cast<char>(std::toupper(part[0]));
                    }
                    if (!out.empty()) out += "_";
                    out += part;
                    i = j + 1;
                }
                return out;
            };

            for (const auto &p : SDOM::icon_index_to_string) {
                int idx = p.first;
                const std::string &name = p.second; // e.g. "empty_checkbox"
                std::string luaKey = make_lua_key(name); // "Empty_Checkbox"
                iconIndexTable.set(luaKey, idx);
            }

            // Register global IconIndex table (if not present)
            sol::object existing = lua["IconIndex"];
            if (!existing.valid() || existing == sol::lua_nil) {
                lua["IconIndex"] = iconIndexTable;
            } else {
                // merge missing entries into existing table
                sol::table t = lua["IconIndex"];
                for (const auto &p : icon_index_to_string) {
                    std::string k = make_lua_key(p.second);
                    sol::object cur = t.raw_get<sol::object>(k);
                    if (!cur.valid() || cur == sol::lua_nil) t.set(k, p.first);
                }
            }

            // Also expose constants on the IconButton class/table (typeName)
            sol::object maybeClass = lua[typeName.c_str()];
            sol::table classTable;
            if (!maybeClass.valid() || maybeClass == sol::lua_nil) {
                classTable = lua.create_table();
                lua[typeName.c_str()] = classTable;
            } else {
                classTable = maybeClass.as<sol::table>();
            }
            for (const auto &p : SDOM::icon_index_to_string) {
                std::string k = make_lua_key(p.second);
                sol::object cur2 = classTable.raw_get<sol::object>(k);
                if (!cur2.valid() || cur2 == sol::lua_nil) classTable.set(k, p.first);
            }
        }

        // { static_cast<int>(IconIndex::Hamburger), "hamburger" },
        // { static_cast<int>(IconIndex::Left_Arrow_Raised), "left_arrow_raised" },
        // { static_cast<int>(IconIndex::Right_Arrow_Raised), "right_arrow_raised" },
        // { static_cast<int>(IconIndex::Up_Arrow_Raised), "up_arrow_raised" },
        // { static_cast<int>(IconIndex::Down_Arrow_Raised), "down_arrow_raised" },
        // { static_cast<int>(IconIndex::Left_Arrow_Depressed), "left_arrow_depressed" },
        // { static_cast<int>(IconIndex::Right_Arrow_Depressed), "right_arrow_depressed" },
        // { static_cast<int>(IconIndex::Up_Arrow_Depressed), "up_arrow_depressed" },
        // { static_cast<int>(IconIndex::Down_Arrow_Depressed), "down_arrow_depressed" },
        // { static_cast<int>(IconIndex::Horizontal_Knob), "horizontal_knob" },
        // { static_cast<int>(IconIndex::Vertical_Knob), "vertical_knob" },
        // { static_cast<int>(IconIndex::Slider_Tick), "slider_tick" },
        // { static_cast<int>(IconIndex::HSlider_Rail), "hslider_rail" },
        // { static_cast<int>(IconIndex::VSlider_Rail), "vslider_rail" },
        // { static_cast<int>(IconIndex::Empty_Checkbox), "empty_checkbox" },
        // { static_cast<int>(IconIndex::Checked_Checkbox), "checked_checkbox" },
        // { static_cast<int>(IconIndex::X_Checkbox), "x_checkbox" },
        // { static_cast<int>(IconIndex::Unselected_Radio), "unselected_radio" },
        // { static_cast<int>(IconIndex::Selected_Radio), "selected_radio" },
        // { static_cast<int>(IconIndex::Left_HProgress), "left_hprogress" },
        // { static_cast<int>(IconIndex::Empty_HProgress), "empty_hprogress" },
        // { static_cast<int>(IconIndex::Thumb_HProgress), "thumb_hprogress" },
        // { static_cast<int>(IconIndex::Right_HProgress), "right_hprogress" },
        // { static_cast<int>(IconIndex::Top_VProgress), "top_vprogress" },
        // { static_cast<int>(IconIndex::Empty_VProgress), "empty_vprogress" },
        // { static_cast<int>(IconIndex::Thumb_VProgress), "thumb_vprogress" },
        // { static_cast<int>(IconIndex::Bottom_VProgress), "bottom_vprogress" }



    } // END: void IconButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua)



} // END: namespace SDOM