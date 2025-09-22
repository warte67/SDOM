// SDOM_ResourcePointer.hpp
#pragma once

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>


namespace SDOM
{
    class resource_ptr
    {
    public:

        resource_ptr() = default;
        resource_ptr(const std::string& name, const std::string& type) : name_(name), type_(type) {}
        resource_ptr(const resource_ptr& other)
            : name_(other.name_), type_(other.type_) {}


        IResourceObject* get() const;

        // template<typename T>
        // T* as() const;

        IResourceObject& operator*() const { return *get(); }
        IResourceObject* operator->() const { return get(); }

        operator bool() const { return get() != nullptr; }
        bool operator==(const resource_ptr& other)  const { return name_ == other.name_; }
        bool operator!=(const resource_ptr& other)  const { return name_ != other.name_; }
        bool operator<(const resource_ptr& other)   const { return name_ < other.name_; }
        bool operator>(const resource_ptr& other)   const { return name_ > other.name_; }
        bool operator<=(const resource_ptr& other)  const { return name_ <= other.name_; }
        bool operator>=(const resource_ptr& other)  const { return name_ >= other.name_; }

        resource_ptr& operator=(const resource_ptr& other)
        {
            if (this != &other) {
                name_ = other.name_;
            }
            return *this;
        }

        std::string getName() const { return name_; }

    private:
        // Factory::Factory() { resource_ptr<IResourceObject>::factory_ = this; }
        friend Core;
        friend Factory;

        inline static Factory* factory_ = nullptr;
        std::string name_;
        std::string type_;

    }; // END class resource_ptr


    // template<typename T>
    // T* resource_ptr::as() const 
    // {
    //     return dynamic_cast<T*>(get());
    // }
} // END namespace SDOM