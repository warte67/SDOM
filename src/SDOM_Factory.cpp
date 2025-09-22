// SDOM_Factory.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_ResourceHandle.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>

namespace SDOM
{

    Factory::Factory() : IDataObject()
    {
        // make sure the ResourceHandles can access this Factory
        ResourceHandle::factory_ = this;

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

    IResourceObject* Factory::getResource(const std::string& name) 
    {
        auto it = resources_.find(name);
        if (it != resources_.end()) 
        {
            return it->second.get();
        }
        return nullptr;
    }
    
    ResourceHandle Factory::getResourcePtr(const std::string& name) 
    {
        auto it = resources_.find(name);
        if (it != resources_.end()) 
        {
            // Use the resource's type for the ResourceHandle
            return ResourceHandle(name, it->second->getType());
        }
        // Return an empty ResourceHandle if not found
        return ResourceHandle();
    }


    // // JSON based object creator
    ResourceHandle Factory::create(const std::string& typeName, const Json& config)
    {
        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromJson) 
        {
            auto resource = it->second.fromJson(config);
            if (!resource) {
                std::cout << "Factory::create: Failed to create resource of type '" << typeName
                        << "' from JSON. Resource is nullptr.\n";
                return ResourceHandle(); // Invalid handle
            }
            std::string name = config.value("name", "");
            resources_[name] = std::move(resource);
            resources_[name]->onInit(); // Initialize the resource
            return ResourceHandle(name, typeName);
        }
        return ResourceHandle(); // Invalid handle
    }

    // String (JSON text) based object creator
    ResourceHandle Factory::create(const std::string& typeName, const std::string& jsonStr) 
    {
        Json config = Json::parse(jsonStr);
        ResourceHandle ret = create(typeName, config);
        if (ret)
            ret->onInit(); // Initialize the resource
        return ret;
    }    

    // InitStruct based object creator
    ResourceHandle Factory::create(const std::string& typeName, const IDisplayObject::InitStruct& init)
    {
        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromInitStruct) 
        {
            auto resource = it->second.fromInitStruct(init);
            if (resource) 
            {
                std::string name = init.name;
                resources_[name] = std::move(resource);
                resource->onInit(); // Initialize the resource
                return ResourceHandle(name, typeName);
            }
        }
        return ResourceHandle(); // Invalid handle
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
                ResourceHandle parentHandle = orphan->getParent();
                IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parentHandle.get());
                if (parentObj)
                {
                    // Remove orphan from parent's children using public removeChild method
                    parentObj->removeChild(orphanHandle);
                    // Set orphan's parent to nullptr using public setParent method
                    orphan->setParent(ResourceHandle());
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

    void Factory::addToOrphanList(const ResourceHandle orphan) 
    {
        if (orphan) 
        {
            orphanList_.push_back(orphan);
        }
    }

    void Factory::addToFutureChildrenList(const ResourceHandle child, const ResourceHandle parent, bool useWorld, int worldX, int worldY) 
    {
        if (child && parent) 
        {

            // std::cout << "child added to queue: " << child->getName() << " to (" << 
            //     worldX << ", " << worldY << ", w:" << child->getWidth() 
            //     << ", h:" << child->getHeight() << ")" << std::endl; 

            futureChildrenList_.push_back({child, parent, useWorld, worldX, worldY});
        }
    }

    
} // namespace SDOM