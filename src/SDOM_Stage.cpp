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
        setTabEnabled(false);
    }

    Stage::Stage(const sol::table& config) : Stage(config, InitStruct())
    {
    }

    Stage::Stage(const sol::table& config, const InitStruct& defaults) : IDisplayObject(config, defaults)
    {
        std::string type = config["type"].valid() ? config["type"].get<std::string>() : TypeName;
        if (type != TypeName) {
            ERROR("Error: Stage constructed with incorrect type: " + type);
        }
        setClickable(true);
        setTabEnabled(false);
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

    bool Stage::onUnitTest(int frame)
    {
        // Run base checks first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Register tests only once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Validate stage dimensions
            ut.add_test(objName, "Stage Size Validation", [this](std::vector<std::string>& errors)
            {
                if (getWidth() <= 0 || getHeight() <= 0)
                {
                    errors.push_back("Stage '" + getName() +
                                    "' has invalid size: w=" +
                                    std::to_string(getWidth()) +
                                    " h=" + std::to_string(getHeight()));
                }
                return true; // ✅ single-frame
            });

            // 🔹 2. Validate clickability
            ut.add_test(objName, "Stage Clickability", [this](std::vector<std::string>& errors)
            {
                if (!isClickable())
                {
                    errors.push_back("Stage '" + getName() +
                                    "' should be clickable by default");
                }
                return true;
            });

            registered = true;
        }

        // ✅ Return false so Stage remains active for the global test cycle
        return false;
    } // END: Stage::onUnitTest()


    void Stage::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Let IDataObject / IDisplayObject wire up their pieces first.
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA) {
            std::string typeNameLocal = "Stage";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // Create (or reuse) the Stage usertype. Using sol::no_constructor keeps Lua
        // from instantiating raw Stage objects; the factory does that.
        if (!lua[typeName].valid()) {
            objHandleType_ = lua.new_usertype<Stage>(
                typeName,
                sol::no_constructor,
                sol::base_classes, sol::bases<IDisplayObject>()
            );
        } else {
            objHandleType_ = lua[typeName];
        }

        sol::table stageTable = lua[typeName];

        auto set_if_absent = [](sol::table& tbl, const char* name, auto&& fn) {
            sol::object current = tbl.raw_get_or(name, sol::lua_nil);
            if (!current.valid() || current == sol::lua_nil) {
                tbl.set_function(name, std::forward<decltype(fn)>(fn));
            }
        };

        set_if_absent(stageTable, "getMouseX", &Stage::getMouseX_lua);
        set_if_absent(stageTable, "getMouseY", &Stage::getMouseY_lua);
        set_if_absent(stageTable, "setMouseX", &Stage::setMouseX_lua);
        set_if_absent(stageTable, "setMouseY", &Stage::setMouseY_lua);
    } // END: Stage::_registerLuaBindings()

    void Stage::registerBindingsImpl(const std::string& typeName)
    {
        // Future: integrate Data Registry properties and functions.
        // BIND_LOG("[" << typeName << "] Registering `Stage` bindings");
        BIND_INFO(typeName, "Stage");
        // addFunction(typeName, "getName", [this]() { return this->getName(); });
        // addFunction(typeName, "setName", [this](const std::string& n){ this->setName(n); });
    }

} // namespace SDOM