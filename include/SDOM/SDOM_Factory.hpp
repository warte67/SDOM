#pragma once
// #include <SDOM/SDOM.hpp>
// #include <SDOM/SDOM_IDisplayObject.hpp>
// #include <SDOM/SDOM_DisplayObject.hpp>
// #include <SDOM/SDOM_IAssetObject.hpp>
// #include <SDOM/SDOM_AssetObject.hpp>

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
    class DisplayObject;
    class IAssetObject;
    class AssetObject;

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
        DisplayObject create(const std::string& typeName, const sol::table& config);
        DisplayObject create(const std::string& typeName, const IDisplayObject::InitStruct& init);        
        DisplayObject create(const std::string& typeName, const std::string& luaScript);

        AssetObject createAsset(const std::string& typeName, const sol::table& config);
        AssetObject createAsset(const std::string& typeName, const IAssetObject::InitStruct& init);        
        AssetObject createAsset(const std::string& typeName, const std::string& luaScript);

        // Helper: attach a newly-created display object (by name/type) to a
        // parent specified in a Lua config value. Accepts string name, DomHandle,
        // or a nested table { parent = ... }. Returns true if attachment occurred.
        bool attachCreatedObjectToParentFromConfig(const std::string& name, const std::string& typeName, const sol::object& parentConfig);

        // --- Object Lookup --- //
        IDisplayObject* getDomObj(const std::string& name);  // change to getIDisplayObject()
        IAssetObject* getResObj(const std::string& name);   //change to getIAssetObject()
        
        DisplayObject getDisplayObject(const std::string& name);
        AssetObject getAssetObject(const std::string& name);
        DisplayObject getStageHandle();

        // --- Display Object Management --- //
        void addDisplayObject(const std::string& name, std::unique_ptr<IDisplayObject> displayObject); // Consider refactoring/removal
        void destroyDisplayObject(const std::string& name);

        // --- Orphan Management --- //
        int countOrphanedDisplayObjects() const;
        std::vector<DisplayObject> getOrphanedDisplayObjects();
        void destroyOrphanedDisplayObjects();
        void detachOrphans();
        void collectGarbage();  // Maintenance orphaned objects based on their retention policy


        // --- Future Child Management --- //
        void attachFutureChildren();
        void addToOrphanList(const DisplayObject orphan);
        void addToFutureChildrenList(const DisplayObject child, const DisplayObject parent,
            bool useWorld=false, int worldX=0, int worldY=0);

        // --- Utility Methods --- //
        std::vector<std::string> listDisplayObjectNames() const;
        // std::vector<std::string> listResourceNames() const;
        void clear();
        void printObjectRegistry() const;

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

        std::vector<DisplayObject> orphanList_;
        struct futureChild 
        {
            DisplayObject child;
            DisplayObject parent;
            bool preserveWorldPosition;
            int dragStartWorldX;
            int dragStartWorldY;
        };            
        std::vector<futureChild> futureChildrenList_;


    };
    

}


