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
        void add_test(const std::string& name, std::function<bool(std::vector<std::string>&)> func, bool is_implemented = true);
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


        void setLuaFilename(const std::string& filename) { lua_filename_ = filename; }  
        std::string getLuaFilename() const { return lua_filename_; }

    private:
        UnitTests() = default;

        std::string lua_filename_ = "";

        struct TestCase {
            std::string name;
            std::function<bool(std::vector<std::string>&)> func;
            bool is_implemented = false;
        };
        std::vector<std::string> _log;
        std::vector<std::string> _errors;
        std::vector<TestCase> _tests;

        std::string objName_; // temporary storage for object name during test runs


        // --- Lua Registration --- //
        void registerLua();
    };

} // namespace SDOM


/************* 
NOTES:

// --- 🧩 Updated Test Model --- //
struct TestCase {
    std::string name;
    std::function<bool(std::vector<std::string>&)> func;
    bool is_implemented = true;
    bool has_run = false;
    bool passed = false;
    bool running = false;
    int frame_count = 0;
    std::vector<std::string> errors;
};

// --- 🧠 New Public Interface --- //
void start_all(const std::string& objName);
void update(); // Called once per frame
bool all_done() const;

// --- usage: --- //
UnitTests& ut = UnitTests::getInstance();
ut.start_all("Event"); // queue all tests
// inside main loop:
ut.update();
if (ut.all_done()) stopAfterUnitTests();

// --- ⚙️ Refactored Execution Logic --- //
// SDOM_UnitTests.cpp (conceptual implementation)
#include <iostream>
#include <iomanip>

namespace SDOM {

UnitTests& UnitTests::getInstance() {
    static UnitTests instance;
    return instance;
}

void UnitTests::clear_tests() {
    _tests.clear();
    _log.clear();
    _errors.clear();
}

void UnitTests::add_test(const std::string& name, std::function<bool(std::vector<std::string>&)> func, bool is_implemented) {
    _tests.push_back({name, func, is_implemented});
}

bool UnitTests::run_all(const std::string& objName) {
    // legacy immediate mode – still works for synchronous test harnesses
    objName_ = objName;
    bool all_passed = true;
    std::cout << "\n=== Running " << objName << " Unit Tests (Legacy Mode) ===\n";
    for (auto& test : _tests) {
        if (!test.is_implemented) continue;
        test.errors.clear();
        bool ok = test.func(test.errors);
        if (ok)
            std::cout << "✅ " << test.name << std::endl;
        else {
            all_passed = false;
            std::cout << "❌ " << test.name << std::endl;
            for (auto& e : test.errors)
                std::cout << "   ↳ " << e << std::endl;
        }
    }
    return all_passed;
}

// --- Frame-by-Frame Execution --- //
void UnitTests::start_all(const std::string& objName) {
    objName_ = objName;
    for (auto& test : _tests) {
        test.has_run = false;
        test.running = false;
        test.passed = false;
        test.frame_count = 0;
        test.errors.clear();
    }
    std::cout << "\n=== Starting " << objName << " Unit Tests (Frame Mode) ===\n";
}

// This should be called every frame
void UnitTests::update() {
    if (_tests.empty()) return;

    // find next unfinished test
    TestCase* current = nullptr;
    for (auto& t : _tests) {
        if (!t.has_run && t.is_implemented) {
            current = &t;
            break;
        }
    }
    if (!current) return; // all tests done

    // If just started
    if (!current->running) {
        current->running = true;
        std::cout << "\n▶ Running Test: " << current->name << std::endl;
    }

    current->frame_count++;

    // Execute once (you could extend later to async multi-frame logic)
    bool ok = current->func(current->errors);
    current->passed = ok;
    current->has_run = true;
    current->running = false;

    if (ok) {
        std::cout << "✅ PASSED: " << current->name << " (" << current->frame_count << " frame)\n";
    } else {
        std::cout << "❌ FAILED: " << current->name << " (" << current->frame_count << " frame)\n";
        for (auto& e : current->errors)
            std::cout << "   ↳ " << e << std::endl;
    }

    _errors.insert(_errors.end(), current->errors.begin(), current->errors.end());
}

bool UnitTests::all_done() const {
    for (auto& t : _tests)
        if (!t.has_run && t.is_implemented)
            return false;
    return true;
}


// --- 🪄 Example Integration in Core Loop --- //
void SDOM::Core::onUpdate()
{
    UnitTests& ut = UnitTests::getInstance();
    if (!ut.all_done()) {
        ut.update();
    } else {
        getFactory().report_performance_stats();
        stopAfterUnitTests();
    }
}




**************/