// SDOM_Handle.hpp
#pragma once

#include <SDOM/SDOM.hpp>


// NOTE: this ~= "DomHandle(getName(), getType())"


namespace SDOM
{
    class Core;
    class Factory;

    template<typename T>
    class Handle
    {
    public:

        Handle() = default;
        Handle(const std::string& name, const std::string& type) : name_(name), type_(type) {}
        Handle(const Handle& other)
            : name_(other.name_), type_(other.type_) {}

        // construction from nullptr
        Handle(std::nullptr_t) { reset(); }

        T* get() const;

        T& operator*() const { return *get(); }
        T* operator->() const { return get(); }

        operator bool() const { return get() != nullptr; }
        bool operator==(const Handle& other)  const { return name_ == other.name_; }
        bool operator!=(const Handle& other)  const { return name_ != other.name_; }
        bool operator<(const Handle& other)   const { return name_ < other.name_; }
        bool operator>(const Handle& other)   const { return name_ > other.name_; }
        bool operator<=(const Handle& other)  const { return name_ <= other.name_; }
        bool operator>=(const Handle& other)  const { return name_ >= other.name_; }

        Handle& operator=(const Handle& other) {
            if (this != &other) {
                // Copy members here
                name_ = other.name_;
                type_ = other.type_;
                // Copy any other relevant members
            }
            return *this;
        }
        // Handle& operator=(T* dispObj) {
        //     set(dispObj);
        //     return *this;
        // }
        // Handle& operator=(std::string objName) {
        //     set(objName);
        //     return *this;
        // }        

        // Allow assignment from nullptr
        Handle& operator=(std::nullptr_t) 
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
        std::string getType() const { return type_; }

        std::string str() const {
            std::ostringstream oss;
            oss << "name: '" << getName() << "' type: `" << getType() << "` " << get();
            return oss.str();
        }

        const char* c_str() const {
            // Store the formatted string in a member variable to keep it alive
            formatted_ = str();
            return formatted_.c_str();
        }

        // T* dispObj() const {
        //     return dynamic_cast<T*>(get());
        // }

        // // Convenience setter
        // void set(T* obj) {
        //     if (!obj) {
        //         *this = Handle(); // Null handle
        //         return;
        //     }
        //     Handle handle = getFactory().getHandle(obj->getName());
        //     if (handle) {
        //         *this = handle;
        //     } else {
        //         ERROR("Handle::set(): No resource found with name '" + obj->getName() + "'");
        //         *this = Handle(); // Or handle error as needed
        //     }
        // }
        // void set(std::string name) {
        //     Handle handle = getFactory().getHandle(name);
        //     if (handle) {
        //         *this = handle;
        //     } else {
        //         ERROR("Handle::set(): No resource found with name '" + name + "'");
        //         *this = Handle(); // Or handle error as needed
        //     }
        // }

        inline static Factory* factory_ = nullptr;

    private:
        // Factory::Factory() { Handle<T>::factory_ = this; }
        friend Core;
        friend Factory;

        std::string name_;
        std::string type_;

        mutable std::string formatted_; // to keep the formatted string alive for c_str()

    }; // END class Handle

    class IDisplayObject;
    class IResourceObject;

    using DomHandle = Handle<IDisplayObject>;
    using ResHandle = Handle<IResourceObject>;

} // END namespace SDOM