// SDOM_Factory.hpp

#pragma once
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>

namespace SDOM 
{
    class ResourceHandle; // forward declaration

    struct TypeCreators 
    {
        std::function<std::unique_ptr<IResourceObject>(const Json&)> fromJson;
        // Accept base InitStruct by reference
        std::function<std::unique_ptr<IResourceObject>(const IDisplayObject::InitStruct&)> fromInitStruct;
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
        virtual bool onUnitTest() override { return true; }

        // Register a resource type with a creation function
        using Creator = std::function<std::unique_ptr<IResourceObject>(const Json&)>;

        // void registerType(const std::string& typeName, Creator creator);
        void registerType(const std::string& typeName, const TypeCreators& creators);


        // JSON based object creator
        ResourceHandle create(const std::string& typeName, const Json& config);
        // String (JSON text) based object creator
        ResourceHandle create(const std::string& typeName, const std::string& jsonStr);
        // InitStruct based object creator
        ResourceHandle create(const std::string& typeName, const IDisplayObject::InitStruct& init);



        // Example of Type-safe access:
        //      ResourceHandle ptr("mainStage", "Stage");
        //      Stage* stage = ptr.as<Stage>();
        IResourceObject* getResource(const std::string& name);

        // Example Usage:
        // ResourceHandle ptr = factory.getResourcePtr("mainStage");
        // if (ptr) {
        //     Stage* stage = dynamic_cast<Stage*>(ptr.get());
        //     // Use stage...
        // }
        ResourceHandle getResourcePtr(const std::string& name);



        void addResource(const std::string& name, 
            std::unique_ptr<IResourceObject> resource);

        void removeResource(const std::string& name);


    private:
        std::unordered_map<std::string, std::unique_ptr<IResourceObject>> resources_;
        std::unordered_map<std::string, TypeCreators> creators_;


        // std::vector<ResourceHandle> orphanList_;
        // struct futureChild 
        // {
        //     ResourceHandle child;
        //     ResourceHandle parent;
        //     bool preserveWorldPosition; // was bool useWorld;
        //     int dragStartWorldX;
        //     int dragStartWorldY;
        // };
        // std::vector<futureChild> futureChildren_;
    };

}