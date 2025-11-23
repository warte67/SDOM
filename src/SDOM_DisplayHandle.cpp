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
        return factory_->getDisplayObjectPtr(name_);
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

        // Register the C-facing opaque struct only once.  The struct conveys
        // the minimal identity data (id + metadata) necessary for generated
        // bindings to marshal handles across ABI boundaries.
        const std::string exportName = "SDOM_DisplayHandle";
        if (!lookup(exportName)) {
            SDOM::TypeInfo ti;
            ti.name        = exportName;
            ti.kind        = SDOM::EntryKind::Struct;
            ti.cpp_type_id = "SDOM::DisplayHandle";
            ti.file_stem   = "Handles";
            ti.export_name = exportName;
            ti.category    = "Handles";
            ti.doc         = "Opaque ABI struct describing an SDOM display handle.";

            auto addField = [&](const std::string& fieldName,
                                const std::string& cppType,
                                const std::string& doc) {
                SDOM::PropertyInfo p;
                p.name      = fieldName;
                p.cpp_type  = cppType;
                p.read_only = false;
                p.doc       = doc;
                ti.properties.push_back(std::move(p));
            };

            addField("object_id", "uint64_t", "Factory-assigned identifier (0 == invalid). ");
            addField("name", "const char*", "Stable display object name token.");
            addField("type", "const char*", "Display object type string (Button, Label, …). ");

            registry().registerType(ti);
        }

        registerMethod(
            typeName,
            "isValid",
            "bool DisplayHandle::isValid() const",
            "bool",
            "SDOM_DisplayHandle_IsValid",
            "bool SDOM_DisplayHandle_IsValid(const SDOM_DisplayHandle* handle)",
            "Returns true if the referenced display object currently resolves to a live instance.",
            [](const DisplayHandle* handle) -> bool {
                return handle && handle->isValid();
            });
    }    

} // namespace SDOM
