// SDOM_UnitTests.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_EventManager.hpp>

#include <SDOM/SDOM_UnitTests.hpp>  

namespace SDOM
{

    UnitTests& UnitTests::getInstance() 
    {
        static UnitTests instance;
        static bool lua_registered = false;
        if (!lua_registered) {
            instance.registerLua();
            lua_registered = true;
        }
        return instance;
    }

    void UnitTests::clear_tests() 
    {
        _tests.clear();
        _log.clear();
        _errors.clear();
    }    

    void UnitTests::add_test(const std::string& name, std::function<bool(std::vector<std::string>&)> func) 
    {
        _tests.push_back({name, func});
    }

    bool UnitTests::run_all(const std::string& objName) 
    {
        std::vector<std::string> output;
        bool allPassed = true;
        for (const auto& test : _tests) {
            std::vector<std::string> errors;
            std::ostringstream oss;
            bool passed = test.func(errors);
            oss << CLR::indent() << CLR::NORMAL << "[" << objName << "] " << CLR::LT_BLUE << test.name << CLR::RESET;
            oss << (passed ? CLR::GREEN + " [PASSED]" : CLR::fg_rgb(255, 0, 0) + " [FAILED]") << CLR::RESET << std::endl;
            if (!passed && !errors.empty()) {
                for (const auto& line : errors)
                    oss << CLR::indent() << CLR::fg_rgb(192, 64, 64) << "    Error: " << line << std::endl;
            }
            output.push_back(oss.str());
            allPassed &= passed;
        }
        if (allPassed) {
            if (show_all_tests) {
                std::cout << CLR::indent() << CLR::LT_BLUE << "Starting " << CLR::NORMAL << "[" << objName << "]" << CLR::LT_BLUE << " Unit Tests..." << CLR::RESET << std::endl;
                for (const auto& line : output) std::cout << line;
            }
            std::cout << CLR::indent() << CLR::NORMAL << "[" << objName << "]" << CLR::LT_BLUE << " Tests" << CLR::GREEN + " [PASSED]" << CLR::RESET << std::endl;
        } else {
            std::cout << CLR::indent() << CLR::NORMAL << "[" << objName << "]" << CLR::fg_rgb(255, 0, 0) << " UnitTest FAILED. Dumping results:" << CLR::RESET << std::endl;
            for (const auto& line : output) std::cout << line;
            std::exit(1);
        }
        return allPassed;
    }

    void UnitTests::push_error(const std::string& error) 
    {
        _errors.push_back(error);
    }


    // --- Core UnitTest Accessors / Mutators --- //

    SDL_Window* UnitTests::getWindow() { return getCore().window_; }
    SDL_Renderer* UnitTests::getRenderer() { return getCore().renderer_; }
    SDL_Texture* UnitTests::getTexture() { return getCore().texture_; }
    SDL_Color UnitTests::getColor() { return getCore().getColor(); }

    // --- Factory & EventManager Access --- //
    const Factory& UnitTests::getFactory() const { return *getCore().factory_; }
    EventManager& UnitTests::getEventManager() const { return *getCore().eventManager_; }
    bool UnitTests::getIsTraversing() const { return getCore().isTraversing_; }
    void UnitTests::setIsTraversing(bool traversing) { getCore().isTraversing_ = traversing; }   
    // --- Lua Registration --- //

    void UnitTests::registerLua()
    {
        /* Example LUA usage: 
            local ut = getUnitTests()
            ut:push_error("Something went wrong")
        */
        Core& core = getCore();
        sol::state& lua = core.getLua();
        if (!lua.lua_state()) {
            ERROR("Cannot register UnitTests Lua bindings: Lua state is not initialized.");
            return;
        }

        lua.new_usertype<UnitTests>("UnitTests"
            , "clear_tests", &UnitTests::clear_tests
            , "add_test", &UnitTests::add_test
            , "run_all", &UnitTests::run_all
            , "push_error", &UnitTests::push_error
        );
        lua.set_function("getUnitTests", &UnitTests::getInstance);

    } // END: registerLua()

} // END: namespace SDOM