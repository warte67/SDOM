// ============================================================================
// SDOM_AssetHandle.hpp
// ----------------------------------------------------------------------------
// @class SDOM::AssetHandle
// @brief Lightweight, non-owning handle for accessing assets within SDOM.
//
// 🧭 Purpose:
//     Provides safe, lightweight access to assets (e.g., textures, sounds,
//     sprite sheets) managed by SDOM::Factory and SDOM::Core. The handle is
//     non-owning and idempotent, suitable for exposure to Lua as `AssetHandle`.
//
// 🧩 Notes:
//     • `AssetHandle` represents a stable reference to an asset by name.
//     • The asset’s name, type, and filename are immutable once created.
//     • Designed for minimal exposure — concrete asset types may extend it.
//     • Used heavily in Lua bindings for safe asset lookup and retrieval.
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
#pragma once
#include <SDOM/SDOM_IDataObject.hpp>

namespace SDOM
{
    class Core;
    class Factory;
    class IAssetObject;

    // =========================================================================
    // 🧩 SDOM::AssetHandle
    // =========================================================================
    class AssetHandle : public IDataObject
    {
        using SUPER = IDataObject;

    public:
        static constexpr const char* LuaHandleName = "AssetHandle";

        // ---------------------------------------------------------------------
        // 🏗️ Construction / Destruction
        // ---------------------------------------------------------------------

        /** @brief Default constructor initializes an empty handle. */
        AssetHandle()
            : name_(LuaHandleName), type_(LuaHandleName), filename_(LuaHandleName)
        {}

        /** @brief Constructs a handle with the given identifiers. */
        AssetHandle(const std::string& name, const std::string& type, const std::string& filename)
            : name_(name), type_(type), filename_(filename)
        {}

        /** @brief Copy constructor. */
        AssetHandle(const AssetHandle& other)
            : name_(other.name_), type_(other.type_), filename_(other.filename_)
        {}

        /** @brief Destructor (virtual to allow subclassing). */
        virtual ~AssetHandle();

        // ---------------------------------------------------------------------
        // ⚙️ IDataObject Virtual Methods
        // ---------------------------------------------------------------------
        virtual bool onInit() override { return true; }
        virtual void onQuit() override {}
        virtual bool onUnitTest(int frame) override { (void)frame; return true; }

        // ---------------------------------------------------------------------
        // 🧩 Core Handle Management
        // ---------------------------------------------------------------------

        /** @brief Clears this handle’s identifying information. */
        void reset() { name_.clear(); }

        /** @brief Returns the associated IAssetObject pointer (may be nullptr). */
        IAssetObject* get() const;

        /** @brief Attempts to cast the asset to a concrete subclass. */
        template<typename T>
        T* as() const { return dynamic_cast<T*>(get()); }

        /** @brief Dereferences the handle, returning the referenced object. */
        IAssetObject& operator*() const { return *get(); }

        /** @brief Provides pointer-style access to the underlying asset. */
        IAssetObject* operator->() const { return get(); }

        /** @brief Returns true if the handle currently references a valid asset. */
        operator bool() const { return get() != nullptr; }

        // ---------------------------------------------------------------------
        // ⚖️ Equality Operators
        // ---------------------------------------------------------------------
        bool operator==(std::nullptr_t) const { return get() == nullptr; }
        bool operator!=(std::nullptr_t) const { return get() != nullptr; }

        bool operator==(const AssetHandle& other) const
        {
            return name_ == other.name_ &&
                   type_ == other.type_ &&
                   filename_ == other.filename_;
        }
        bool operator!=(const AssetHandle& other) const { return !(*this == other); }

        AssetHandle& operator=(const AssetHandle& other) = default;
        AssetHandle(AssetHandle&&) = default;
        AssetHandle& operator=(AssetHandle&&) = default;

        // ---------------------------------------------------------------------
        // 🧭 Accessors (Immutable Identifiers)
        // ---------------------------------------------------------------------
        // These properties define the asset’s identity and must never change
        // during the asset’s lifetime. No corresponding mutators exist.

        /** @brief Returns true if this handle resolves to a valid asset. */
        bool isValid() const { return get() != nullptr; }

        /** @brief Returns the name of the asset (as registered in Factory). */
        const std::string& getName() const { return name_; }

        /** @brief Returns the asset’s type string. */
        const std::string& getType() const { return type_; }

        /** @brief Returns the filename associated with this asset. */
        const std::string& getFilename() const { return filename_; }

        // ---------------------------------------------------------------------
        // 📜 Lua-Friendly Accessors
        // ---------------------------------------------------------------------

        /** @brief Lua accessor alias for getName(). */
        std::string getName_lua() const { return getName(); }

        /** @brief Lua accessor alias for getType(). */
        std::string getType_lua() const { return getType(); }

        /** @brief Lua accessor alias for getFilename(). */
        std::string getFilename_lua() const { return getFilename(); }

        // ---------------------------------------------------------------------
        // 📜 Lua Binding Helpers
        // ---------------------------------------------------------------------

        /**
         * @brief Ensures the shared Lua table for AssetHandle exists.
         * @param lua The active sol::state_view instance.
         * @return The created or existing handle table.
         */
        static sol::table ensure_handle_table(sol::state_view lua);

        /**
         * @brief Registers the minimal AssetHandle surface for Lua exposure.
         * @param lua The active Lua state.
         * @note Exposes only immutable identity accessors and validity checks.
         */
        static void bind_minimal(sol::state_view lua);

        /**
         * @brief Converts a Lua table, string, or handle into an AssetHandle.
         * @param spec A Lua object referencing an asset by name or handle.
         * @return The resolved AssetHandle (may be empty if not found).
         */
        static AssetHandle resolveSpec(const sol::object& spec);

    public:
        // Shared factory pointer, managed externally.
        inline static Factory* factory_ = nullptr;

    protected:
        friend Core;
        friend Factory;

        // ---------------------------------------------------------------------
        // 🧩 Internal Identity
        // ---------------------------------------------------------------------
        std::string name_;       ///< Registered asset name (immutable)
        std::string type_;       ///< Asset type string (immutable)
        std::string filename_;   ///< Source filename for the asset (immutable)

        // ---------------------------------------------------------------------
        // 📜 Lua Extension Hook
        // ---------------------------------------------------------------------
        /**
         * @brief Allows concrete asset classes to register Lua extensions.
         * @param typeName The Lua-visible asset type name.
         * @param lua The active sol::state_view.
         * @note Overridden by subclasses to expose extra methods.
         */
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua) override;
    };

} // namespace SDOM
