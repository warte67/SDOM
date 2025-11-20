#pragma once
// #include <SDOM/SDOM.hpp>
// #include <SDOM/SDOM_IDisplayObject.hpp>
// #include <SDOM/SDOM_DisplayHandle.hpp>
// #include <SDOM/SDOM_IAssetObject.hpp>
// #include <SDOM/SDOM_AssetHandle.hpp>

#include <string>
#include <memory>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <shared_mutex>
#include <cstdint>
// #include <external/nlohmann/json.hpp>
#include <json.hpp>

// include the concrete interface headers so unique_ptr<T> is instantiated with a complete type
// #include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>   // required for std::unique_ptr<IDisplayObject>
#include <SDOM/SDOM_IAssetObject.hpp>     // required for std::unique_ptr<IAssetObject>
#include <SDOM/SDOM_DataRegistry.hpp>
 

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
        using InitFn = std::function<std::unique_ptr<IDisplayObject>(const IDisplayObject::InitStruct&)>;
        using JsonFn = std::function<std::unique_ptr<IDisplayObject>(const nlohmann::json&)>;

        InitFn fromInitStruct;
        JsonFn fromJson;
    };

    struct AssetTypeCreators 
    {
        using InitFn = std::function<std::unique_ptr<IAssetObject>(const IAssetObject::InitStruct&)>;
        using JsonFn = std::function<std::unique_ptr<IAssetObject>(const nlohmann::json&)>;

        InitFn fromInitStruct;
        JsonFn fromJson;
    };


    // --- Registry Record Types --- //
    // Wrap an owned display object plus metadata (type + stable id)
    struct DisplayRecord
    {
        std::unique_ptr<IDisplayObject> obj;
        std::string type;
        uint64_t id = 0;

        DisplayRecord(std::unique_ptr<IDisplayObject> o = nullptr, const std::string& t = "", uint64_t i = 0)
            : obj(std::move(o)), type(t), id(i) {}
    };

    // Wrap an asset object (shared ownership allowed) plus metadata
    struct AssetRecord
    {
        std::shared_ptr<IAssetObject> obj;
        std::string type;
        uint64_t id = 0;

        AssetRecord(std::shared_ptr<IAssetObject> o = nullptr, const std::string& t = "", uint64_t i = 0)
            : obj(std::move(o)), type(t), id(i) {}
    };

    class Factory final
    {
        friend class Core;  // Core should have direct access to the Factory internals

    public:
        // --- Lifecycle --- //
        Factory();
        ~Factory() = default;
        bool onInit();
        // Owner-controlled lifecycle helpers
        bool startup();
        void shutdown();
        bool isInitialized() const { return initialized_; }
        bool onUnitTest(int frame);

        // --- DataRegistry access --- //
        // Preferred name for clarity
        SDOM::DataRegistry& getDataRegistry() { return registry_; }
        const SDOM::DataRegistry& getDataRegistry() const { return registry_; }
        bool exportBindings(const std::string& outputDir) { return registry_.generateBindings(outputDir); }

        // --- Object Type Registration --- //
        void registerDisplayObjectType(const std::string& typeName, const TypeCreators& creators);
        void registerAssetObjectType(const std::string& typeName, const AssetTypeCreators& creators);

        // --- Object Creation --- //
        // Preferred explicit names
        DisplayHandle createDisplayObject(const std::string& typeName, const IDisplayObject::InitStruct& init);
        DisplayHandle createDisplayObjectFromJson(const std::string& typeName, const nlohmann::json&);

        AssetHandle createAssetObject(const std::string& typeName, const IAssetObject::InitStruct& init);
        AssetHandle createAssetObjectFromJson(const std::string& typeName, const nlohmann::json&);

        // --- Object Lookup --- //
        // Preferred modern names: return raw interface pointers for callers
        IDisplayObject* getDisplayObjectPtr(const std::string& name);
        IAssetObject* getAssetObjectPtr(const std::string& name);
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

        // Helper: find an existing asset by filename (optionally matching type)
        AssetHandle findAssetByFilename(const std::string& filename, const std::string& typeName = "") const;

        // Helper: find a SpriteSheet asset matching filename and sprite dimensions
        AssetHandle findSpriteSheetByParams(const std::string& filename, int spriteW, int spriteH) const;

        // Helper: Unload Asset Objects
        void unloadAllAssetObjects() 
        {
            for (auto& [name, assetEntryPtr] : assetObjects_) 
            {
                if (assetEntryPtr && assetEntryPtr->obj && assetEntryPtr->obj->isLoaded()) 
                {
                    // INFO("Factory::unloadAllAssetObjects: Unloading asset: " << name << " (" << assetEntryPtr->obj->getType() << ")");
                    // Use owner-safe helper to avoid virtual calls from destructors
                    assetEntryPtr->obj->unload();
                }
            }
        }
        // Helper: Reload Asset Objects
        void reloadAllAssetObjects() 
        {
            for (auto& [name, assetEntryPtr] : assetObjects_) 
            {
                if (assetEntryPtr && assetEntryPtr->obj && !assetEntryPtr->obj->isLoaded()) 
                {
                    // INFO("Factory::loadAllAssetObjects: Loading asset: " << name << " (" << assetEntryPtr->obj->getType() << ")");
                    // Use owner-safe helper so ownership semantics are respected
                    assetEntryPtr->obj->load();
                }
            }
        }

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
        
        // Performance timers keyed by object identity. Name snapshots are stored in PerfStats.
        void start_update_timer(const IDisplayObject* obj);
        void stop_update_timer(const IDisplayObject* obj, const std::string* name_snapshot = nullptr);
        void abandon_update_timer(const IDisplayObject* obj);
        void start_render_time(const IDisplayObject* obj);
        void stop_render_time(const IDisplayObject* obj, const std::string* name_snapshot = nullptr);
        void abandon_render_time(const IDisplayObject* obj);
        void report_performance_stats() const; // summary entrypoint (calls the two below)
        void report_update_stats(std::size_t topN = 15) const;
        void report_render_stats(std::size_t topN = 15) const;
        void reset_performance_stats();
        // Clear last-frame deltas before measuring a new frame
        void begin_frame_metrics();

        // Query the last measured deltas (in microseconds) for a given object
        float getLastUpdateDelta(const IDisplayObject* obj) const;
        float getLastRenderDelta(const IDisplayObject* obj) const;
        float getLastUpdateDelta(const std::string& obj_name) const;
        float getLastRenderDelta(const std::string& obj_name) const;
        

    private:
        // initialization guard to make onInit idempotent
        bool initialized_ = false;
        // --- Internal Storage --- //
        // Now store registry entries which own/manage objects and contain a stable id
        std::unordered_map<std::string, std::unique_ptr<DisplayRecord>> displayObjects_;
        // Use shared_ptr inside AssetRecord so multiple registry names can alias the same underlying asset
        std::unordered_map<std::string, std::unique_ptr<AssetRecord>> assetObjects_;

        std::unordered_map<std::string, TypeCreators> creators_;            // are these now needed?
        std::unordered_map<std::string, AssetTypeCreators> assetCreators_;  // are these now needed?

        // --- Performance Tracking Maps --- //
        std::unordered_map<const IDisplayObject*, PerfStats> perf_map;
        std::unordered_map<const IDisplayObject*, std::chrono::steady_clock::time_point> update_start_times;
        std::unordered_map<const IDisplayObject*, std::chrono::steady_clock::time_point> render_start_times;

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

    // --- ID Registry --- //
    // Atomic counter for issuing stable 64-bit ids (0 reserved)
    std::atomic<uint64_t> next_object_id_{1};
    // Map stable id -> handle (separate maps for display and asset handles). Protected by a shared_mutex for concurrent reads.
    std::unordered_map<uint64_t, DisplayHandle> display_id_map_;
    std::unordered_map<uint64_t, AssetHandle> asset_id_map_;
    mutable std::shared_mutex id_map_mutex_;

    // ID registry helpers (private)
    uint64_t registerDisplayObject(const std::string& name, const DisplayHandle& handle);
    DisplayHandle resolveDisplayObject(uint64_t id) const;
    void unregisterDisplayObject(uint64_t id);

    uint64_t registerAssetObject(const std::string& name, const AssetHandle& handle);
    AssetHandle resolveAssetObject(uint64_t id) const;
    void unregisterAssetObject(uint64_t id);

    // --- DataRegistry owned by Factory --- //
    SDOM::DataRegistry registry_;

        // Calls TTF_CloseFont on all loaded TTFAsset objects
        void closeAllTruetypeAssets_();

    };
    

}
