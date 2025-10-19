// SDOM_UnitTests.hpp

#pragma once

#include <SDOM/SDOM.hpp>

namespace SDOM
{
    class UnitTests
    {
    public:
        static void printTest(const std::string& objName, const std::string& testName) 
        {
            std::cout << CLR::indent() << CLR::LT_BLUE << "[" << objName << "] " << CLR::LT_BLUE << testName << " ..." << CLR::RESET;
            fflush(stdout); // Ensure immediate output
        }

        // static void printTest(const std::string& objName, const std::string& testName) 
        // {
        //     std::cout << CLR::indent() << CLR::LT_BLUE << "[" << objName << "] " << CLR::LT_BLUE << testName << " ";
        //     CLR::get_cursor_pos(UnitTests::_saved_row, UnitTests::_saved_col);
        //     std::cout << "..." << CLR::RESET;
        //     fflush(stdout); // Ensure immediate output
        // }

        static void printResult(const std::string& testName, bool passed) 
        {
            std::cout << "\b\b\b\b"; // Erase the " ..." part
            std::cout << (passed ? CLR::GREEN + " [PASSED]" : CLR::fg_rgb(255, 0, 0) + " [FAILED]") << CLR::RESET << std::endl;
        }

        // static void printResult(const std::string& testName, bool passed) 
        // {
        //     CLR::save_cursor();
        //     CLR::set_cursor_pos(UnitTests::_saved_row, UnitTests::_saved_col);
        //     std::cout << (passed ? CLR::GREEN + " [PASSED]" : CLR::fg_rgb(255, 0, 0) + " [FAILED]") << CLR::RESET << std::endl;
        //     CLR::restore_cursor();
        // }

        // A helper to run and print a test
        template<typename Func>
        static bool run(const std::string& objName, const std::string& testName, Func&& testFunc) 
        {
            if (UNIT_TESTS_ENABLED)   printTest(objName, testName);
            bool result = testFunc();
            if (UNIT_TESTS_ENABLED)   printResult(testName, result);
            return result;
        }

    private:
        static int _saved_row;
        static int _saved_col;
    };
} // namespace SDOM