// SDOM_DisplayObject.hpp

#pragma once

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDataObject.hpp>

// NOTE: this ~= "DisplayObject(getName(), getType())"

namespace SDOM
{
    class Core;
    class Factory;
    class IDisplayObject;

    class DisplayObject : public IDataObject
    {
        using SUPER = IDataObject;


    public:

        DisplayObject();
        DisplayObject(const std::string& name, const std::string& type) : name_(name), type_(type) {}
        DisplayObject(const DisplayObject& other)
            : name_(other.name_), type_(other.type_) {}

        // construction from nullptr
        DisplayObject(std::nullptr_t) { reset(); }

        virtual ~DisplayObject();

        // virtual methods from IDataObject
        virtual bool onInit() override { return true; }
        virtual void onQuit() override {}
        virtual bool onUnitTest() override { return true; }

        IDisplayObject* get() const
        {
            if (!factory_) return nullptr;
            return factory_->getDomObj(name_);
        }

        template<typename T>
        T* as() const
        {
            if (!factory_) return nullptr;
            return dynamic_cast<T*>(factory_->getDomObj(name_));
        }
    
        // Convenience: get raw IDisplayObject pointer (non-owning), or nullptr if not available
        IDisplayObject& operator*() const { return *get(); }
        IDisplayObject* operator->() const { return get(); }
        operator bool() const { return get() != nullptr; }

        DisplayObject& operator=(const DisplayObject& other) {
            if (this != &other) {
                name_ = other.name_;
                type_ = other.type_;
            }
            return *this;
        }

        // // Allow assignment from nullptr
        // DisplayObject& operator=(std::nullptr_t) 
        //     return *this;
        // Comparison operators
        bool operator==(std::nullptr_t) const { return get() == nullptr; }
        bool operator!=(std::nullptr_t) const { return get() != nullptr; }
        bool operator==(const DisplayObject& other) const { return name_ == other.name_ && type_ == other.type_; }
        bool operator!=(const DisplayObject& other) const { return !(*this == other); }
        // }

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

        // --- LUA Wrapper Functions --- //

        IDisplayObject* get_lua(DisplayObject* self) const { return self->get(); }
        bool isValid_lua(DisplayObject* self) const { return self->isValid(); }
        std::string getName_lua(DisplayObject* self) const { return self->getName(); }
        std::string getType_lua(DisplayObject* self) const { return self->getType(); }


    public:
        inline static Factory* factory_ = nullptr;
    private:
        // Factory::Factory() { DisplayObject<T>::factory_ = this; }
        friend Core;
        friend Factory;

        std::string name_;
        std::string type_;

        mutable std::string formatted_; // to keep the formatted string alive for c_str()

        
    protected:
        // --- LUA Registration --- //
        // virtual void _registerLua(const std::string& typeName, sol::state_view lua);
        sol::usertype<DisplayObject> objHandleType_;

    private:
        // Resolve a Lua child spec (string, DisplayObject, or table{ child=... | name=... })
        static DisplayObject resolveChildSpec(const sol::object& spec);

        virtual void _registerLua(const std::string& typeName, sol::state_view lua) override {} // Deprecated

        virtual void _registerDisplayObject(const std::string& typeName, sol::state_view lua) override;

    }; // end class DisplayObject

} // namespace SDOM