/**
 * @class SDOM::AssetHandle
 * @brief Lightweight, non-owning handle for accessing assets within SDOM.
 * @author Jay Faries (https://github.com/warte67)
 * @inherits SDOM::IDataObject
 * @luaType AssetHandle
 *
 * The **AssetHandle** class provides safe, lightweight access to assets managed
 * by `SDOM::Factory` and `SDOM::Core`. It is a non-owning reference type, designed
 * for Lua exposure as `AssetHandle` to support safe lookup and inspection of
 * registered assets such as textures, sprite sheets, and fonts.
 *
 * ---
 * @section AssetHandle_Purpose Purpose
 * - Offers idempotent, immutable references to loaded assets.
 * - Acts as a stable interface between the C++ factory and Lua runtime.
 * - Eliminates accidental ownership or duplication of asset objects.
 * - Allows subclasses (e.g., `Texture`, `SpriteSheet`, `BitmapFont`) to extend Lua bindings safely.
 *
 * ---
 * @section AssetHandle_Properties Properties
 * | Property | Type | Description |
 * |-----------|------|-------------|
 * | `name` | string | The registered asset name (immutable). |
 * | `type` | string | The asset type identifier (immutable). |
 * | `filename` | string | The source filename for the asset (immutable). |
 * | `isValid` | bool | Returns `true` if the handle currently references a valid asset. |
 *
 * ---
 * @section AssetHandle_Functions Functions
 * | Function | Returns | Description |
 * |-----------|----------|-------------|
 * | `getName()` | string | Returns the registered asset name. |
 * | `getType()` | string | Returns the asset’s type identifier. |
 * | `getFilename()` | string | Returns the asset’s source filename. |
 * | `isValid()` | bool | Returns `true` if the asset is currently valid. |
 * | `as<T>()` | `T*` | Returns a typed pointer to the concrete asset, or `nullptr`. |
 * | `get()` | `IAssetObject*` | Returns the raw asset pointer, or `nullptr` if not loaded. |
 *
 * ---
 * @section AssetHandle_Notes Notes
 * - `AssetHandle` instances are **immutable**; name, type, and filename cannot change.
 * - The handle does not own the underlying asset — destruction of the asset invalidates the handle.
 * - Typically retrieved via `Factory::getAssetObject(name)` or from Lua using `Core:getAssetObject()`.
 * - All bindings are idempotent and safe to re-register.
 *
 * ---
 * @section License ZLIB License
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once
#include <SDOM/SDOM_IDataObject.hpp>

namespace SDOM
{
    class Core;
    class Factory;
    class IAssetObject;

    /**
     * @brief Non-owning reference to a registered SDOM asset.
     */
    class AssetHandle : public IDataObject
    {
        using SUPER = IDataObject;

    public:
        static constexpr const char* LuaHandleName = "AssetHandle";

        // ---------------------------------------------------------------------
        // 🏗️ Construction / Destruction
        // ---------------------------------------------------------------------
        AssetHandle()
            : name_(LuaHandleName), type_(LuaHandleName), filename_(LuaHandleName)
        {}
        AssetHandle(const std::string& name, const std::string& type, const std::string& filename)
            : name_(name), type_(type), filename_(filename)
        {}
        AssetHandle(const AssetHandle& other)
            : name_(other.name_), type_(other.type_), filename_(other.filename_)
        {}
        virtual ~AssetHandle();

        // ---------------------------------------------------------------------
        // ⚙️ IDataObject Virtual Methods
        // ---------------------------------------------------------------------
        bool onInit() override { return true; }
        void onQuit() override {}
        bool onUnitTest(int frame) override { (void)frame; return true; }

        // ---------------------------------------------------------------------
        // 🧩 Core Handle Management
        // ---------------------------------------------------------------------
        void reset() { name_.clear(); }
        IAssetObject* get() const;
        template<typename T> T* as() const { return dynamic_cast<T*>(get()); }
        IAssetObject& operator*() const { return *get(); }
        IAssetObject* operator->() const { return get(); }
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
        bool isValid() const { return get() != nullptr; }
        const std::string& getName() const { return name_; }
        const std::string& getType() const { return type_; }
        const std::string& getFilename() const { return filename_; }

        // ---------------------------------------------------------------------
        // 📜 Lua-Friendly Accessors
        // ---------------------------------------------------------------------
        std::string getName_lua() const { return getName(); }
        std::string getType_lua() const { return getType(); }
        std::string getFilename_lua() const { return getFilename(); }

        // ---------------------------------------------------------------------
        // 📜 Lua Binding Helpers
        // ---------------------------------------------------------------------
        static sol::table ensure_handle_table(sol::state_view lua);
        static void bind_minimal(sol::state_view lua);
        static AssetHandle resolveSpec(const sol::object& spec);

    public:
        inline static Factory* factory_ = nullptr;

    protected:
        friend Core;
        friend Factory;

        // ---------------------------------------------------------------------
        // 🧩 Internal Identity
        // ---------------------------------------------------------------------
        std::string name_;
        std::string type_;
        std::string filename_;

        // ---------------------------------------------------------------------
        // 📜 Legacy Lua Extension Hook
        // ---------------------------------------------------------------------
        void _registerLuaBindings(const std::string& typeName, sol::state_view lua) override;


        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;        
    };
} // namespace SDOM
