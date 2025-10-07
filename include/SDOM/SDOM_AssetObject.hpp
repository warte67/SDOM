// SDOM_AssetObject.hpp
#pragma once
// #include <string>
// #include <sol/sol.hpp>
#include <SDOM/SDOM_IDataObject.hpp>
// #include <SDOM/SDOM_Factory.hpp>
// #include <SDOM/SDOM_IAssetObject.hpp>

namespace SDOM
{
    class IAssetObject;

    // Lightweight handle for IAssetObject (idempotent Lua bindings, no heavy APIs).
    class AssetObject : public IDataObject
    {
        using SUPER = IDataObject;

    public:
        static constexpr const char* LuaHandleName = "AssetObject";

        AssetObject() : name_(LuaHandleName), type_(LuaHandleName), filename_(LuaHandleName) {}

        AssetObject(const std::string& name, const std::string& type, const std::string& filename) : name_(name), type_(type), filename_(filename) {}
        AssetObject(const AssetObject& other)
            : name_(other.name_), type_(other.type_), filename_(other.filename_) {}


        virtual ~AssetObject();

        // virtual methods from IDataObject
        virtual bool onInit() override { return true; }
        virtual void onQuit() override { }
        virtual bool onUnitTest() override { return true; }

        // Return the concrete IAssetObject (or nullptr)
        IAssetObject* get() const;

        bool isValid() const { return get() != nullptr; }

        const std::string& getName() const { return name_; }
        const std::string& getType() const { return type_; }
        const std::string& getFilename() const { return filename_; }

        // Lua-friendly accessors
        std::string getName_lua() const { return getName(); }
        std::string getType_lua() const { return getType(); }
        std::string getFilename_lua() const { return getFilename(); }

        // Ensure the shared Lua handle table exists for AssetObject
        static sol::table ensure_handle_table(sol::state_view lua);

        // Bind the minimal handle surface (idempotent)
        static void bind_minimal(sol::state_view lua);

        // Resolve a Lua spec/object into an AssetObject handle
        static AssetObject resolveSpec(const sol::object& spec);

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