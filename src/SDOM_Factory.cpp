// SDOM_Factory.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_ResourceHandle.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>

namespace SDOM
{

    Factory::Factory() : IDataObject()
    {
        // Register built-in types
        registerType("Stage", Stage::Creator);

        ResourceHandle::factory_ = this;
    }


    void Factory::registerType(const std::string& typeName, Creator creator)
    {
        creators_[typeName] = std::move(creator);
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



} // namespace SDOM