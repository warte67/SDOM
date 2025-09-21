// SDOM_Factory.hpp

#pragma once
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>

namespace SDOM 
{
        
    class Factory final : public IDataObject
    {
    public:
        Factory() = default;
        virtual ~Factory() = default;

        // IDataObject overrides
        virtual bool onInit() override { return true; }
        virtual void onQuit() override {}
        virtual bool onUnitTest() override { return true; }

        template<typename T>
        T* getResource(const std::string& name) {
            // Example: dynamic_cast to requested type
            auto it = resources_.find(name);
            if (it != resources_.end()) {
                return dynamic_cast<T*>(it->second.get());
            }
            return nullptr;
        }

        void addResource(const std::string& name, std::unique_ptr<IResourceObject> resource) {
            resources_[name] = std::move(resource);
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<IResourceObject>> resources_;
    };










    
    /**
     * @class resource_ptr
     * @brief Smart pointer for referencing Factory-managed resources by name.
     * @details
     * Template class to hold a reference to a resource of type T managed by the Factory.
     * The resource is looked up by name when accessed. If the resource is not found,
     * the pointer evaluates to false.
     * 
     * Usage:
     *   resource_ptr<IDisplayObject> ptr("mainStage");
     *   if (ptr) { ptr->onRender(); }
     */
    template<typename T>
    class resource_ptr
    {
    public:
        /**
         * @brief Default constructor. Creates a null resource pointer.
         */
        resource_ptr() = default;

        /**
         * @brief Constructs a resource pointer referencing a resource by name.
         * @param name Name of the resource.
         */
        resource_ptr(const std::string& name) : name_(name) {}

        /**
         * @brief Gets the raw pointer to the resource.
         * @return Pointer to resource of type T, or nullptr if not found.
         */
        T* get() const
        {
            if (!factory_) return nullptr;
            return factory_->template getResource<T>(name_);
        }

        /**
         * @brief Dereference operator.
         * @return Reference to the resource.
         */
        T& operator*() const { return *get(); }

        /**
         * @brief Arrow operator for member access.
         * @return Pointer to the resource.
         */
        T* operator->() const { return get(); }

        /**
         * @brief Boolean conversion operator.
         * @return True if the resource exists, false otherwise.
         */
        operator bool() const { return get() != nullptr; }

        /**
         * @brief Equality comparison operator.
         * @param other Another resource_ptr.
         * @return True if both reference the same resource name.
         */
        bool operator==(const resource_ptr& other)  const { return name_ == other.name_; }

        /**
         * @brief Inequality comparison operator.
         * @param other Another resource_ptr.
         * @return True if resource names differ.
         */
        bool operator!=(const resource_ptr& other)  const { return name_ != other.name_; }

        /**
         * @brief Less-than comparison operator (by resource name).
         * @param other Another resource_ptr.
         * @return True if this resource name is lexicographically less.
         */
        bool operator<(const resource_ptr& other)   const { return name_ < other.name_; }

        /**
         * @brief Greater-than comparison operator (by resource name).
         * @param other Another resource_ptr.
         * @return True if this resource name is lexicographically greater.
         */
        bool operator>(const resource_ptr& other)   const { return name_ > other.name_; }

        /**
         * @brief Less-than-or-equal comparison operator (by resource name).
         * @param other Another resource_ptr.
         * @return True if this resource name is lexicographically less or equal.
         */
        bool operator<=(const resource_ptr& other)  const { return name_ <= other.name_; }

        /**
         * @brief Greater-than-or-equal comparison operator (by resource name).
         * @param other Another resource_ptr.
         * @return True if this resource name is lexicographically greater or equal.
         */
        bool operator>=(const resource_ptr& other)  const { return name_ >= other.name_; }

        /**
         * @brief Assignment operator.
         * @param other Another resource_ptr.
         * @return Reference to this resource_ptr.
         */
        resource_ptr& operator=(const resource_ptr& other)
        {
            if (this != &other) {
                name_ = other.name_;
            }
            return *this;
        }

        /**
         * @brief Gets the resource name referenced by this pointer.
         * @return Resource name as a string.
         */
        std::string getName() const { return name_; }

    private:
        // Factory::Factory() { resource_ptr<IResourceObject>::factory_ = this; }
        friend Core;
        inline static Factory* factory_ = nullptr; ///< Pointer to Factory instance (must be set before use)
        std::string name_; ///< Name of the resource referenced

    }; // END class resource_ptr

}