// SDOM_AssetHandle.hpp
#pragma once
#include <SDOM/SDOM_IDataObject.hpp>

namespace SDOM
{
    class Core;
    class Factory;
    class IAssetObject;

    // Lightweight handle for IAssetObject (idempotent Lua bindings, no heavy APIs).
    class AssetHandle : public IDataObject
    {
        using SUPER = IDataObject;

    public:
        static constexpr const char* LuaHandleName = "AssetHandle";

        AssetHandle() : name_(LuaHandleName), type_(LuaHandleName), filename_(LuaHandleName) {}
        AssetHandle(const std::string& name, const std::string& type, const std::string& filename)
            : name_(name), type_(type), filename_(filename) {}
        AssetHandle(const AssetHandle& other)
            : name_(other.name_), type_(other.type_), filename_(other.filename_) {}


        virtual ~AssetHandle();

        // virtual methods from IDataObject
        virtual bool onInit() override { return true; }
        virtual void onQuit() override { }
        virtual bool onUnitTest() override { return true; }

        void reset() {name_.clear(); }

        // Return the concrete IAssetObject (or nullptr)
        IAssetObject* get() const;
        template<typename T>
        T* as() const { return dynamic_cast<T*>(get()); }

        IAssetObject& operator*() const { return *get(); }
        IAssetObject* operator->() const { return get(); }
        operator bool() const { return get() != nullptr; }

        bool operator==(std::nullptr_t) const { return get() == nullptr; }
        bool operator!=(std::nullptr_t) const { return get() != nullptr; }
        bool operator==(const AssetHandle& other) const { return name_ == other.name_ && type_ == other.type_ && filename_ == other.filename_; }
        bool operator!=(const AssetHandle& other) const { return !(*this == other); }

        AssetHandle& operator=(const AssetHandle& other) = default;
        AssetHandle(AssetHandle&&) = default;
        AssetHandle& operator=(AssetHandle&&) = default;
        
        bool isValid() const { return get() != nullptr; }

        const std::string& getName() const { return name_; }
        const std::string& getType() const { return type_; }
        const std::string& getFilename() const { return filename_; }

        // Lua-friendly accessors
        std::string getName_lua() const { return getName(); }
        std::string getType_lua() const { return getType(); }
        std::string getFilename_lua() const { return getFilename(); }

        // Ensure the shared Lua handle table exists for AssetHandle
        static sol::table ensure_handle_table(sol::state_view lua);

        // Bind the minimal handle surface (idempotent)
        static void bind_minimal(sol::state_view lua);

        // Resolve a Lua spec/object into an AssetHandle
        static AssetHandle resolveSpec(const sol::object& spec);

    public:
        inline static Factory* factory_ = nullptr;
    protected:
        friend Core;
        friend Factory;

        // Underlying factory and identity (lightweight; public for convenience)
        std::string name_;
        std::string type_;
        std::string filename_;
      
        // Called by concrete IAssetObject implementations to register type-specific exposure
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua) override;

    };

} // namespace SDOM