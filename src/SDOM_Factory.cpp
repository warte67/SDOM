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


    // JSON based object creator
    ResourceHandle Factory::create(const std::string& typeName, const Json& config)
    {
        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromJson) 
        {
            auto resource = it->second.fromJson(config);
            if (resource) 
            {
                std::string name = config.value("name", "");
                resources_[name] = std::move(resource);
                return ResourceHandle(name, typeName);
            }
        }
        return ResourceHandle(); // Invalid handle
    }

    // String (JSON text) based object creator
    ResourceHandle Factory::create(const std::string& typeName, const std::string& jsonStr) 
    {
        Json config = Json::parse(jsonStr);
        return create(typeName, config);
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

} // namespace SDOM