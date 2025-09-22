// SDOM_ResourceHandle.hpp
#pragma once

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>

// NOTE: this ~= "ResourceHandle(getName(), getType())"


namespace SDOM
{
    class Core;
    
    class ResourceHandle
    {
    public:

        ResourceHandle() = default;
        ResourceHandle(const std::string& name, const std::string& type) : name_(name), type_(type) {}
        ResourceHandle(const ResourceHandle& other)
            : name_(other.name_), type_(other.type_) {}
        ResourceHandle& operator=(const ResourceHandle& other) {
            if (this != &other) {
                // Copy members here
                name_ = other.name_;
                type_ = other.type_;
                // Copy any other relevant members
            }
            return *this;
        }
        // construction from nullptr
        ResourceHandle(std::nullptr_t) { reset(); }

        IResourceObject* get() const;

        // Gotta love circular includes...  NOT!!!
        // template<typename T>
        // T* as() const;

        IResourceObject& operator*() const { return *get(); }
        IResourceObject* operator->() const { return get(); }

        operator bool() const { return get() != nullptr; }
        bool operator==(const ResourceHandle& other)  const { return name_ == other.name_; }
        bool operator!=(const ResourceHandle& other)  const { return name_ != other.name_; }
        bool operator<(const ResourceHandle& other)   const { return name_ < other.name_; }
        bool operator>(const ResourceHandle& other)   const { return name_ > other.name_; }
        bool operator<=(const ResourceHandle& other)  const { return name_ <= other.name_; }
        bool operator>=(const ResourceHandle& other)  const { return name_ >= other.name_; }

        // ResourceHandle& operator=(const ResourceHandle& other)
        // {
        //     if (this != &other) {
        //         name_ = other.name_;
        //     }
        //     return *this;
        // }

        // Allow assignment from nullptr
        ResourceHandle& operator=(std::nullptr_t) 
        {
            reset();
            return *this;
        }

        void reset() {
            // Clear internal state (pointer, name, etc.)
            // Example:
            // ptr_ = nullptr;
            name_.clear();
            // ...other members...
        }
        std::string getName() const { return name_; }

    private:
        // Factory::Factory() { ResourceHandle<IResourceObject>::factory_ = this; }
        friend Core;
        friend Factory;

        inline static Factory* factory_ = nullptr;
        std::string name_;
        std::string type_;

    }; // END class ResourceHandle


    // template<typename T>
    // T* ResourceHandle::as() const 
    // {
    //     return dynamic_cast<T*>(get());
    // }
} // END namespace SDOM