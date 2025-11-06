// ============================================================================
// SDOM_AssetHandle.cpp
// ----------------------------------------------------------------------------
// @file SDOM_AssetHandle.cpp
// @brief Implementation of the SDOM::AssetHandle class.
//
// 🧭 Purpose:
//     Provides Lua bindings and lightweight management for assets registered
//     through SDOM::Factory. Exposes read-only identity accessors (`getName`,
//     `getType`, `getFilename`) and `isValid()` for runtime checks.
//
// 📜 Lua Binding Summary:
//     • Registers `AssetHandle` usertype and table in Lua.  
//     • Exposes only immutable identity accessors.  
//     • No setters are provided — assets are immutable post-creation.  
//
// 🧩 Related Classes:
//     • SDOM::IAssetObject — concrete asset type base class.  
//     • SDOM::Factory — creates and manages asset instances.  
//     • SDOM::Core — global orchestrator and registry.  
//
// ----------------------------------------------------------------------------
// ⚖️ License: ZLIB
// ----------------------------------------------------------------------------
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
// ----------------------------------------------------------------------------
// Author: Jay Faries (https://github.com/warte67)
// ============================================================================

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

        // Minimal surface similar to DisplayHandle; keep lightweight and safe.
        set_if_absent(handle, "isValid",      &AssetHandle::isValid);
        set_if_absent(handle, "getName",      &AssetHandle::getName_lua);
        set_if_absent(handle, "getType",      &AssetHandle::getType_lua);
        set_if_absent(handle, "getFilename",  &AssetHandle::getFilename_lua);

        // 🔒 These properties are immutable and should not have mutators.
        // Name, type, and filename remain constant for the asset’s lifetime.

        // Concrete IAssetObject implementations may safely extend this surface:
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