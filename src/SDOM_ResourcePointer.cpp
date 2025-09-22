// SDOM_ResourcePointer.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>
#include <SDOM/SDOM_ResourcePointer.hpp>

namespace SDOM
{

    IResourceObject* resource_ptr::get() const 
    {
        if (!factory_) 
            return nullptr;
        return factory_->getResource(name_);
    }
}