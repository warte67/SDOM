#include <SDOM/SDOM_IDisplayObjectAPI.hpp>

#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_CoreAPI.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/CAPI/SDOM_CAPI_Handles.h>

#include <exception>
#include <mutex>
#include <string>

namespace SDOM {
namespace IDisplayObjectAPI {
namespace {

constexpr const char* kApiTypeName = "IDisplayObject";

IDisplayObject* resolveSubject(const SDOM_DisplayHandle* handle,
                               const char* contextLabel)
{
    if (!handle || !handle->name) {
        std::string message = std::string(contextLabel) + ": handle is null or missing name";
        SDOM::CoreAPI::setErrorMessage(message.c_str());
        return nullptr;
    }

    SDOM::Factory& factory = SDOM::Core::getInstance().getFactory();
    IDisplayObject* subject = factory.getDisplayObjectPtr(handle->name);
    if (!subject) {
        std::string message = std::string(contextLabel) + ": display object '" + handle->name + "' not found";
        SDOM::CoreAPI::setErrorMessage(message.c_str());
        return nullptr;
    }
    return subject;
}

template<typename Fn>
bool invokeWithSubject(const SDOM_DisplayHandle* handle,
                       const char* contextLabel,
                       Fn&& fn)
{
    try {
        IDisplayObject* subject = resolveSubject(handle, contextLabel);
        if (!subject) {
            return false;
        }
        fn(*subject);
        return true;
    } catch (const std::exception& e) {
        SDOM::CoreAPI::setErrorMessage(e.what());
    } catch (...) {
        SDOM::CoreAPI::setErrorMessage("IDisplayObjectAPI: unexpected exception");
    }
    return false;
}

} // namespace

void registerBindings(IDisplayObject& object, const std::string& typeName)
{
    (void)typeName;

    static std::once_flag s_once;
    std::call_once(s_once, [&object]() {
        SDOM::TypeInfo& typeInfo = object.ensureType(
            kApiTypeName,
            SDOM::EntryKind::Object,
            "SDOM::IDisplayObject",
            "IDisplayObject",
            "SDOM_IDisplayObject",
            "DisplayObjects",
            "IDisplayObject base bindings for dirty/state management.");

        typeInfo.subject_kind = "DisplayObject";
        typeInfo.subject_uses_handle = true;
        typeInfo.has_handle_override = true;
        typeInfo.dispatch_family_override = "method_table";

        object.setModuleBrief(typeInfo,
            "Common runtime services for SDOM display objects, including dirty flag control and state queries.");

        const std::string& apiTypeName = typeInfo.name;

        object.registerMethod(
            apiTypeName,
            "CleanAll",
            "void IDisplayObject::cleanAll()",
            "bool",
            "SDOM_IDisplayObject_CleanAll",
            "bool SDOM_IDisplayObject_CleanAll(const SDOM_DisplayHandle* handle)",
            "Clears the dirty state for the target display object and cascades to its descendants.",
            [](const SDOM_DisplayHandle* handle) -> bool {
                return invokeWithSubject(handle, "SDOM_IDisplayObject_CleanAll", [](IDisplayObject& subject) {
                    subject.cleanAll();
                });
            });

        object.registerMethod(
            apiTypeName,
            "SetDirty",
            "bool IDisplayObjectAPI::SetDirty(IDisplayObject* object)",
            "bool",
            "SDOM_IDisplayObject_SetDirty",
            "bool SDOM_IDisplayObject_SetDirty(const SDOM_DisplayHandle* handle)",
            "Marks the display object as dirty so it will be refreshed next frame.",
            [](const SDOM_DisplayHandle* handle) -> bool {
                return invokeWithSubject(handle, "SDOM_IDisplayObject_SetDirty", [](IDisplayObject& subject) {
                    subject.setDirty();
                });
            });

        object.registerMethod(
            apiTypeName,
            "SetDirtyState",
            "bool IDisplayObjectAPI::SetDirtyState(IDisplayObject* object, bool dirty)",
            "bool",
            "SDOM_IDisplayObject_SetDirtyState",
            "bool SDOM_IDisplayObject_SetDirtyState(const SDOM_DisplayHandle* handle, bool dirty)",
            "Explicitly sets the dirty flag on the target display object to the provided state.",
            [](const SDOM_DisplayHandle* handle, bool dirty) -> bool {
                return invokeWithSubject(handle, "SDOM_IDisplayObject_SetDirtyState", [dirty](IDisplayObject& subject) {
                    subject.setDirty(dirty);
                });
            });

        object.registerMethod(
            apiTypeName,
            "IsDirty",
            "bool IDisplayObject::isDirty() const",
            "bool",
            "SDOM_IDisplayObject_IsDirty",
            "bool SDOM_IDisplayObject_IsDirty(const SDOM_DisplayHandle* handle)",
            "Returns true if the target display object is currently marked dirty.",
            [](const SDOM_DisplayHandle* handle) -> bool {
                bool result = false;
                invokeWithSubject(handle, "SDOM_IDisplayObject_IsDirty", [&result](IDisplayObject& subject) {
                    result = subject.isDirty();
                });
                return result;
            });
    });
}

} // namespace IDisplayObjectAPI
} // namespace SDOM
