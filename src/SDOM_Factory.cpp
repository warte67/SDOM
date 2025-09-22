// SDOM_Factory.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_ResourcePointer.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>

namespace SDOM
{

    Factory::Factory() : IDataObject()
    {
        // Register built-in types
        registerType("Stage", Stage::Creator);

        resource_ptr::factory_ = this;
    }


    void Factory::registerType(const std::string& typeName, Creator creator)
    {
        creators_[typeName] = std::move(creator);
    }


    resource_ptr Factory::getResourcePtr(const std::string& name) 
    {
        auto it = resources_.find(name);
        if (it != resources_.end()) 
        {
            // Use the resource's type for the resource_ptr
            return resource_ptr(name, it->second->getType());
        }
        // Return an empty resource_ptr if not found
        return resource_ptr();
    }



} // namespace SDOM