#pragma once
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_DisplayObject.hpp>

namespace SDOM 
{
    class Stage;
    class DisplayObject;

    // --- Type Creation Structs --- //
    struct TypeCreators 
    {
        std::function<std::unique_ptr<IDisplayObject>(const sol::table&)> fromLua;
        std::function<std::unique_ptr<IDisplayObject>(const IDisplayObject::InitStruct&)> fromInitStruct;
    };

    class Factory final : public IDataObject
    {
        friend class Core;  // Core should have direct access to the Factory internals

    public:
        // --- Lifecycle (IDataObject overrides) --- //
        Factory();
        virtual ~Factory() = default;
        virtual bool onInit() override;
        virtual void onQuit() override {}
        virtual bool onUnitTest() override;

        // --- Object Type Registration --- //
        void registerDomType(const std::string& typeName, const TypeCreators& creators);
        // void registerResType(const std::string& typeName, const TypeCreators& creators);

        // --- Object Creation --- //
        DisplayObject create(const std::string& typeName, const sol::table& config);
        DisplayObject create(const std::string& typeName, const IDisplayObject::InitStruct& init);        
        DisplayObject create(const std::string& typeName, const std::string& luaScript);

        // Helper: attach a newly-created display object (by name/type) to a
        // parent specified in a Lua config value. Accepts string name, DomHandle,
        // or a nested table { parent = ... }. Returns true if attachment occurred.
        bool attachCreatedObjectToParentFromConfig(const std::string& name, const std::string& typeName, const sol::object& parentConfig);

        // --- Object Lookup --- //
        IDisplayObject* getDomObj(const std::string& name);
        DisplayObject getDisplayObject(const std::string& name);
        DisplayObject getStageHandle();

        // --- Display Object Management --- //
        void addDisplayObject(const std::string& name, std::unique_ptr<IDisplayObject> displayObject); // Consider refactoring/removal
        void destroyDisplayObject(const std::string& name);

        // --- Orphan Management --- //
        int countOrphanedDisplayObjects() const;
        std::vector<DisplayObject> getOrphanedDisplayObjects();
        void destroyOrphanedDisplayObjects();
        void detachOrphans();

        // --- Future Child Management --- //
        void attachFutureChildren();
        void addToOrphanList(const DisplayObject orphan);
        void addToFutureChildrenList(const DisplayObject child, const DisplayObject parent,
            bool useWorld=false, int worldX=0, int worldY=0);

        // --- Utility Methods --- //
        std::vector<std::string> listDisplayObjectNames() const;
        // std::vector<std::string> listResourceNames() const;
        void clear();
        void printObjectRegistry() const;

        // --- Internal Lua Integration --- //
        void initFromLua(const sol::table& lua);
        // void initLuaProcessResource(const sol::table& resource);

    private:
        // --- Internal Storage --- //
        std::unordered_map<std::string, std::unique_ptr<IDisplayObject>> displayObjects_;
        // std::unordered_map<std::string, std::unique_ptr<IResourceObject>> resources_;
        std::unordered_map<std::string, TypeCreators> creators_;

        // --- Orphan & Future Child Lists --- //

        std::vector<DisplayObject> orphanList_;
        struct futureChild 
        {
            DisplayObject child;
            DisplayObject parent;
            bool preserveWorldPosition;
            int dragStartWorldX;
            int dragStartWorldY;
        };            
        std::vector<futureChild> futureChildrenList_;

    public:
        // --- Lua Object Type Registry (public) --- //

        // [[deprecated("Factory::registerLuaProperty is deprecated. Use concrete type registration instead.")]]
        // void registerLuaProperty(const std::string& typeName,
        //                     const std::string& propertyName,
        //                     IDataObject::Getter getter,
        //                     IDataObject::Setter setter);

        // [[deprecated("Factory::registerLuaCommand is deprecated. Use concrete type registration instead.")]]
        // void registerLuaCommand(const std::string& typeName,
        //                     const std::string& commandName,
        //                     IDataObject::Command command);    

        // [[deprecated("Factory::registerLuaProperty is deprecated. Use concrete type registration instead.")]]
        // void registerLuaFunction(const std::string& typeName,
        //                     const std::string& functionName,
        //                     IDataObject::Function function);


        // -------------------------------------------------------------
        // DEPRECATED: ObjectTypeRegistry (kept for legacy registerLua())
        //
        // Reason:
        //  - This registry duplicates actual Lua binding code and is fragile:
        //    keeping registry entries in sync with concrete _registerLua()
        //    is error-prone and leads to divergence between C++ behavior
        //    and Lua bindings.
        //  - New approach: concrete types should populate the centralized
        //    DisplayObject binding surface via IDataObject::registerDisplayObject()
        //    or Factory::registerLuaFunction/registerLuaProperty directly.
        //
        // Migration:
        //  1) Stop adding new bindings to ObjectTypeRegistry.
        //  2) Move per-type bindings into registerDisplayObject() implementations
        //     that call Factory::registerLuaFunction / registerLuaProperty.
        //  3) Once all types use the new path, remove ObjectTypeRegistry.
        //
        // The types and helpers below remain for backwards compatibility only.
        // They are scheduled        struct ObjectTypeRegistryEntry
        // struct ObjectTypeRegistryEntry
        // {
        //     std::string typeName;

        //     struct PropertyEntry {
        //         std::string propertyName;
        //         IDataObject::Getter getter;
        //         IDataObject::Setter setter;
        //     };
        //     std::vector<PropertyEntry> properties;

        //     struct CommandEntry {
        //         std::string commandName;
        //         IDataObject::Command command;
        //     };
        //     std::vector<CommandEntry> commands;
            
        //     struct FunctionEntry {
        //         std::string functionName;
        //         IDataObject::Function function;
        //     };
        //     std::vector<FunctionEntry> functions;
        // };

        // // Deprecated, will be removed. Use registerLuaProperty/registerLuaCommand/
        // // registerLuaFunction and the concrete type registration paths instead.
        // [[deprecated("Factory::getTypeRegistryEntry is deprecated. Use concrete type registration instead.")]]
        // ObjectTypeRegistryEntry* getTypeRegistryEntry(const std::string& typeName);

        // [[deprecated("Factory::getPropertyEntry is deprecated. Use concrete type registration instead.")]]
        // ObjectTypeRegistryEntry::PropertyEntry* getPropertyEntry(
        //             const std::string& typeName, const std::string& propertyName);

        // [[deprecated("Factory::getCommandEntry is deprecated. Use concrete type registration instead.")]]
        // ObjectTypeRegistryEntry::CommandEntry* getCommandEntry(
        //             const std::string& typeName, const std::string& commandName);

        // [[deprecated("Factory::getFunctionEntry is deprecated. Use concrete type registration instead.")]]
        // ObjectTypeRegistryEntry::FunctionEntry* getFunctionEntry(
        //             const std::string& typeName, const std::string& functionName);


        // [[deprecated("Factory::getPropertyNamesForType is deprecated. Remove.")]]
        // std::vector<std::string> getPropertyNamesForType(const std::string& typeName) const;

        // [[deprecated("Factory::getCommandNamesForType is deprecated. Remove.")]]
        // std::vector<std::string> getCommandNamesForType(const std::string& typeName) const;

        // [[deprecated("Factory::getFunctionNamesForType is deprecated. Remove.")]]
        // std::vector<std::string> getFunctionNamesForType(const std::string& typeName) const;


    private:
        // // --- Lua Object Type Registry (private) --- //
        // std::unordered_map<std::string, ObjectTypeRegistryEntry> typeRegistry_;


    };
    

}


