// SDOM_IAssetObject.hpp
#pragma once
// #include <sol/sol.hpp> 
#include <SDOM/SDOM_IDataObject.hpp>

namespace SDOM
{
    
    class IAssetObject : public IDataObject
    {
        using SUPER = IDataObject;

    public:

        static constexpr const char* TypeName = "IAssetObject";

        // --- Construction & Initialization --- //
        struct InitStruct {
            std::string name = TypeName;
            std::string type = TypeName;
            std::string filename = "";
            bool isInternal = true;
        };

        IAssetObject(const InitStruct& init);
        IAssetObject(const sol::table& config);
        IAssetObject();

        virtual ~IAssetObject();

        virtual bool onInit() = 0;
        virtual void onQuit() = 0;
        virtual void onLoad() = 0;
        virtual void onUnload() = 0;
        virtual bool onUnitTest() override { return true; }

        // accessors (read only)
        const std::string& getType() const { return type_; }
        const std::string& getName() const { return name_; }
        const std::string& getFilename() const { return filename_; }
        bool isInternal() const { return isInternal_; }
        bool isLoaded() const { return isLoaded_; }

        void setType(const std::string& type) { type_ = type; }
        void setName(const std::string& name) { name_ = name; }
        void setFilename(const std::string& filename) { filename_ = filename; }

    protected:
        std::string name_;
        std::string type_;
        std::string filename_;
        bool isInternal_;
        bool isLoaded_ = false;

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua) override;
        sol::usertype<IAssetObject> objHandleType_;

    }; // class IAssetObject

} // end namespace SDOM