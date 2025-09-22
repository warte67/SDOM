// SDOM_Factory.hpp

#pragma once
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>

namespace SDOM 
{
    class resource_ptr; // forward declaration
        
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
        void registerType(const std::string& typeName, Creator creator);





        // Example of Type-safe access:
        //      resource_ptr ptr("mainStage", "Stage");
        //      Stage* stage = ptr.as<Stage>();
        IResourceObject* getResource(const std::string& name) 
        {
            auto it = resources_.find(name);
            if (it != resources_.end()) 
            {
                return it->second.get();
            }
            return nullptr;
        }

        // Example Usage:
        // resource_ptr ptr = factory.getResourcePtr("mainStage");
        // if (ptr) {
        //     Stage* stage = dynamic_cast<Stage*>(ptr.get());
        //     // Use stage...
        // }
        resource_ptr getResourcePtr(const std::string& name);



        void addResource(const std::string& name, std::unique_ptr<IResourceObject> resource) {
            resources_[name] = std::move(resource);
        }
        
        void removeResource(const std::string& name) {
            resources_.erase(name);
        }


    private:
        std::unordered_map<std::string, std::unique_ptr<IResourceObject>> resources_;
        std::unordered_map<std::string, Creator> creators_;



        // std::vector<std::shared_ptr<IDisplayObject>> orphanList_;
        // struct futureChild 
        // {
        //     std::shared_ptr<IDisplayObject> child;
        //     std::shared_ptr<IDisplayObject> parent;
        //     bool useWorld;
        //     int worldX;
        //     int worldY;
        // };
        // std::vector<futureChild> futureChildren_;
    };

}