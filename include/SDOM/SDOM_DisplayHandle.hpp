// SDOM_DisplayHandle.hpp

#pragma once

// #include <sol/sol.hpp>
// #include <string>
// #include <sstream>
// #include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDataObject.hpp>

// NOTE: this ~= "DisplayHandle(getName(), getType())"





namespace SDOM
{
    class Core;
    class Factory;
    class IDisplayObject;

    class DisplayHandle : public IDataObject
    {
        using SUPER = IDataObject;

    public:

        DisplayHandle();
        DisplayHandle(const std::string& name, const std::string& type) : name_(name), type_(type) {}
        DisplayHandle(const DisplayHandle& other)
            : name_(other.name_), type_(other.type_) {}

        // Provide explicit copy-assignment to avoid implicitly-declared
        // deprecated assignment operator warnings when a user-provided
        // copy constructor exists.
        DisplayHandle& operator=(const DisplayHandle& other) {
            if (this != &other) {
                name_ = other.name_;
                type_ = other.type_;
            }
            return *this;
        }

        // Provide move-assignment as well for completeness
        DisplayHandle& operator=(DisplayHandle&& other) noexcept {
            if (this != &other) {
                name_ = std::move(other.name_);
                type_ = std::move(other.type_);
            }
            return *this;
        }

        // construction from nullptr
        DisplayHandle(std::nullptr_t) { reset(); }

        virtual ~DisplayHandle();

        // virtual methods from IDataObject
        virtual bool onInit() override { return true; }
        virtual void onQuit() override {}
        virtual bool onUnitTest() override { return true; }

        IDisplayObject* get() const;

        template<typename T>
        T* as() const
        {
            return dynamic_cast<T*>(get());
        }
    
        // Convenience: get raw IDisplayObject pointer (non-owning), or nullptr if not available
        IDisplayObject& operator*() const { return *get(); }
        IDisplayObject* operator->() const { return get(); }
        operator bool() const { return get() != nullptr; }

        // DisplayHandle& operator=(const DisplayHandle& other) {
        //     if (this != &other) {
        //         name_ = other.name_;
        //         type_ = other.type_;
        //     }
        //     return *this;
        // }

        // // Allow assignment from nullptr
        // DisplayHandle& operator=(std::nullptr_t) 
        //     return *this;
        // Comparison operators
        bool operator==(std::nullptr_t) const { return get() == nullptr; }
        bool operator!=(std::nullptr_t) const { return get() != nullptr; }
        // bool operator==(const DisplayHandle& other) const { return name_ == other.name_ && type_ == other.type_; }
        bool operator==(const DisplayHandle& other) const { return name_ == other.name_; }
        bool operator!=(const DisplayHandle& other) const { return !(*this == other); }
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
        void setName(const std::string& newName) { name_ = newName; }
        void setType(const std::string& newType) { type_ = newType; }

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

        IDisplayObject* get_lua(DisplayHandle* self) const { return self->get(); }
        bool isValid_lua(DisplayHandle* self) const { return self->isValid(); }
        std::string getName_lua(DisplayHandle* self) const { return self->getName(); }
        std::string getType_lua(DisplayHandle* self) const { return self->getType(); }

        // Shared Lua handle usertype helpers (augmentable by base/descendants)
        inline static constexpr const char* LuaHandleName = "DisplayHandle";

        // Ensure the handle usertype exists in this Lua state and return its table.
        static sol::table ensure_handle_table(sol::state_view lua);
    // Ensure per-type binding table exists and return it. Parent optional.
    static sol::table ensure_type_bind_table(sol::state_view lua, const std::string& typeName, const std::string& parentTypeName = "");
        
        // Bind only the minimal, safe helpers if absent (idempotent).
        static void bind_minimal(sol::state_view lua);


    public:
        inline static Factory* factory_ = nullptr;
    private:
        // Factory::Factory() { DisplayHandle<T>::factory_ = this; }
        friend Core;
        friend Factory;

        std::string name_;
        std::string type_;

        mutable std::string formatted_; // to keep the formatted string alive for c_str()

        
    protected:
        // --- LUA Registration --- //
        // virtual void _registerLua(const std::string& typeName, sol::state_view lua);
        sol::usertype<DisplayHandle> objHandleType_;

    protected:
        // Resolve a Lua child spec (string, DisplayHandle, or table{ child=... | name=... })
        static DisplayHandle resolveChildSpec(const sol::object& spec);

        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua) override;

    }; // end class DisplayHandle

} // namespace SDOM