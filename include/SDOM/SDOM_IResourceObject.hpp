#pragma once

#include <SDOM/SDOM_IDataObject.hpp>

/**
 * @file SDOM_IResourceObject.hpp
 * @brief Defines the base interface for Factory-managed resources and the resource_ptr smart pointer.
 */

namespace SDOM
{
    class Factory; // Forward declaration

    /**
     * @class IResourceObject
     * @brief Base interface for all Factory-managed resources in SDOM.
     * @details
     * Inherits from IDataObject. All resources managed by Factory must implement this interface.
     * 
     * Required methods (inherited from IDataObject):
     *   - bool onInit()
     *   - void onQuit()
     *   - bool onUnitTest()
     * 
     * Members:
     *   - name_: Unique resource name assigned by Factory.
     *   - filemame_: Filename associated with the resource (may be empty).
     *   - type_: Extensible string identifier for resource type.
     */
    class IResourceObject : public IDataObject
    {
    public:
        /**
         * @brief Virtual destructor for safe polymorphic deletion.
         */
        virtual ~IResourceObject() = default;

        // Required methods from IDataObject (must be implemented by derived classes):
        // virtual bool onInit() = 0;
        // virtual void onQuit() = 0;

        /**
         * @brief Unit test for the resource object.
         * @return Always returns true by default. Override in derived classes for actual tests.
         */
        virtual bool onUnitTest() override { return true; }

        /**
         * @brief Gets the unique resource name.
         * @return Resource name as a string.
         */
        std::string getName() const { return name_; }

        /**
         * @brief Sets the unique resource name.
         * @param newName New resource name.
         * @return Reference to this object.
         */
        IResourceObject& setName(const std::string& newName) { name_ = newName; return *this; }

        /**
         * @brief Gets the filename associated with the resource.
         * @return Filename as a string.
         */
        std::string getFilename() const { return filemame_; }

        /**
         * @brief Sets the filename associated with the resource.
         * @param newFilename New filename.
         * @return Reference to this object.
         */
        IResourceObject& setFilename(const std::string& newFilename) { filemame_ = newFilename; return *this; }

        /**
         * @brief Gets the resource type identifier.
         * @return Resource type as a string.
         */
        std::string getType() const { return type_; }

        /**
         * @brief Sets the resource type identifier.
         * @param newType New resource type.
         * @return Reference to this object.
         */
        IResourceObject& setType(const std::string& newType) { type_ = newType; return *this; }

    private:
        std::string name_;      ///< Unique resource name
        std::string filemame_;  ///< Filename associated with the resource
        std::string type_;      ///< Extensible string identifier for resource type

    };  // END class IResourceObject




} // namespace SDOM