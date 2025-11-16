// SDOM_IAssetObject.cpp

#include <sol/sol.hpp> 
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>

namespace SDOM
{

    IAssetObject::IAssetObject(const InitStruct& init)
        : IDataObject()
        , name_(init.name)
        , type_(init.type)
        , filename_(init.filename)
        , isInternal_(init.is_internal)
        , isLoaded_(false)
    {
    }

    IAssetObject::IAssetObject()
        : IDataObject()
        , name_(TypeName)
        , type_(TypeName)
        , filename_("")
        , isInternal_(true)
        , isLoaded_(false)
    {
    }

    IAssetObject::~IAssetObject()
    {
        // default dtor; ensure emission for vtable
    }

    // Public non-virtual lifecycle helpers ----------------------------------
    bool IAssetObject::startup()
    {
        if (started_) return true;
        bool ok = false;
        try {
            ok = onInit();
        } catch(...) { ok = false; }
        if (ok) started_ = true;
        return ok;
    }

    void IAssetObject::shutdown()
    {
        if (!started_) return;
        try {
            // Ensure resource is unloaded before quitting
            if (isLoaded_) unload();
            onQuit();
        } catch(...) {}
        started_ = false;
    }

    bool IAssetObject::load()
    {
        if (isLoaded_) return false;
        onLoad();
        isLoaded_ = true;
        return true;
    }

    void IAssetObject::unload()
    {
        if (!isLoaded_) return;
        onUnload();
        isLoaded_ = false;
    }
    
    void IAssetObject::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "IAssetObject");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    

} // namespace SDOM