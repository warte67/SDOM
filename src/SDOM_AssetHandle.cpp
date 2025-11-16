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
    IAssetObject* res = factory_->getAssetObjectPtr(name_);
        if (!res) return nullptr;
        return res;    
    }


    void AssetHandle::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "AssetHandle");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    

} // namespace SDOM
