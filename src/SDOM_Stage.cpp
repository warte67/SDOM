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
    }

    void Stage::onEvent(const Event& event)
    {
        // // Stage event handling logic
        // std::cout << "Stage::onEvent() called for Stage: " << getName()
        //           << " with event type: " << event.getTypeName() << std::endl;
    }

    void Stage::onRender()
    {
        // Stage rendering logic

        // std::cout << "Stage::onRender() called for Stage: " << getName() << std::endl;


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


    void Stage::_registerDisplayObject(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerDisplayObject(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Stage";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN 
                    << typeName << CLR::RESET << std::endl;
        }

        // Create the Stage usertype and bind stage-specific properties directly
        sol::usertype<Stage> objHandleType = lua.new_usertype<Stage>(typeName,
            sol::no_constructor, sol::base_classes, sol::bases<SUPER>());

        // Expose mouseX/mouseY as properties backed by the static Stage getters/setters
        objHandleType["mouseX"] = sol::property(
            [](const Stage& /*s*/) { return Stage::getMouseX(); },
            [](Stage& /*s*/, sol::object val) {
                if (val.is<int>()) Stage::setMouseX(val.as<int>());
                else if (val.is<double>()) Stage::setMouseX(static_cast<int>(val.as<double>()));
            }
        );

        objHandleType["mouseY"] = sol::property(
            [](const Stage& /*s*/) { return Stage::getMouseY(); },
            [](Stage& /*s*/, sol::object val) {
                if (val.is<int>()) Stage::setMouseY(val.as<int>());
                else if (val.is<double>()) Stage::setMouseY(static_cast<int>(val.as<double>()));
            }
        );

        // Store the usertype for later use
        this->objHandleType_ = objHandleType;

    } // End Stage::_registerDisplayObject()

} // namespace SDOM