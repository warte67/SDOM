// SDOM_Frame.cpp

#include <SDOM/SDOM.hpp>

#include <SDOM/SDOM_Frame.hpp>



namespace SDOM
{

    Frame::Frame(const InitStruct& init) : IPanelObject(init)
    {
        // std::cout << "Box constructed with InitStruct: " << getName() 
        //           << " at address: " << this << std::endl;

        if (init.type != TypeName) {
            ERROR("Error: Frame constructed with incorrect type: " + init.type);
        }
    }


    Frame::Frame(const sol::table& config) : IPanelObject(config, Frame::InitStruct())
    {
        // std::cout << "Box constructed with Lua config: " << getName() 
        //         << " at address: " << this << std::endl;            

        std::string type = config["type"].valid() ? config["type"].get<std::string>() : "";

        // INFO("Frame::Frame(const sol::table& config) -- name: " << getName() 
        //         << " type: " << type << " typeName: " << TypeName << std::endl 
        // ); // END INFO()

        if (type != TypeName) {
            ERROR("Error: Frame constructed with incorrect type: " + type);
        }
    }


    // --- Virtual Methods --- //
    bool Frame::onInit()
    {
        return SUPER::onInit();
    } // END: bool Frame::onInit()


    void Frame::onQuit()
    {
        SUPER::onQuit();
    } // END: void Frame::onQuit()


    void Frame::onRender()
    {
        SUPER::onRender();
    } // END: void Frame::onRender()


    void Frame::onUpdate(float fElapsedTime)
    {
        (void) fElapsedTime; // suppress unused parameter warning

    } // END: void Frame::onUpdate(float fElapsedTime)


    void Frame::onEvent(const Event& event)
    {
        (void)event;    // suppress unused parameter warning

    } // END: void Frame::onEvent(const Event& event)
    

    bool Frame::onUnitTest(int frame)
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
            // 🔹 Frame should have a positive width and height
            ut.add_test(objName, "Frame Dimensions Validity", [this](std::vector<std::string>& errors)
            {
                if (getWidth() <= 0 || getHeight() <= 0)
                {
                    errors.push_back(
                        "Frame '" + getName() + "' has invalid size: w=" +
                        std::to_string(getWidth()) + " h=" + std::to_string(getHeight()));
                }
                return true; // ✅ test completes in a single frame
            });

            registered = true;
        }

        // ✅ Return false to remain active for multi-frame consistency
        return false;
    } // END: Frame::onUnitTest()



    // --- Lua Registration --- //
    void Frame::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Frame";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // Go-by for future bindings on DisplayHandle:
        //   sol::table handle = SDOM::IDataObject::ensure_sol_table(lua, SDOM::DisplayHandle::LuaHandleName);

        // // Helper to check if a property/command is already registered
        // auto absent = [&](const char* name) -> bool {
        //     sol::object cur = handle.raw_get_or(name, sol::lua_nil);
        //     return !cur.valid() || cur == sol::lua_nil;
        // };

        // --- no additional members from Frame (so far) --- //

    } // END: void Frame::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM
