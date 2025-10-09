#pragma once
// #include <SDOM/SDOM.hpp>
// #include <SDOM/SDOM_IDisplayObject.hpp>
// #include <SDOM/SDOM_DisplayHandle.hpp>
// #include <SDOM/SDOM_IAssetObject.hpp>
// #include <SDOM/SDOM_AssetHandle.hpp>

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <sol/sol.hpp>

// include the concrete interface headers so unique_ptr<T> is instantiated with a complete type
// #include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>   // required for std::unique_ptr<IDisplayObject>
#include <SDOM/SDOM_IAssetObject.hpp>     // required for std::unique_ptr<IAssetObject>
 

namespace SDOM 
{
    class Stage;
    class IDisplayObject;
    class DisplayHandle;
    class IAssetObject;
    class AssetHandle;

    // --- Type Creation Structs --- //
    struct TypeCreators 
    {
        std::function<std::unique_ptr<IDisplayObject>(const sol::table&)> fromLua;
        std::function<std::unique_ptr<IDisplayObject>(const IDisplayObject::InitStruct&)> fromInitStruct;
    };

    struct AssetTypeCreators 
    {
        std::function<std::unique_ptr<IAssetObject>(const sol::table&)> fromLua;
        std::function<std::unique_ptr<IAssetObject>(const IAssetObject::InitStruct&)> fromInitStruct;
    };

    class Factory final : public IDataObject
    {
        friend class Core;  // Core should have direct access to the Factory internals

    public:
        // --- Lifecycle (IDataObject overrides) --- //
        Factory();
        virtual ~Factory() = default;
        virtual bool onInit() override;
        virtual void onQuit() override {}
        virtual bool onUnitTest() override;

        // --- Object Type Registration --- //
        void registerDomType(const std::string& typeName, const TypeCreators& creators);  // change to registerDisplayObjectType()
        void registerResType(const std::string& typeName, const AssetTypeCreators& creators);  // change to registerAssetObjectType()

        // --- Object Creation --- //
        DisplayHandle create(const std::string& typeName, const sol::table& config);
        DisplayHandle create(const std::string& typeName, const IDisplayObject::InitStruct& init);        
        DisplayHandle create(const std::string& typeName, const std::string& luaScript);

        AssetHandle createAsset(const std::string& typeName, const sol::table& config);
        AssetHandle createAsset(const std::string& typeName, const IAssetObject::InitStruct& init);        
        AssetHandle createAsset(const std::string& typeName, const std::string& luaScript);

        // Helper: attach a newly-created display object (by name/type) to a
        // parent specified in a Lua config value. Accepts string name, DomHandle,
        // or a nested table { parent = ... }. Returns true if attachment occurred.
        bool attachCreatedObjectToParentFromConfig(const std::string& name, const std::string& typeName, const sol::object& parentConfig);

        // --- Object Lookup --- //
        IDisplayObject* getDomObj(const std::string& name);  // change to getIDisplayObject()
        IAssetObject* getResObj(const std::string& name);   //change to getIAssetObject()        
        DisplayHandle getDisplayObject(const std::string& name);
        AssetHandle getAssetObject(const std::string& name);
        DisplayHandle getStageHandle();

        // --- Display Object Management --- //

        void addDisplayObject(const std::string& name, std::unique_ptr<IDisplayObject> displayObject); // Deprecated: Consider refactoring/removal

        void destroyDisplayObject(const std::string& name);
        void destroyAssetObject(const std::string& name);   // NEW, needs LUA bindings

        // --- Orphan Management --- //
        int countOrphanedDisplayObjects() const;
        std::vector<DisplayHandle> getOrphanedDisplayObjects();
        void destroyOrphanedDisplayObjects();
        void detachOrphans();
        void collectGarbage();  // Maintenance orphaned objects based on their retention policy


        // --- Future Child Management --- //
        void attachFutureChildren();
        void addToOrphanList(const DisplayHandle orphan);
        void addToFutureChildrenList(const DisplayHandle child, const DisplayHandle parent,
            bool useWorld=false, int worldX=0, int worldY=0);

        // --- Utility Methods --- //
        std::vector<std::string> listDisplayObjectNames() const;
        // std::vector<std::string> listResourceNames() const;
        void clear();
        void printObjectRegistry() const;
        void printAssetRegistry() const;   // NEW, needs LUA bindings

        // --- Internal Lua Integration --- //
        void initFromLua(const sol::table& lua);
        // void initLuaProcessResource(const sol::table& resource);

    private:
        // --- Internal Storage --- //
        std::unordered_map<std::string, std::unique_ptr<IDisplayObject>> displayObjects_;
        std::unordered_map<std::string, std::unique_ptr<IAssetObject>> assetObjects_;
        std::unordered_map<std::string, TypeCreators> creators_;
        std::unordered_map<std::string, AssetTypeCreators> assetCreators_;

        // --- Orphan & Future Child Lists --- //

        std::vector<DisplayHandle> orphanList_;
        struct futureChild 
        {
            DisplayHandle child;
            DisplayHandle parent;
            bool preserveWorldPosition;
            int dragStartWorldX;
            int dragStartWorldY;
        };            
        std::vector<futureChild> futureChildrenList_;


    };
    

}


