// SDOM_Stage.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_Factory.hpp>

namespace SDOM
{

    Stage::Stage(const InitStruct& init) : IDisplayObject(init)
    {
        if (init.type != TypeName) {
            ERROR("Error: Stage constructed with incorrect type: " + init.type);
        }        
        setClickable(true);
    }

    Stage::Stage(const sol::table& config) : IDisplayObject(config)
    {
        std::string type = config["type"].valid() ? config["type"].get<std::string>() : TypeName;
        if (type != TypeName) {
            ERROR("Error: Stage constructed with incorrect type: " + type);
        }
        setClickable(true);
    }


    bool Stage::onInit()
    {
        // Stage initialization logic
        // std::cout << "Stage::onInit() called for Stage: " << getName() << std::endl;
        return true;
    }

    void Stage::onQuit()
    {
        // Stage shutdown logic
        // std::cout << "Stage::onQuit() called for Stage: " << getName() << std::endl;
    }

    void Stage::onUpdate(float fElapsedTime)
    {
        // Stage update logic
        // std::cout << "Stage::onUpdate() called for Stage: " << getName()
        //           << " with elapsed time: " << fElapsedTime << " seconds." << std::endl;
        SUPER::onUpdate(fElapsedTime); // Call base class update handler
    }

    void Stage::onEvent(const Event& event)
    {
        // // Stage event handling logic
        // std::cout << "Stage::onEvent() called for Stage: " << getName()
        //           << " with event type: " << event.getTypeName() << std::endl;

        SUPER::onEvent(event); // Call base class event handler
    }

    void Stage::onRender()
    {
        // Stage rendering logic
        SDL_Renderer* renderer = Core::getInstance().getRenderer();
        if (!renderer)
            ERROR("Core::onRender() Error: Renderer is null.");

        // Clear the entire window to the border color
        SDL_Color color = getColor();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
    }

    bool Stage::onUnitTest()
    {
        // Stage unit testing logic

        // std::cout << CLR::CYAN << "Stage::onUnitTest() called for Stage: " << getName() << std::endl;

        return true;
    }


    void Stage::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Stage";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN 
                    << typeName << CLR::RESET << std::endl;
        }

        // Augment the single shared DisplayHandle handle usertype
        sol::table handle = DisplayHandle::ensure_handle_table(lua);

        auto absent = [&](const char* name) -> bool {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        // Expose mouseX/mouseY as properties backed by static Stage getters/setters
        if (absent("mouseX")) {
            // Use member function pointers to avoid complex template deduction
            handle["mouseX"] = sol::property(static_cast<int (Stage::*)() const>(&Stage::getMouseX_lua), static_cast<void (Stage::*)(int)>(&Stage::setMouseX_lua));
        }

        if (absent("mouseY")) {
            handle["mouseY"] = sol::property(static_cast<int (Stage::*)() const>(&Stage::getMouseY_lua), static_cast<void (Stage::*)(int)>(&Stage::setMouseY_lua));
        }
    } // End Stage::_registerDisplayHandle()

} // namespace SDOM