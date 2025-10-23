// SDOM_UnitTests.hpp

#pragma once

#include <SDOM/SDOM.hpp>
#include <string>
#include <vector>

// NOTE: This should be a singleton class with only static members

namespace SDOM
{
    class Core;
    class Factory;
    class EventManager;

    class UnitTests
    {
    public:
        inline static bool show_all_tests = true;

        static UnitTests& getInstance();

        // Disable copy/move
        UnitTests(const UnitTests&) = delete;
        UnitTests& operator=(const UnitTests&) = delete;

        // Instance methods
        void clear_tests();
        void add_test(const std::string& name, std::function<bool(std::vector<std::string>&)> func);
        bool run_all(const std::string& objName);
        void push_error(const std::string& error);
        bool run_lua_tests(std::vector<std::string>& errors, const std::string& filename);

        // Deprecated static methods (to be removed)
        template<typename Func>        
        static bool run(const std::string& objName, const std::string& testName, Func&& testFunc) 
        {
            std::cerr << "[UnitTests::run] DEPRECATED: Refactor to use add_test and run_all. Test '" 
                      << testName << "' (" << objName << ") was not executed." << std::endl;
            return false;
        }

        // --- Core UnitTest Accessors / Mutators --- //
        SDL_Window* getWindow();
        SDL_Renderer* getRenderer();
        SDL_Texture* getTexture();
        SDL_Color getColor();

        // --- Factory & EventManager Access --- //
        const Factory& getFactory() const;
        EventManager& getEventManager() const;
        bool getIsTraversing() const;
        void setIsTraversing(bool traversing);      


    private:
        UnitTests() = default;

        struct TestCase {
            std::string name;
            std::function<bool(std::vector<std::string>&)> func;
        };
        std::vector<std::string> _log;
        std::vector<std::string> _errors;
        std::vector<TestCase> _tests;


        // --- Lua Registration --- //
        void registerLua();
    };

} // namespace SDOM