// SDOM_Handle.cpp

#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Handle.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp> 
#include <SDOM/SDOM_IResourceObject.hpp> 

namespace SDOM
{

    // Generic template (not implemented)
    template<typename T>
    T* SDOM::Handle<T>::get() const {
        static_assert(sizeof(T) == 0, "Handle<T>::get() must be specialized for this type.");
        return nullptr;
    }

    // Specialization for IDisplayObject (DomHandle)
    template<>
    IDisplayObject* SDOM::Handle<IDisplayObject>::get() const {
        if (!factory_) return nullptr;
        return factory_->getDomObj(name_);
    }

    // Specialization for IResourceObject (ResHandle)
    template<>
    IResourceObject* SDOM::Handle<IResourceObject>::get() const {
        if (!factory_) return nullptr;
        return factory_->getResObj(name_);
    }


} // END namespace SDOM

// // Explicit instantiation for needed types
// class IDisplayObject;
// class IResourceObject;
// template class SDOM::Handle<IDisplayObject>;
// template class SDOM::Handle<IResourceObject>;