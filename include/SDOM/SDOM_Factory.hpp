// SDOM_Factory.hpp

#pragma once
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>

namespace SDOM 
{
    class Stage;

    struct TypeCreators 
    {
        std::function<std::unique_ptr<IDisplayObject>(const Json&)> fromJson;
        // Accept base InitStruct by reference
        std::function<std::unique_ptr<IDisplayObject>(const IDisplayObject::InitStruct&)> fromInitStruct;
    };
        
    class Factory final : public IDataObject
    {
        friend class Core;  // Core should have direct access to the Factory internals

    public:
        Factory();
        virtual ~Factory() = default;

        // IDataObject overrides
        virtual bool onInit() override { return true; }
        virtual void onQuit() override {}
        virtual bool onUnitTest() override;

        // ----- Creation Methods -----

        // Register a resource type with a creation function
        using Creator = std::function<std::unique_ptr<IResourceObject>(const Json&)>;

        // void registerType(const std::string& typeName, Creator creator);
        void registerType(const std::string& typeName, const TypeCreators& creators);
        // JSON based object creator
        DomHandle create(const std::string& typeName, const Json& config);
        // String (JSON text) based object creator
        DomHandle create(const std::string& typeName, const std::string& jsonStr);
        // InitStruct based object creator
        DomHandle create(const std::string& typeName, const IDisplayObject::InitStruct& init);

        // ----- Resource Management -----


        IResourceObject* getResObj(const std::string& name);
        IDisplayObject* getDomObj(const std::string& name);

        // Example Usage:
        // DomHandle ptr = factory.getDomHandle("mainStage");
        // if (ptr) {
        //     Stage* stage = dynamic_cast<Stage*>(ptr.get());
        //     // Use stage...
        // }
        DomHandle getDomHandle(const std::string& name);

        DomHandle getStageHandle();

        void addDisplayObject(const std::string& name, 
            std::unique_ptr<IDisplayObject> displayObject);
        void removeDisplayObject(const std::string& name);

        // ----- Utility Methods -----
        std::vector<std::string> listDisplayObjectNames() const;
        // std::vector<std::string> listResourceNames() const;
        void clear();
        void printRegistry() const;

        // ----- Orphan and Future Child Management -----

        // Detach all objects in the orphan list
        void detachOrphans();

        // Attach all future children to their respective parents
        void attachFutureChildren();

        // Add a child node to the orphan list
        void addToOrphanList(const DomHandle orphan);

        // Add a future child to the future children list
        void addToFutureChildrenList(const DomHandle child, const DomHandle parent,
            bool useWorld=false, int worldX=0, int worldY=0);

        
        // ----- JSON -----
        
        void initFromJson(const Json& json);
        void processResource(const Json& resource);
            

    private:
        std::unordered_map<std::string, std::unique_ptr<IDisplayObject>> displayObjects_;
        // std::unordered_map<std::string, std::unique_ptr<IResourceObject>> resources_;
        std::unordered_map<std::string, TypeCreators> creators_;


        std::vector<DomHandle> orphanList_;
        struct futureChild 
        {
            DomHandle child;
            DomHandle parent;
            bool preserveWorldPosition; // was bool useWorld;
            int dragStartWorldX;
            int dragStartWorldY;
        };
        std::vector<futureChild> futureChildrenList_;
    };

}