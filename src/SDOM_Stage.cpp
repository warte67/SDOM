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

    void Stage::_registerLua(const std::string& typeName, sol::state_view lua)
    {
        // if (DEBUG_REGISTER_LUA)
        // {
        //     std::string typeNameLocal = "Stage";
        //     std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
        //                 << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName << CLR::RESET << std::endl;
        // }
        // 1. Create and save usertype table (no constructor)
        sol::usertype<Stage> objHandleType = lua.new_usertype<Stage>(typeName,
            sol::base_classes, sol::bases<SUPER>() );
        this->objHandleType_ = objHandleType;

        // 2. Call base class registration to include inherited properties/commands
        SUPER::_registerLua(typeName, lua);

        // 3. Register properties/commands (custom logic)
        // register simple mouse coordinate accessors
        getFactory().registerLuaProperty(typeName, "mouseX",
                [](const IDataObject& obj, sol::state_view lua) -> sol::object {
                    return sol::make_object(lua, Stage::getMouseX());
                },
                [](IDataObject& obj, sol::object val, sol::state_view lua) -> IDataObject& {
                    if (val.is<int>()) Stage::setMouseX(val.as<int>());
                    else if (val.is<double>()) Stage::setMouseX(static_cast<int>(val.as<double>()));
                    return obj;
                }
            );

        getFactory().registerLuaProperty(typeName, "mouseY",
                [](const IDataObject& obj, sol::state_view lua) -> sol::object {
                    return sol::make_object(lua, Stage::getMouseY());
                },
                [](IDataObject& obj, sol::object val, sol::state_view lua) -> IDataObject& {
                    if (val.is<int>()) Stage::setMouseY(val.as<int>());
                    else if (val.is<double>()) Stage::setMouseY(static_cast<int>(val.as<double>()));
                    return obj;
                }
            );

        // 4. Register the Lua usertype using the registry
        getFactory().registerLuaPropertiesAndCommands(typeName, objHandleType_);          
    }

    void Stage::_registerDisplayObject(const std::string& typeName, sol::state_view lua)
    {
        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Stage";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN 
                    << typeName << CLR::RESET << std::endl;
        }

        // Call base class registration to include inherited properties/commands
        SUPER::_registerDisplayObject(typeName, lua);

        // // Create and save usertype table 
        // SDOM::Factory& factory = SDOM::getFactory();

        sol::usertype<Stage> objHandleType = lua.new_usertype<Stage>(typeName, sol::base_classes, sol::bases<SUPER>()
            // ...Stage-specific methods...
        );

        // Store the usertype for later use
        this->objHandleType_ = objHandleType;

    } // End Box::_registerDisplayObject()

} // namespace SDOM