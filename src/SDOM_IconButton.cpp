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

        // NOTE:
        // IconIndex and IconButton numeric constants are registered centrally
        // by `Core::_registerLuaBindings` so configuration scripts can
        // reference them before type-specific registration runs. Avoid
        // duplicating that global registration here to keep Lua state setup
        // deterministic. If per-type extension is required later, merge
        // into the global `IconIndex` table instead of overwriting it.

    } // END: void IconButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua)



} // END: namespace SDOM