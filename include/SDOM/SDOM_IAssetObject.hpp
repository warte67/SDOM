// SDOM_IAssetObject.hpp
#pragma once
// #include <sol/sol.hpp> 
#include <SDOM/SDOM_IDataObject.hpp>
#include <string>

namespace SDOM
{
    
    class IAssetObject : public IDataObject
    {
        using SUPER = IDataObject;

    public:

        static constexpr const char* TypeName = "IAssetObject";

        // --- Construction & Initialization --- //
        struct InitStruct
        {
            std::string name     = TypeName;   // default registry key
            std::string type     = TypeName;   // asset type name
            std::string filename = "";         // backing file or virtual identifier
            bool is_internal     = true;       // legacy: was "isInternal"

            // ------------------------------------------------------------
            // JSON deserializer — correct signature & inheritance-ready
            // ------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                if (j.contains("name"))
                    out.name = j["name"].get<std::string>();

                if (j.contains("type"))
                    out.type = j["type"].get<std::string>();

                if (j.contains("filename"))
                    out.filename = j["filename"].get<std::string>();

                if (j.contains("is_internal"))
                    out.is_internal = j["is_internal"].get<bool>();
            }
        };



        IAssetObject(const InitStruct& init);
        IAssetObject();

        virtual ~IAssetObject();

        bool onInit() override = 0;
        void onQuit() override = 0;
        virtual void onLoad() = 0;
        virtual void onUnload() = 0;
        // Owner-controlled load/unload helpers (idempotent)
        bool load();
        void unload();
        // Owner-controlled lifecycle helpers (idempotent)
        bool startup();
        void shutdown();
        bool onUnitTest(int frame) override { (void)frame; return true; }

        // accessors
        const std::string& getType() const { return type_; }
        const std::string& getName() const { return name_; }
        const std::string& getFilename() const { return filename_; }
        bool isInternal() const { return isInternal_; }
        bool isLoaded() const { return isLoaded_; }
        // mutators
        void setType(const std::string& type) { type_ = type; }
        void setName(const std::string& name) { name_ = name; }
        void setFilename(const std::string& filename) { filename_ = filename; }

        template <typename T>
        T* as() { return dynamic_cast<T*>(this); }

    protected:
        std::string name_;
        std::string type_;
        std::string filename_;
        bool isInternal_;

        // Lifecycle state: true if startup() completed and onInit() ran.
        bool started_ = false;
        // Resource loaded state for onLoad()/onUnload() ownership
        bool isLoaded_ = false;

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;      

    }; // class IAssetObject

    // --- comparison operators: MUST be at namespace scope (not inside the class) --- //
    inline bool operator<(const IAssetObject& a, const IAssetObject& b) { return a.getName() < b.getName(); }
    inline bool operator==(const IAssetObject& a, const IAssetObject& b) { return a.getName() == b.getName() && a.getType() == b.getType() && a.getFilename() == b.getFilename(); }
    inline bool operator<=(const IAssetObject& a, const IAssetObject& b) { return !(b < a); }


} // end namespace SDOM