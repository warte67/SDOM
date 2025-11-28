// SDOM_IUnitTest.hpp
#pragma once
// #include <SDOM/SDOM.hpp>

namespace SDOM
{

    /**
     * @interface IUnitTest
     * @brief Interface for unit testing objects in SDOM.
     * @details
     * Provides a standardized method for running unit tests on objects that implement this interface.
     * Classes implementing this interface should define the onUnitTest method to perform their specific tests.
     *
     * Example usage:
     * @code
     * class MyClass : public IUnitTest {
     * public:
     *     bool onUnitTest() override {
     *         // Implement test logic here
     *         return true; // Return true if tests pass, false otherwise
     *     }
     * };
     * @endcode
     *
     * @author Jay Faries (warte67)
     * @copyright ZLIB License
     * @date 2025-09-19
     */
    class IUnitTest
    {
    public:
        /**
         * @brief Runs unit tests for this object.
         * @return true if all tests pass, false otherwise.
         */
        virtual bool onUnitTest(int frame) = 0;

        virtual ~IUnitTest() = default;

        // Optionally add logging/display methods here
        // ...

    };

} // namespace SDOM
