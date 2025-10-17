// SDOM_ArrowButton.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_ArrowButton.hpp>

namespace SDOM
{
    // --- Constructors --- //

    ArrowButton::ArrowButton(const InitStruct& init) : IconButton(init)
    {
        if (init.type != TypeName) {
            DEBUG_LOG("WARNING: ArrowButton constructed with incorrect type: " + init.type);
        }
        // Initialize ArrowButton-specific members
        direction_ = init.direction;
        
    } // END: ArrowButton::ArrowButton(const InitStruct& init)

    ArrowButton::ArrowButton(const sol::table& config) : IconButton(config, ArrowButton::InitStruct())
    {
        // Validate type
        std::string type = config.get_or("type", std::string(TypeName));
        if (type != TypeName) {
            DEBUG_LOG("WARNING: ArrowButton constructed with incorrect type: " + type);
        }
        
        // Initialize ArrowButton-specific members from Lua table
        if (config["direction"].valid()) {
            try {
                if (config["direction"].get_type() == sol::type::string) {
                    std::string dirStr = config["direction"].get<std::string>();
                    // normalize to lowercase
                    std::transform(dirStr.begin(), dirStr.end(), dirStr.begin(), ::tolower);
                    auto it = string_to_arrow_direction.find(dirStr);
                    direction_ = (it != string_to_arrow_direction.end()) ? it->second : ArrowDirection::Up;
                } else if (config["direction"].get_type() == sol::type::number) {
                    int dirInt = config["direction"].get<int>();
                    direction_ = (dirInt >= 0 && dirInt < 4) ? static_cast<ArrowDirection>(dirInt) : ArrowDirection::Up;
                }
            } catch (...) {
                direction_ = ArrowDirection::Up;
            }
        } else {
            direction_ = ArrowDirection::Up;
        }   
             
    } // END: ArrowButton::ArrowButton(const sol::table& config)



    // --- Virtual Methods --- //

    bool ArrowButton::onInit() 
    {
        return SUPER::onInit(); // Call (IconButton) base class init
    } // END: bool ArrowButton::onInit() 


    void ArrowButton::onQuit() 
    {
        SUPER::onQuit(); // Call (IconButton) base class quit
    } // END: void ArrowButton::onQuit() 


    void ArrowButton::onEvent(const Event& event) 
    {
        SUPER::onEvent(event); // Call (IconButton) base class event handler first

        if (event.getType() == EventType::MouseButtonDown)  { arrowState_ = ArrowState::Depressed; }
        if (event.getType() == EventType::MouseButtonUp)    { arrowState_ = ArrowState::Raised; }

    } // END: void ArrowButton::onEvent(const Event& event) 


    void ArrowButton::onUpdate(float fElapsedTime) 
    {
        icon_index_ = getIconIndex();   // Update icon index based on current arrow state
        SUPER::onUpdate(fElapsedTime);  // Call (IconButton) base class update

    } // END: void ArrowButton::onUpdate(float fElapsedTime) 


    void ArrowButton::onRender() 
    {
        SUPER::onRender(); // Call (IconButton) base class render
    } // END: void ArrowButton::onRender() 





    // --- Lua Registration --- //

    void ArrowButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // // Include IButtonObject bindings first
        // IButtonObject::registerLuaBindings(lua);  // IconButton calls this

        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = typeName;
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
        if (absent("direction"))
        {
            handle.set("direction", sol::property(
                [](SDOM::DisplayHandle h) { return static_cast<int>(h.as<ArrowButton>()->getDirection()); },
                [](SDOM::DisplayHandle h, int v) { h.as<ArrowButton>()->direction_ = static_cast<ArrowButton::ArrowDirection>(v); }
            ));
        }

        // Expose getDirection and setDirection as Lua methods
        if (absent("getDirection")) 
        {
            handle.set_function("getDirection", [](SDOM::DisplayHandle h) 
            {
                auto* ab = h.as<ArrowButton>();
                return ab ? static_cast<int>(ab->getDirection()) : 0;
            });
        }
        if (absent("setDirection")) 
        {
            handle.set_function("setDirection", [](SDOM::DisplayHandle h, int v) 
            {
                auto* ab = h.as<ArrowButton>();
                if (ab && v >= 0 && v < 4) ab->setDirection(static_cast<ArrowButton::ArrowDirection>(v));
            });
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

