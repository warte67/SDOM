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
    bool isInitialized() const { return initialized_; }
        virtual void onQuit() override {}
        virtual bool onUnitTest(int frame) override;

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
        void destroyAssetObject(const std::string& name);

        // --- Orphan Management --- //
        int countOrphanedDisplayObjects() const;
        std::vector<DisplayHandle> getOrphanedDisplayObjects();
        void destroyOrphanedDisplayObjects();
        void detachOrphans();   // Detach all orphans in the orphan list from their parents.
        void collectGarbage();  // Maintenance orphaned objects based on their retention policy


        // --- Future Child Management --- //
        void attachFutureChildren();
        void addToOrphanList(const DisplayHandle orphan);
        void addToFutureChildrenList(const DisplayHandle child, const DisplayHandle parent,
            bool useWorld=false, int worldX=0, int worldY=0);

        // --- Utility Methods --- //
        std::vector<std::string> getDisplayObjectNames() const;
        void clear();
        void printObjectRegistry() const;
        void printAssetRegistry() const;   // NEW, needs LUA bindings
        void printAssetTree() const;       // Print a dependency tree of assets (BitmapFont->SpriteSheet->Texture, TruetypeFont->TTFAsset)
        void printAssetTreeGrouped() const; // Grouped view with Texture/TTFAsset roots and dependents indented like printTree()

        // --- Internal Lua Integration --- //
        void initFromLua(const sol::table& lua);
        // void initLuaProcessResource(const sol::table& resource);

        // Helper: find an existing asset by filename (optionally matching type)
        AssetHandle findAssetByFilename(const std::string& filename, const std::string& typeName = "") const;

        // Helper: find a SpriteSheet asset matching filename and sprite dimensions
        AssetHandle findSpriteSheetByParams(const std::string& filename, int spriteW, int spriteH) const;

        // Helper: Unload Asset Objects
        void unloadAllAssetObjects() 
        {
            for (auto& [name, assetPtr] : assetObjects_) 
            {
                if (assetPtr && assetPtr->isLoaded()) 
                {
                    // INFO("Factory::unloadAllAssetObjects: Unloading asset: " << name << " (" << assetPtr->getType() << ")");
                    assetPtr->onUnload();
                }
            }
        }
        // Helper: Reload Asset Objects
        void reloadAllAssetObjects() 
        {
            for (auto& [name, assetPtr] : assetObjects_) 
            {
                if (assetPtr && !assetPtr->isLoaded()) 
                {
                    // INFO("Factory::loadAllAssetObjects: Loading asset: " << name << " (" << assetPtr->getType() << ")");
                    assetPtr->onLoad();
                }
            }
        }



        // IDEA: continiually track before and after onUpdate() and onRender() 


        // --- Performance Test Helpers --- //

        struct PerfStats {
            uint64_t update_calls = 0;
            uint64_t render_calls = 0;
            // Accumulate at nanosecond precision to avoid truncation to 0us
            std::chrono::nanoseconds update_time_ns{0};
            std::chrono::nanoseconds render_time_ns{0};
            // Track most recent deltas per object (nanoseconds)
            std::chrono::nanoseconds last_update_ns{0};
            std::chrono::nanoseconds last_render_ns{0};
        };
        
        void start_update_timer(const std::string& objName);
        void stop_update_timer(const std::string& objName);
        void start_render_time(const std::string& objName);
        void stop_render_time(const std::string& objName);
        void report_performance_stats() const; // summary entrypoint (calls the two below)
        void report_update_stats(std::size_t topN = 15) const;
        void report_render_stats(std::size_t topN = 15) const;
        void reset_performance_stats();

        // Query the last measured deltas (in microseconds) for a given object
        float getLastUpdateDelta(const IDisplayObject* obj) const;
        float getLastRenderDelta(const IDisplayObject* obj) const;
        float getLastUpdateDelta(const std::string& obj_name) const;
        float getLastRenderDelta(const std::string& obj_name) const;
        

    private:
        // initialization guard to make onInit idempotent
        bool initialized_ = false;
        // --- Internal Storage --- //
        std::unordered_map<std::string, std::unique_ptr<IDisplayObject>> displayObjects_;
        // Use shared_ptr so multiple registry names can alias the same underlying asset
        std::unordered_map<std::string, std::shared_ptr<IAssetObject>> assetObjects_;
        std::unordered_map<std::string, TypeCreators> creators_;
        std::unordered_map<std::string, AssetTypeCreators> assetCreators_;

        // --- Performance Tracking Maps --- //
        std::unordered_map<std::string, PerfStats> perf_map;
        std::unordered_map<std::string, std::chrono::steady_clock::time_point> update_start_times;
        std::unordered_map<std::string, std::chrono::steady_clock::time_point> render_start_times;

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

        // Calls TTF_CloseFont on all loaded TTFAsset objects
        void closeAllTruetypeAssets_();

    };
    

}
