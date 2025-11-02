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

        struct TestCase {
            std::string obj_name;
            std::string name;
            std::function<bool(std::vector<std::string>&)> func;
            bool is_implemented = false;
            bool passed = false;
            bool has_run = false;
            bool running = false;
            int frame_count = 0;
            std::vector<std::string> errors;   
            size_t last_error_count = 0; // for multi-frame test        
        };        

        static UnitTests& getInstance();

        // Disable copy/move
        UnitTests(const UnitTests&) = delete;
        UnitTests& operator=(const UnitTests&) = delete;

        // Instance methods
        void clear_tests();
        void add_test(const std::string& objName, const std::string& name, std::function<bool(std::vector<std::string>&)> func, bool is_implemented = true);
        bool run_single_test(TestCase& test, const std::string& objName);
        bool run_all(const std::string& objName);
        bool run_lua_tests(std::vector<std::string>& errors, const std::string& filename);

        // new member functions
        void update();          // Called once per frame
        bool all_done() const;  // All tests have completed



        // Deprecated static methods (to be removed)
        template<typename Func>        
        static bool run(const std::string& objName, const std::string& testName, Func&& testFunc) 
        {
            std::cerr << "[UnitTests::run] DEPRECATED: Refactor to use add_test and run_all. Test '" 
                      << testName << "' (" << objName << ") was not executed." << std::endl;
            return false;
        }

        // --- static test helpers --- //

        static bool run_event_behavior_test(
            const std::vector<std::pair<std::string, std::function<void(DisplayHandle)>>>& actions, 
            std::vector<std::string>& errors);

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

        void setLuaFilename(const std::string& filename) { lua_filename_ = filename; }  
        std::string getLuaFilename() const { return lua_filename_; }

        void set_frame_counter(int frame_counter) { _frame_counter = frame_counter; }
        int get_frame_counter() const { return _frame_counter; }

    private:
        UnitTests() = default;

        int _current_index = 0;
        int _frame_counter = 0;
        bool all_passed_ = true;
        int _tests_failed = 0;

        std::string lua_filename_ = "";

        std::vector<std::string> _log;
        std::vector<std::string> _errors;
        std::vector<TestCase> _tests;

        std::string objName_; // temporary storage for object name during test runs


        // --- Lua Registration --- //
        void registerLua();
    };

} // namespace SDOM
