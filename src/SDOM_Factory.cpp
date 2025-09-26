// SDOM_Factory.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Handle.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>

namespace SDOM
{

    Factory::Factory() : IDataObject()
    {
        // make sure the DomHandles can access this Factory
        DomHandle::factory_ = this;
        ResHandle::factory_ = this;

        // ===== Register built-in types =====

        // register the Stage
        registerType("Stage", TypeCreators{
            Stage::CreateFromLua, // Update to Lua-based creator
            Stage::CreateFromInitStruct
        });

        // Register other built-in types here as needed ...        
    }


    void Factory::registerType(const std::string& typeName, const TypeCreators& creators)
    {
        creators_[typeName] = creators;
    }

    IResourceObject* Factory::getResObj(const std::string& name)
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) 
        {
            return dynamic_cast<IResourceObject*>(it->second.get());
        }
        return nullptr;
    }
    IDisplayObject* Factory::getDomObj(const std::string& name)
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) 
        {
            return dynamic_cast<IDisplayObject*>(it->second.get());
        }
        return nullptr;
    }



    DomHandle Factory::getDomHandle(const std::string& name) 
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) 
        {
            // Use the resource's type for the DomHandle
            return DomHandle(name, it->second->getType());
        }
        // Return an empty DomHandle if not found
        return DomHandle();
    }

    DomHandle Factory::getStageHandle() 
    {
        Stage* stage = getStage();
        if (!stage) {
            ERROR("Factory::getStageHandle: No stage available");
            return DomHandle(); // Return an empty/null handle if stage is not available
        }
        std::string stageName = stage->getName();
        return getDomHandle(stageName);
    }

    // String (LUA text) based object creator
    DomHandle Factory::create(const std::string& typeName, const sol::table& config)
    {
        // Get main texture size for defaults
        int maxStageWidth = 0;
        int maxStageHeight = 0;
        SDL_Texture* texture = getTexture();
        if (texture) {
            maxStageWidth = texture->w;
            maxStageHeight = texture->h;
        }

        // Minimum required properties (x, y, width, height now optional)
        if (!config["name"].valid() || !config["type"].valid() || !config["color"].valid()) {
            std::cout << "Factory::create: Missing required property in Lua config.\n";
            return DomHandle();
        }

        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromLua) 
        {
            // Set defaults for x, y, width, height if not present
            int x = config["x"].valid() ? static_cast<int>(config["x"].get<double>()) : 0;
            int y = config["y"].valid() ? static_cast<int>(config["y"].get<double>()) : 0;
            int width = config["width"].valid() ? static_cast<int>(config["width"].get<double>()) : maxStageWidth - x;
            int height = config["height"].valid() ? static_cast<int>(config["height"].get<double>()) : maxStageHeight - y;

            // Optionally, you may want to inject these values into config for the display object constructor:
            sol::table configCopy = config;
            configCopy["x"] = x;
            configCopy["y"] = y;
            configCopy["width"] = width;
            configCopy["height"] = height;

            auto displayObject = it->second.fromLua(configCopy);
            if (!displayObject) {
                std::cout << "Factory::create: Failed to create display object of type '" << typeName
                        << "' from Lua. Display object is nullptr.\n";
                return DomHandle(); // Invalid handle
            }
            std::string name = config["name"];
            displayObjects_[name] = std::move(displayObject);
            displayObjects_[name]->onInit();

            // Optionally set additional properties if present
            if (config["anchorTop"].valid())    displayObjects_[name]->setAnchorTop(stringToAnchorPoint_.at(config["anchorTop"].get<std::string>()));
            if (config["anchorLeft"].valid())   displayObjects_[name]->setAnchorLeft(stringToAnchorPoint_.at(config["anchorLeft"].get<std::string>()));
            if (config["anchorBottom"].valid()) displayObjects_[name]->setAnchorBottom(stringToAnchorPoint_.at(config["anchorBottom"].get<std::string>()));
            if (config["anchorRight"].valid())  displayObjects_[name]->setAnchorRight(stringToAnchorPoint_.at(config["anchorRight"].get<std::string>()));
            if (config["z_order"].valid())      displayObjects_[name]->setZOrder(static_cast<int>(config["z_order"].get<double>()));
            if (config["priority"].valid())     displayObjects_[name]->setPriority(static_cast<int>(config["priority"].get<double>()));
            if (config["isClickable"].valid())  displayObjects_[name]->setClickable(config["isClickable"].get<bool>());
            if (config["isEnabled"].valid())    displayObjects_[name]->setEnabled(config["isEnabled"].get<bool>());
            if (config["isHidden"].valid())     displayObjects_[name]->setHidden(config["isHidden"].get<bool>());
            if (config["tabPriority"].valid())  displayObjects_[name]->setTabPriority(static_cast<int>(config["tabPriority"].get<double>()));
            if (config["tabEnabled"].valid())   displayObjects_[name]->setTabEnabled(config["tabEnabled"].get<bool>());

            // Set x, y, width, height
            displayObjects_[name]->setX(x);
            displayObjects_[name]->setY(y);
            displayObjects_[name]->setWidth(width);
            displayObjects_[name]->setHeight(height);

            return DomHandle(name, typeName);
        }
        return DomHandle(); // Invalid handle
    }

    // InitStruct based object creator
    DomHandle Factory::create(const std::string& typeName, const IDisplayObject::InitStruct& init)
    {
        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromInitStruct) 
        {
            auto displayObject = it->second.fromInitStruct(init);
            if (displayObject) 
            {
                std::string name = init.name;
                displayObject->onInit(); // Initialize the display object
                displayObjects_[name] = std::move(displayObject);
                return DomHandle(name, typeName);
            }
        }
        return DomHandle(); // Invalid handle
    }


    void Factory::addDisplayObject(const std::string& name, 
        std::unique_ptr<IDisplayObject> displayObject) 
    {
        displayObjects_[name] = std::move(displayObject);
    }
    
    void Factory::removeDisplayObject(const std::string& name) 
    {
        displayObjects_.erase(name);
    }

    std::vector<std::string> Factory::listDisplayObjectNames() const
    {
        std::vector<std::string> names;
        for (const auto& pair : displayObjects_) {
            names.push_back(pair.first);
        }
        return names;
    }

    void Factory::clear()
    {
        displayObjects_.clear();
    }

    void Factory::printRegistry() const
    {
        std::cout << "Factory Display Object Registry:\n";
        for (const auto& pair : displayObjects_) {
            const auto& name = pair.first;
            const auto& displayObject = pair.second;
            std::cout << "  Name: " << name
                    << ", Type: " << (displayObject ? displayObject->getType() : "Unknown")
                    << "\n";
        }
        std::cout << "Total display objects: " << displayObjects_.size() << std::endl;
    }

    

    // ----- Orphan and Future Child Management -----

    void Factory::detachOrphans()
    {
        for (auto& orphanHandle : orphanList_)
        {
            DomHandle orphan = orphanHandle;
            if (orphan)
            {
                DomHandle parent = orphan->getParent();
                if (parent)
                {
                    // Remove orphan from parent's children using public removeChild method
                    parent->removeChild(orphanHandle);
                    // Set orphan's parent to nullptr using public setParent method
                    orphan->setParent(DomHandle());
                }
            }
        }
        orphanList_.clear();
    }

    void Factory::attachFutureChildren() 
    {
        for (auto& futureChild : futureChildrenList_) 
        {
            DomHandle child = futureChild.child;
            if (child)
            {
                child->attachChild_(
                    futureChild.child,
                    futureChild.parent,
                    futureChild.preserveWorldPosition,
                    futureChild.dragStartWorldX,
                    futureChild.dragStartWorldY
                );
            }
        }
        futureChildrenList_.clear();
    }

    void Factory::addToOrphanList(const DomHandle orphan) 
    {
        if (orphan) 
        {
            orphanList_.push_back(orphan);
        }
    }

    void Factory::addToFutureChildrenList(const DomHandle child, const DomHandle parent, bool useWorld, int worldX, int worldY) 
    {
        if (child && parent) 
        {

            // std::cout << "child added to queue: " << child->getName() << " to (" << 
            //     worldX << ", " << worldY << ", w:" << child->getWidth() 
            //     << ", h:" << child->getHeight() << ")" << std::endl; 

            futureChildrenList_.push_back({child, parent, useWorld, worldX, worldY});
        }
    }

    void Factory::initFromLua(const sol::table& lua)
    {
        // Handle array of resources
        if (lua["resources"].valid() && lua["resources"].is<sol::table>())
        {
            sol::table resources = lua["resources"];
            for (auto& kv : resources)
            {
                sol::table resource = kv.second.as<sol::table>();
                processResource(resource);
            }
        }

        // Handle single resource object
        if (lua["resource"].valid() && lua["resource"].is<sol::table>())
        {
            processResource(lua["resource"]);
        }

        // Debug: List installed display objects
        auto names = listDisplayObjectNames();
        std::cout << "Factory::initFromLua() --> Installed display objects: ";
        for (const auto& name : names)
        {
            std::cout << name << " ";
        }
        std::cout << std::endl;
    }

    void Factory::processResource(const sol::table& resource)
    {
        if (!resource["type"].valid() || !resource["name"].valid() || !resource["config"].valid())
        {
            ERROR("Resource entry is missing required fields: 'type', 'name', or 'config'.");
            return;
        }
        std::string type = resource["type"];
        std::string name = resource["name"];
        sol::table config = resource["config"];
        auto creatorIt = creators_.find(type);
        if (creatorIt != creators_.end())
        {
            const TypeCreators& creators = creatorIt->second;
            std::unique_ptr<IDisplayObject> displayObj;
            if (creators.fromLua) {
                displayObj = creators.fromLua(config);
            }
            if (displayObj)
            {
                addDisplayObject(name, std::move(displayObj));
            }
            else
            {
                ERROR("Failed to create display object: " + name + " of type: " + type);
            }
        }
        else
        {
            ERROR("Unknown display object type: " + type);
        }
    }

    
    bool Factory::onUnitTest()
    {
        // std::cout << CLR::WHITE << "Factory Default Unit Tests:" << CLR::RESET << std::endl;
        return true;       
    } // END Factory::onUnitTest()

} // namespace SDOM

