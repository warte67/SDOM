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


    bool ArrowButton::onUnitTest(int frame)
    {
        // Run base class tests first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Only register once
        static bool registered = false;
        if (!registered)
        {
            // Validate direction enum
            ut.add_test(objName, "ArrowDirection Validity", [this](std::vector<std::string>& errors)
            {
                switch (direction_)
                {
                    case ArrowDirection::Up:
                    case ArrowDirection::Down:
                    case ArrowDirection::Left:
                    case ArrowDirection::Right:
                        break;
                    default:
                        errors.push_back(
                            "[ArrowButton] '" + getName() + 
                            "' has invalid direction: " + 
                            std::to_string(static_cast<int>(direction_))
                        );
                        break;
                }
                return true; // ✅ finished this frame
            });

            // Validate sprite sheet dimensions (if applicable)
            ut.add_test(objName, "SpriteSheet Dimensions", [this](std::vector<std::string>& errors)
            {
                SpriteSheet* ss = getSpriteSheetPtr();
                if (!ss)
                    return true; // ✅ nothing to validate if no spritesheet

                if (ss->getSpriteWidth() <= 0 || ss->getSpriteHeight() <= 0)
                {
                    errors.push_back(
                        "[ArrowButton] '" + getName() +
                        "' has invalid sprite size: w=" +
                        std::to_string(ss->getSpriteWidth()) +
                        " h=" + std::to_string(ss->getSpriteHeight())
                    );
                }
                return true; // ✅ finished this frame
            });

            registered = true;
        }

        // ✅ Return false to indicate that this test group remains active until complete
        return false;
    } // END: ArrowButton::onUnitTest()




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

        // // Augment the single shared DisplayHandle handle usertype
        // sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);

        // // Helper to check if a property/command is already registered
        // auto absent = [&](const char* name) -> bool 
        // {
        //     sol::object cur = handle.raw_get_or(name, sol::lua_nil);
        //     return !cur.valid() || cur == sol::lua_nil;
        // };


    } // END: void IconButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM

