#pragma once
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>

namespace SDOM 
{
    class Stage;

    // --- Type Creation Structs --- //
    struct TypeCreators 
    {
        std::function<std::unique_ptr<IDisplayObject>(const sol::table&)> fromLua;
        std::function<std::unique_ptr<IDisplayObject>(const IDisplayObject::InitStruct&)> fromInitStruct;
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

        // --- Type Registration --- //
        void registerDomType(const std::string& typeName, const TypeCreators& creators);
        // void registerResType(const std::string& typeName, const TypeCreators& creators);

        // --- Object Creation --- //
        DomHandle create(const std::string& typeName, const sol::table& config);
        DomHandle create(const std::string& typeName, const IDisplayObject::InitStruct& init);        
        DomHandle create(const std::string& typeName, const std::string& luaScript);

        // --- Object Lookup --- //
        IDisplayObject* getDomObj(const std::string& name);
        IResourceObject* getResObj(const std::string& name);
        DomHandle getDomHandle(const std::string& name);
        // ResHandle getResHandle(const std::string& name);
        DomHandle getStageHandle();

        // --- Display Object Management --- //
        void addDisplayObject(const std::string& name, std::unique_ptr<IDisplayObject> displayObject); // Consider refactoring/removal
        void destroyDisplayObject(const std::string& name);

        // --- Orphan Management --- //
        int countOrphanedDisplayObjects() const;
        std::vector<DomHandle> getOrphanedDisplayObjects();
        void destroyOrphanedDisplayObjects();
        void detachOrphans();

        // --- Future Child Management --- //
        void attachFutureChildren();
        void addToOrphanList(const DomHandle orphan);
        void addToFutureChildrenList(const DomHandle child, const DomHandle parent,
            bool useWorld=false, int worldX=0, int worldY=0);

        // --- Utility Methods --- //
        std::vector<std::string> listDisplayObjectNames() const;
        // std::vector<std::string> listResourceNames() const;
        void clear();
        void printRegistry() const;

        // --- Lua Integration --- //
        void initFromLua(const sol::table& lua);
        void processResource(const sol::table& resource);

    private:
        // --- Internal Storage --- //
        std::unordered_map<std::string, std::unique_ptr<IDisplayObject>> displayObjects_;
        // std::unordered_map<std::string, std::unique_ptr<IResourceObject>> resources_;
        std::unordered_map<std::string, TypeCreators> creators_;

        // --- Orphan & Future Child Lists --- //
        std::vector<DomHandle> orphanList_;
        struct futureChild 
        {
            DomHandle child;
            DomHandle parent;
            bool preserveWorldPosition;
            int dragStartWorldX;
            int dragStartWorldY;
        };
        std::vector<futureChild> futureChildrenList_;

        // // --- Object Type Registry --- //
        // struct TypeRegistryEntry
        // {
        //     std::string typeName;
        //     std::unordered_map<std::string, IDataObject::Getter> getters;
        //     std::unordered_map<std::string, IDataObject::Setter> setters;
        //     std::unordered_map<std::string, IDataObject::Command> commands;
        // };
        // std::unordered_map<std::string, TypeRegistryEntry> typeRegistry_;        
    };

}


