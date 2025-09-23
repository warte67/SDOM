/***  SDOM_IDataObject.cpp  ****************************
 *   ___ ___   ___  __  __   ___ ___       _         ___  _     _        _                  
 *  / __|   \ / _ \|  \/  | |_ _|   \ __ _| |_ __ _ / _ \| |__ (_)___ __| |_   __ _ __ _ __ 
 *  \__ \ |) | (_) | |\/| |  | || |) / _` |  _/ _` | (_) | '_ \| / -_) _|  _|_/ _| '_ \ '_ \
 *  |___/___/ \___/|_|  |_|_|___|___/\__,_|\__\__,_|\___/|_.__// \___\__|\__(_)__| .__/ .__/
 *                       |___|                               |__/                |_|  |_|   
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

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_Factory.hpp>

namespace SDOM
{

    void IDataObject::fromJson(const Json& json)
    {
        // Set all registered properties from JSON
        for (const auto& [propName, setter] : setters_) 
        {
            if (json.contains(propName) && setter) 
            {
                setter(*this, json[propName]);
            }
        }

        // Recursively initialize children if present in JSON
        if (json.contains("children") && json["children"].is_array()) 
        {
            if (auto* displayObj = dynamic_cast<IDisplayObject*>(this)) 
            {
                Factory* factory = &Core::getInstance().getFactory();
                for (const auto& childJson : json["children"]) 
                {
                    std::string typeName = childJson.value("type", "IDisplayObject"); // Default to IDisplayObject
                    ResourceHandle childHandle = factory->create(typeName, childJson);
                    if (childHandle) 
                    {
                        auto childObj = dynamic_cast<IDisplayObject*>(childHandle.get());
                        if (childObj)
                            displayObj->addChild(childHandle);
                        else
                            std::cout << "Failed to cast child to IDisplayObject for type: " << typeName << std::endl;
                    }
                    else
                    {
                        std::cout << "Failed to create child of type: " << typeName << std::endl;
                    }
                }
            }
        }
    }

    Json IDataObject::toJson() const
    {
        Json json;

        // Serialize all registered properties
        for (const auto& [propName, getter] : getters_) 
        {
            if (getter) 
            {
                json[propName] = getter(*this);
            }
        }

        // Serialize children if this is an IDisplayObject
        if (const auto* displayObj = dynamic_cast<const IDisplayObject*>(this)) 
        {
            Json childrenJson = Json::array();
            for (const auto& child : displayObj->getChildren()) 
            {
                if (child) 
                {
                    childrenJson.push_back(child->toJson());
                }
            }
            json["children"] = childrenJson;
        }

        return json;
    }

    void IDataObject::registerProperty(const std::string& name, Getter getter, Setter setter)
    {
        getters_[name] = getter;
        setters_[name] = setter;
    }

    Json IDataObject::getProperty(const std::string& name) const
    {
        auto it = getters_.find(name);
        if (it != getters_.end() && it->second) 
        {
            return it->second(*this);
        }
        ERROR("Getter property '" + name + "' not found.");  // Throw an error if not found
        return Json(); // Return empty Json if not found
    }

    IDataObject& IDataObject::setProperty(const std::string& name, const Json& value)
    {
        auto it = setters_.find(name);
        if (it != setters_.end() && it->second) 
        {
            return it->second(*this, value);
        }
        ERROR("Setter property '" + name + "' not found.");  // Throw an error if not found
        return *this;
    }

    void IDataObject::registerCommand(const std::string& name, Command cmd)
    {
        commands_[name] = cmd;
    }

    void IDataObject::command(const std::string& name, const Json& args)
    {
        auto it = commands_.find(name);
        if (it != commands_.end() && it->second) 
        {
            it->second(*this, args);
        } 
        else 
        {
            ERROR("Command '" + name + "' not found.");  // Throw an error if not found
        }        
    }     


} // namespace SDOM