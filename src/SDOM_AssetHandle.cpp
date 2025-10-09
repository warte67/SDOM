// SDOM_AssetHandle.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <iostream>

namespace SDOM
{


    AssetHandle::~AssetHandle()
    {
        // default dtor; ensure vtable emission
    }

    IAssetObject* AssetHandle::get() const
    {
        if (!factory_) return nullptr;
        IAssetObject* res = factory_->getResObj(name_);
        if (!res) return nullptr;
        return res;    
    }

    // Resolve an asset spec from Lua: accepts an AssetHandle userdata, a string name,
    // or a table with { name = "..." } or { parent = <asset> } style helpers.
    AssetHandle AssetHandle::resolveSpec(const sol::object& spec)
    {
        if (!spec.valid()) return AssetHandle();

        // if it's already an AssetHandle userdata
        try { if (spec.is<AssetHandle>()) return spec.as<AssetHandle>(); } catch(...) {}

        // if it's a simple string, treat it as the asset name
        try {
            if (spec.is<std::string>()) {
                AssetHandle out;
                out.name_ = spec.as<std::string>();
                return out;
            }
        } catch(...) {}

        // if it's a table, accept name / filename / type fields
        if (spec.is<sol::table>()) {
            sol::table t = spec.as<sol::table>();
            AssetHandle out;
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

        return AssetHandle();
    }


    sol::table AssetHandle::ensure_handle_table(sol::state_view lua)
    {
        if (!lua[LuaHandleName].valid())
        {
            lua.new_usertype<AssetHandle>(LuaHandleName, sol::no_constructor);
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

    void AssetHandle::bind_minimal(sol::state_view lua)
    {
        sol::table handle = ensure_handle_table(lua);

        // Minimal surface similar to DisplayHandle; keep lightweight.
        set_if_absent(handle, "isValid", &AssetHandle::isValid);
        set_if_absent(handle, "getName", &AssetHandle::getName_lua);
        set_if_absent(handle, "getType", &AssetHandle::getType_lua);
        set_if_absent(handle, "getFilename", &AssetHandle::getFilename_lua);
        set_if_absent(handle, "setName", [](AssetHandle& self, const std::string& newName) { self.name_ = newName; });
        set_if_absent(handle, "setType", [](AssetHandle& self, const std::string& newType) { self.type_ = newType; });
        set_if_absent(handle, "setFilename", [](AssetHandle& self, const std::string& newFilename) { self.filename_ = newFilename; });

        // Concrete IAssetObject implementations should call:
        //   auto t = AssetHandle::ensure_handle_table(lua);
        //   if (t["methodName"] == nil) t.set_function("methodName", ...);
    }

    void AssetHandle::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        if (DEBUG_REGISTER_LUA)
        {
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << "AssetHandle:" << getName()
                      << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA << typeName << CLR::RESET << std::endl;
        }

        // Ensure minimal shared handle exists.
        bind_minimal(lua);

        // Do not add type-specific bindings here.
    }

} // namespace SDOM