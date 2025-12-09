// SDOM_DisplayHandle.hpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <functional>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>
#include <SDOM/SDOM_IRangeControl.hpp>

namespace SDOM
{        

    DisplayHandle::DisplayHandle()
    {
        // keep lightweight; ctor must be emitted in a TU to provide the class's key function
    }

    DisplayHandle::~DisplayHandle()
    {
        // default dtor; ensure emission for vtable
    }

    IDisplayObject* DisplayHandle::get() const
    {
        if (!factory_) return nullptr;

        if (id_ != 0) {
            DisplayHandle resolved = factory_->resolveDisplayHandleById(id_);
            if (resolved.getId() != 0) {
                if (name_.empty()) {
                    const_cast<DisplayHandle*>(this)->name_ = resolved.getName();
                }
                if (type_.empty()) {
                    const_cast<DisplayHandle*>(this)->type_ = resolved.getType();
                }
                const_cast<DisplayHandle*>(this)->id_ = resolved.getId();
                if (auto* ptr = factory_->getDisplayObjectPtr(resolved.getName())) {
                    return ptr;
                }
            }
        }

        if (!name_.empty()) {
            return factory_->getDisplayObjectPtr(name_);
        }

        return nullptr;
    }

    void DisplayHandle::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "DisplayHandle");

        // Ensure the reflection entry for the C++ type exists so future
        // method/property registrations have a canonical home.
        (void)ensureType(typeName,
                 SDOM::EntryKind::Object,
                 "SDOM::DisplayHandle",
                 "Handles",
                 "SDOM_DisplayHandle",
                 "Handles",
                 "Lightweight proxy used to reference display objects by name/type.");

    }    

} // namespace SDOM
