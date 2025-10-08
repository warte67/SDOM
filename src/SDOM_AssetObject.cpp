// SDOM_AssetObject.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>
#include <SDOM/SDOM_AssetObject.hpp>
#include <iostream>

namespace SDOM
{


    AssetObject::~AssetObject()
    {
        // default dtor; ensure vtable emission
    }

    IAssetObject* AssetObject::get() const
    {
        if (!factory_) return nullptr;
        IAssetObject* res = factory_->getResObj(name_);
        if (!res) return nullptr;
        // Ensure the resource is loaded before returning the pointer.
        try 
        {
            if (!res->isLoaded()) { res->onLoad(); }
        } 
        catch (const std::exception& e) 
        {
            ERROR("AssetObject::get(): onLoad() threw: " + std::string(e.what()));
        } 
        catch (...) 
        {
            ERROR("AssetObject::get(): onLoad() threw unknown exception");
        }
        return res;    
    }

    // Resolve an asset spec from Lua: accepts an AssetObject userdata, a string name,
    // or a table with { name = "..." } or { parent = <asset> } style helpers.
    AssetObject AssetObject::resolveSpec(const sol::object& spec)
    {
        if (!spec.valid()) return AssetObject();

        // if it's already an AssetObject userdata
        try { if (spec.is<AssetObject>()) return spec.as<AssetObject>(); } catch(...) {}

        // if it's a simple string, treat it as the asset name
        try {
            if (spec.is<std::string>()) {
                AssetObject out;
                out.name_ = spec.as<std::string>();
                return out;
            }
        } catch(...) {}

        // if it's a table, accept name / filename / type fields
        if (spec.is<sol::table>()) {
            sol::table t = spec.as<sol::table>();
            AssetObject out;
            bool any = false;

            sol::object nameObj = t.get<sol::object>("name");
            if (nameObj.valid() && nameObj.is<std::string>()) {
                out.name_ = nameObj.as<std::string>();
                any = true;
            }

            sol::object typeObj = t.get<sol::object>("type");
            if (typeObj.valid() && typeObj.is<std::string>()) {
                out.type_ = typeObj.as<std::string>();
                any = true;
            }
            sol::object filenameObj = t.get<sol::object>("filename");
            if (filenameObj.valid() && filenameObj.is<std::string>()) {
                out.filename_ = filenameObj.as<std::string>();
                any = true;
            }

            if (any) {
                // factory resolution (e.g. set factory_ or leave name-only) is left to caller
                return out;
            }
        }

        return AssetObject();
    }


    sol::table AssetObject::ensure_handle_table(sol::state_view lua)
    {
        if (!lua[LuaHandleName].valid())
        {
            lua.new_usertype<AssetObject>(LuaHandleName, sol::no_constructor);
        }
        return lua[LuaHandleName];
    }

    static inline void set_if_absent(sol::table& handle, const char* name, auto&& fn)
    {
        sol::object cur = handle.raw_get_or(name, sol::lua_nil);
        if (!cur.valid() || cur == sol::lua_nil)
        {
            handle.set_function(name, std::forward<decltype(fn)>(fn));
        }
    }

    void AssetObject::bind_minimal(sol::state_view lua)
    {
        sol::table handle = ensure_handle_table(lua);

        // Minimal surface similar to DisplayObject; keep lightweight.
        set_if_absent(handle, "isValid", &AssetObject::isValid);
        set_if_absent(handle, "getName", &AssetObject::getName_lua);
        set_if_absent(handle, "getType", &AssetObject::getType_lua);
        set_if_absent(handle, "getFilename", &AssetObject::getFilename_lua);
        set_if_absent(handle, "setName", [](AssetObject& self, const std::string& newName) { self.name_ = newName; });
        set_if_absent(handle, "setType", [](AssetObject& self, const std::string& newType) { self.type_ = newType; });
        set_if_absent(handle, "setFilename", [](AssetObject& self, const std::string& newFilename) { self.filename_ = newFilename; });

        // Concrete IAssetObject implementations should call:
        //   auto t = AssetObject::ensure_handle_table(lua);
        //   if (t["methodName"] == nil) t.set_function("methodName", ...);
    }

    void AssetObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        if (DEBUG_REGISTER_LUA)
        {
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << "AssetObject:" << getName()
                      << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA << typeName << CLR::RESET << std::endl;
        }

        // Ensure minimal shared handle exists.
        bind_minimal(lua);

        // Do not add type-specific bindings here.
    }

} // namespace SDOM