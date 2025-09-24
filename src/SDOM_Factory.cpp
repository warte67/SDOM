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
            Stage::CreateFromJson,
            Stage::CreateFromInitStruct
        });

        // Register other built-in types here as needed ...        
    }


    void Factory::registerType(const std::string& typeName, const TypeCreators& creators)
    {
        creators_[typeName] = creators;
    }

    // IResourceObject* Factory::getResource(const std::string& name) 
    // {
    //     auto it = resources_.find(name);
    //     if (it != resources_.end()) 
    //     {
    //         return it->second.get();
    //     }
    //     return nullptr;
    // }
    IResourceObject* Factory::getResObj(const std::string& name)
    {
        auto it = resources_.find(name);
        if (it != resources_.end()) 
        {
            return dynamic_cast<IDisplayObject*>(it->second.get());
        }
        return nullptr;
    }
    IDisplayObject* Factory::getDomObj(const std::string& name)
    {
        auto it = resources_.find(name);
        if (it != resources_.end()) 
        {
            return dynamic_cast<IDisplayObject*>(it->second.get());
        }
        return nullptr;
    }



    DomHandle Factory::getDomHandle(const std::string& name) 
    {
        auto it = resources_.find(name);
        if (it != resources_.end()) 
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

    // // JSON based object creator
    DomHandle Factory::create(const std::string& typeName, const Json& config)
    {
        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromJson) 
        {
            auto resource = it->second.fromJson(config);
            if (!resource) {
                std::cout << "Factory::create: Failed to create resource of type '" << typeName
                        << "' from JSON. Resource is nullptr.\n";
                return DomHandle(); // Invalid handle
            }
            std::string name = config.value("name", "");
            resources_[name] = std::move(resource);
            resources_[name]->onInit(); // Initialize the resource
            return DomHandle(name, typeName);
        }
        return DomHandle(); // Invalid handle
    }

    // String (JSON text) based object creator
    DomHandle Factory::create(const std::string& typeName, const std::string& jsonStr) 
    {
        Json config = Json::parse(jsonStr);
        DomHandle ret = create(typeName, config);
        if (ret)
            ret->onInit(); // Initialize the resource
        return ret;
    }    

    // InitStruct based object creator
    DomHandle Factory::create(const std::string& typeName, const IDisplayObject::InitStruct& init)
    {
        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromInitStruct) 
        {
            auto resource = it->second.fromInitStruct(init);
            if (resource) 
            {
                std::string name = init.name;
                resource->onInit(); // Initialize the resource
                resources_[name] = std::move(resource);
                return DomHandle(name, typeName);
            }
        }
        return DomHandle(); // Invalid handle
    }


    void Factory::addResource(const std::string& name, 
        std::unique_ptr<IResourceObject> resource) 
    {
        resources_[name] = std::move(resource);
    }
    
    void Factory::removeResource(const std::string& name) 
    {
        resources_.erase(name);
    }

    std::vector<std::string> Factory::listResourceNames() const
    {
        std::vector<std::string> names;
        for (const auto& pair : resources_) {
            names.push_back(pair.first);
        }
        return names;
    }

    void Factory::clear()
    {
        resources_.clear();
    }

    void Factory::printRegistry() const
    {
        std::cout << "Factory Resource Registry:\n";
        for (const auto& pair : resources_) {
            const auto& name = pair.first;
            const auto& resource = pair.second;
            std::cout << "  Name: " << name
                    << ", Type: " << (resource ? resource->getType() : "Unknown")
                    << "\n";
        }
        std::cout << "Total resources: " << resources_.size() << std::endl;
    }

    

    // ----- Orphan and Future Child Management -----

    void Factory::detachOrphans()
    {
        for (auto& orphanHandle : orphanList_)
        {
            IDisplayObject* orphan = dynamic_cast<IDisplayObject*>(orphanHandle.get());
            if (orphan)
            {
                DomHandle parentHandle = orphan->getParent();
                IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parentHandle.get());
                if (parentObj)
                {
                    // Remove orphan from parent's children using public removeChild method
                    parentObj->removeChild(orphanHandle);
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
            IDisplayObject* child = dynamic_cast<IDisplayObject*>(futureChild.child.get());
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



    void Factory::initFromJson(const Json& json)
    {
        // Handle array of resources
        if (json.contains("resources") && json["resources"].is_array())
        {
            for (const auto& resource : json["resources"])
            {
                processResource(resource);
            }
        }

        // Handle single resource object
        if (json.contains("resource") && json["resource"].is_object())
        {
            processResource(json["resource"]);
        }

        // Debug: List installed resources
        auto names = listResourceNames();
        std::cout << "Factory::initFromJson() --> Installed resources: ";
        for (const auto& name : names)
        {
            std::cout << name << " ";
        }
        std::cout << std::endl;
    }

    void Factory::processResource(const Json& resource)
    {
        if (!resource.contains("type") || !resource.contains("name") || !resource.contains("config"))
        {
            ERROR("Resource entry is missing required fields: 'type', 'name', or 'config'.");
            return;
        }
        std::string type = resource["type"];
        std::string name = resource["name"];
        Json config = resource["config"];
        auto creatorIt = creators_.find(type);
        if (creatorIt != creators_.end())
        {
            const TypeCreators& creators = creatorIt->second;
            std::unique_ptr<IResourceObject> resourceObj;
            if (creators.fromJson) {
                resourceObj = creators.fromJson(config);
            }
            // Optionally, support InitStruct creation if needed:
            // else if (creators.fromInitStruct) {
            //     IDisplayObject::InitStruct init = ...; // convert config to InitStruct
            //     resourceObj = creators.fromInitStruct(init);
            // }
            if (resourceObj)
            {
                addResource(name, std::move(resourceObj));
            }
            else
            {
                ERROR("Failed to create resource: " + name + " of type: " + type);
            }
        }
        else
        {
            ERROR("Unknown resource type: " + type);
        }
    }

    bool Factory::onUnitTest()
    {
        // std::cout << CLR::WHITE << "Factory Default Unit Tests:" << CLR::RESET << std::endl;
        return true;       
    } // END Factory::onUnitTest()

} // namespace SDOM

