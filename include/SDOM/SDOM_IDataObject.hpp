/***  SDOM_IDataObject.hpp  ****************************
 *  ___ ___   ___  __  __   ___ ___       _         ___  _     _        _     _              
 * / __|   \ / _ \|  \/  | |_ _|   \ __ _| |_ __ _ / _ \| |__ (_)___ __| |_  | |_  _ __ _ __ 
 * \__ \ |) | (_) | |\/| |  | || |) / _` |  _/ _` | (_) | '_ \| / -_) _|  _|_| ' \| '_ \ '_ \
 * |___/___/ \___/|_|  |_|_|___|___/\__,_|\__\__,_|\___/|_.__// \___\__|\__(_)_||_| .__/ .__/
 *                      |___|                               |__/                  |_|  |_|   
 *  
 * The SDOM_IDataObject class defines the core interface for all data-driven objects within 
 * the SDOM framework. It provides a flexible property and command registration system, 
 * enabling dynamic serialization, deserialization, and runtime manipulation of object state 
 * through JSON. By supporting property getters, setters, and commands, IDataObject allows for 
 * introspection, scripting, and editor integration, making it easy to extend and interact with 
 * objects in a generic way. This interface forms the foundation for resource and display object 
 * management in SDOM, facilitating robust data binding, configuration, and automation across 
 * the entire framework.
 * 
 *   This software is provided 'as-is', without any express or implied
 *   warranty.  In no event will the authors be held liable for any damages
 *   arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *   including commercial applications, and to alter it and redistribute it
 *   freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not
 *       claim that you wrote the original software. If you use this software
 *       in a product, an acknowledgment in the product documentation would be
 *       appreciated but is not required.
 *   2. Altered source versions must be plainly marked as such, and must not be
 *       misrepresented as being the original software.
 *   3. This notice may not be removed or altered from any source distribution.
 *
 * Released under the ZLIB License.
 * Original Author: Jay Faries (warte67)
 *
 ******************/

#ifndef __SDOM_IDataObject_HPP__
#define __SDOM_IDataObject_HPP__

#include <SDOM/SDOM.hpp>
#include <SDOM/json.hpp> // Include nlohmann/json.hpp
#include <SDOM/SDOM_IUnitTest.hpp>

namespace SDOM
{

    using Json = nlohmann::json;

    /**
     * @class IDataObject
     * @brief Core interface for all data-driven objects in SDOM.
     * @details
     * Provides property and command registration, dynamic serialization/deserialization,
     * and runtime manipulation of object state via JSON. Enables introspection,
     * scripting, and editor integration for resource and display object management.
     * Forms the foundation for robust data binding, configuration, and automation.
     *
     * Example usage:
     * @code
     * // Register a property
     * registerProperty("x",
     *     [](const IDataObject& obj) { return obj.x_; },
     *     [](IDataObject& obj, const Json& val) { obj.x_ = val.get<int>(); });
     *
     * // Register a command
     * registerCommand("reset", [](IDataObject& o, const Json&) { /-- reset logic --/ });
     * @endcode
     *
     * @author Jay Faries (warte67)
     * @copyright ZLIB License
     */
    class IDataObject : public SDOM::IUnitTest
    {
    public:
        using Getter = std::function<Json(const IDataObject&)>;
        using Setter = std::function<IDataObject&(IDataObject&, const Json&)>;
        using Command = std::function<void(IDataObject&, const Json&)>;

        /**
         * @name Virtual Methods
         * @brief All virtual methods for this class.
         * @{
         */

        /**
         * @brief Called during object initialization.
         * @return True if initialization succeeds, false otherwise.
         */
        virtual bool onInit() = 0;

        /**
         * @brief Called during object shutdown.
         */
        virtual void onQuit() = 0;
        
        /**
         * @brief Runs unit tests for this data object.
         * @details
         * Called during startup or explicit unit test runs. Each object should validate its own state and behavior.
         * @return true if all tests pass, false otherwise.
         */
        virtual bool onUnitTest() { return true; }
        /** @} */


        /**
         * @name Property System
         * @{
         */

        /**
         * @brief Populates object state from a JSON object.
         * @param json JSON object containing state.
         */
        void fromJson(const Json& json);

        /**
         * @brief Serializes object state to a JSON object.
         * @return JSON object representing current state.
         */
        Json toJson() const;

        /**
         * @brief Registers a property with getter and optional setter.
         * @param name Property name.
         * @param getter Function to get property value.
         * @param setter Function to set property value (optional).
         */
        void registerProperty(const std::string& name, Getter getter, Setter setter = nullptr);

        /**
         * @brief Gets the value of a property.
         * @param name Property name.
         * @return Property value as JSON.
         */
        Json getProperty(const std::string& name) const;

        /**
         * @brief Sets the value of a property.
         * @param name Property name.
         * @param value New value as JSON.
         * @return Reference to this object (for chaining).
         */
        IDataObject& setProperty(const std::string& name, const Json& value);
        /** @} */

        /**
         * @name Command System
         * @{
         */

        /**
         * @brief Registers a command.
         * @param name Command name.
         * @param cmd Command function.
         */
        void registerCommand(const std::string& name, Command cmd);

        /**
         * @brief Executes a command.
         * @param name Command name.
         * @param args Arguments as JSON (optional).
         */
        void command(const std::string& name, const Json& args = Json{});

        /**
         * @brief Gets all registered commands.
         * @return Map of command names to command functions.
         */
        const std::unordered_map<std::string, Command>& getCommands() const;
        /** @} */

        /**
         * @name JSON Helpers
         * @{
         */

        /**
         * @brief Gets a value from a JSON object using case-insensitive key lookup.
         * @tparam T Type of value to return.
         * @param j JSON object.
         * @param key Key to search for.
         * @param default_value Value to return if key is not found.
         * @return Value from JSON or default.
         */
        template<typename T>
        static T json_value_case_insensitive(const nlohmann::json& j, const std::string& key, const T& default_value) 
        {
            auto it = std::find_if(j.begin(), j.end(), [&](const auto& item) 
            {
                const std::string& k = item.first;
                return std::equal(k.begin(), k.end(), key.begin(), key.end(),
                    [](char a, char b) { return std::tolower(a) == std::tolower(b); });
            });
            if (it != j.end()) 
            {
                return it.value();
            }
            return default_value;
        }
        /** @} */

    private:
        std::unordered_map<std::string, Getter> getters_;
        std::unordered_map<std::string, Setter> setters_;
        std::unordered_map<std::string, Command> commands_;

        // // Example property
        // int x_ = 0;
    };

} // namespace SDOM

#endif // __SDOM_IDataObject_HPP__