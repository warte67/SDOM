#pragma once

#include <SDOM/SDOM_IDataObject.hpp>

/**
 * @file SDOM_IResourceObject.hpp
 * @brief Defines the base interface for Factory-managed resources and the resource_ptr smart pointer.
 */

namespace SDOM
{
    class Factory; // Forward declaration


    class IResourceObject : public IDataObject
    {
        friend class Factory; // Allow Factory to create Resource Objects
        friend class Core;
        // // Protect the new and delete operator to ensure that 
        // // the Factory is used for all allocations
        // static void* operator new(std::size_t size);
        // static void operator delete(void* ptr);

    public:
        IResourceObject() = delete;
        IResourceObject(std::string name="", std::string type="", std::string filename = "") : name_(name), type_(type), filemame_(filename) {}


        virtual ~IResourceObject() = default;

        // Required methods from IDataObject (must be implemented by derived classes):
        // virtual bool onInit() = 0;
        // virtual void onQuit() = 0;

        virtual bool onUnitTest() override { return true; }

        // Default comparison operators for Sol2
        bool operator==(const IResourceObject&) const { return false; }
        bool operator!=(const IResourceObject&) const { return true; }
        bool operator<(const IResourceObject&) const { return false; }
        bool operator<=(const IResourceObject&) const { return true; }
        bool operator>(const IResourceObject&) const { return false; }
        bool operator>=(const IResourceObject&) const { return true; }      

        std::string getName() const { return name_; }
        IResourceObject& setName(const std::string& newName) { name_ = newName; return *this; }
        std::string getFilename() const { return filemame_; }
        IResourceObject& setFilename(const std::string& newFilename) { filemame_ = newFilename; return *this; }
        std::string getType() const { return type_; }
        IResourceObject& setType(const std::string& newType) { type_ = newType; return *this; }

    protected:
        std::string name_;      ///< Unique resource name
        std::string filemame_;  ///< Filename associated with the resource
        std::string type_;      ///< Extensible string identifier for resource type

    };  // END class IResourceObject




} // namespace SDOM