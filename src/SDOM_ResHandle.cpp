// SDOM_ResHandle.cpp

#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_ResHandle.hpp>

namespace SDOM
{
    IResourceObject* ResHandle::get() const
    {
        if (!factory_) return nullptr;
        return factory_->getResObj(name_);
    }

} // namespace SDOM
