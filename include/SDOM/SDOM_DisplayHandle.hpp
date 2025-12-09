/**
 * @class SDOM::DisplayHandle
 * @brief A safe, name-based reference proxy to display objects managed by the SDOM Core.
 *
 * DisplayHandle replaces traditional pointers in SDOM’s API with a lightweight, 
 * copyable reference type that resolves objects by name and type through the global
 * Factory registry. It ensures **no direct ownership**, **no double-frees**, and 
 * **graceful fallback** when a target object is missing.
 *
 * ---
 * ### 🧩 Key Features
 * - **Pointer-Safe:** Never owns or deletes the target object; resolution is deferred to Core.
 * - **Self-Validating:** `get()` returns `nullptr` if the target is destroyed or unresolved.
 * - **Copy/Move Friendly:** Handles can be freely copied, passed, or stored; all copies remain valid references.
 * - **Recoverable by Name:** Even if all handles are dropped, the object can be fetched again via `Core::getDisplayObject(name)`.
 * - **Lua Compatible:** Provides matching bindings (`getName`, `getType`, `isValid`) via sol2 for scripting integration.
 * - **Debug Friendly:** `str()` and `c_str()` produce clear, introspectable summaries for diagnostics.
 *
 * ---
 * ### 🧠 Lifetime Model
 * - DisplayHandles are **non-owning**.
 * - Actual object deletion occurs only through Core or Factory teardown routines.
 * - When an object is removed, any existing DisplayHandles resolve to `nullptr`.
 * - Lost handles can always be recovered by name until the object is explicitly destroyed or garbage-collected.
 *
 * ---
 * ### 🪄 Example
 * ```cpp
 * DisplayHandle box("blueishBox", "Box");
 * if (box.isValid()) {
 *     box->setVisible(true);
 *     std::cout << box.str() << std::endl;
 * }
 * ```
 *
 * ---
 * @see SDOM::AssetHandle
 * @see SDOM::Core
 * @see SDOM::Factory
 */
#pragma once

// #include <sol/sol.hpp>
// #include <string>
// #include <sstream>
// #include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDataObject.hpp>
#include <cstdint>

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
        DisplayHandle(const std::string& name, const std::string& type, uint64_t id = 0)
            : name_(name), type_(type), id_(id) {}
        DisplayHandle(const DisplayHandle& other)
            : name_(other.name_), type_(other.type_), id_(other.id_) {}

        // Provide explicit copy-assignment to avoid implicitly-declared
        // deprecated assignment operator warnings when a user-provided
        // copy constructor exists.
        DisplayHandle& operator=(const DisplayHandle& other) {
            if (this != &other) {
                name_ = other.name_;
                type_ = other.type_;
                id_   = other.id_;
            }
            return *this;
        }

        // Provide move-assignment as well for completeness
        DisplayHandle& operator=(DisplayHandle&& other) noexcept {
            if (this != &other) {
                name_ = std::move(other.name_);
                type_ = std::move(other.type_);
                id_   = other.id_;
            }
            return *this;
        }

        // construction from nullptr
        DisplayHandle(std::nullptr_t) { reset(); }

        virtual ~DisplayHandle();

        // virtual methods from IDataObject
        bool onInit() override { return true; }
        void onQuit() override {}
        bool onUnitTest(int frame) override { (void)frame; return true; }

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
            id_ = 0;
            // ...other members...
        }
        std::string getName() const { return name_; }
        std::string getType() const { return type_; }
        void setName(const std::string& newName) { name_ = newName; }
        void setType(const std::string& newType) { type_ = newType; }
        uint64_t getId() const { return id_; }
        void setId(uint64_t newId) { id_ = newId; }

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

    public:
        inline static Factory* factory_ = nullptr;
    private:
        // Factory::Factory() { DisplayHandle<T>::factory_ = this; }
        friend Core;
        friend Factory;

        std::string name_;
        std::string type_;
        uint64_t id_ = 0;

        mutable std::string formatted_; // to keep the formatted string alive for c_str()

        
    protected:
        
        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;     
        
    }; // end class DisplayHandle

} // namespace SDOM