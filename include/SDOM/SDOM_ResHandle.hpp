// SDOM_ResHandle.hpp
#pragma once

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDataObject.hpp>


// NOTE: this ~= "ResHandle(getName(), getType())"


namespace SDOM
{
    class Core;
    class Factory;

    class IResourceObject;

    class ResHandle : public IDataObject
    {
        using SUPER = IDataObject;

    public:

        ResHandle() = default;
        ResHandle(const std::string& name, const std::string& type) : name_(name), type_(type) {}
        ResHandle(const ResHandle& other)
            : name_(other.name_), type_(other.type_) {}

        // construction from nullptr
        ResHandle(std::nullptr_t) { reset(); }
        
        // virtual methods from IDataObject
        virtual bool onInit() override { return true; }
        virtual void onQuit() override {}
        virtual bool onUnitTest() override { return true; }        

        IResourceObject* get() const;

        IResourceObject& operator*() const { return *get(); }
        IResourceObject* operator->() const { return get(); }
        operator bool() const { return get() != nullptr; }
        bool operator==(const ResHandle& other)  const { return name_ == other.name_ && type_ == other.type_ ; }
        bool operator!=(const ResHandle& other)  const { return !(*this == other); }
        
        ResHandle& operator=(const ResHandle& other) {
            if (this != &other) {
                name_ = other.name_;
                type_ = other.type_;
            }
            return *this;
        }
        // Allow assignment from nullptr
        ResHandle& operator=(std::nullptr_t) 
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
       
        bool isValid() const {
            return get() != nullptr;
        }          

        inline static Factory* factory_ = nullptr;

    private:
        // Factory::Factory() { ResHandle<T>::factory_ = this; }
        friend Core;
        friend Factory;

        std::string name_;
        std::string type_;

        mutable std::string formatted_; // to keep the formatted string alive for c_str()

        // --- LUA Registration --- //
    protected:
        virtual void _registerLua_Usertype(sol::state_view lua) override;
        virtual void _registerLua_Properties(sol::state_view lua) override;
        virtual void _registerLua_Commands(sol::state_view lua) override;
        virtual void _registerLua_Meta(sol::state_view lua) override;
        virtual void _registerLua_Children(sol::state_view lua) override;
        virtual void _registerLua_All(sol::state_view lua) override;
        
    }; // END class ResHandle

} // END namespace SDOM