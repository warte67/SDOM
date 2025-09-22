// SDOM_ResourceHandle.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>
#include <SDOM/SDOM_ResourceHandle.hpp>

namespace SDOM
{

    IResourceObject* ResourceHandle::get() const 
    {
        if (!factory_) 
            return nullptr;
        return factory_->getResource(name_);
    }
}