#pragma once
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IResourceObject.hpp>

namespace SDOM 
{
    class Stage;

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
        DomHandle create(const std::string& typeName, const sol::table& config);
        DomHandle create(const std::string& typeName, const IDisplayObject::InitStruct& init);        
        DomHandle create(const std::string& typeName, const std::string& luaScript);

        // --- Object Lookup --- //
        IDisplayObject* getDomObj(const std::string& name);
        IResourceObject* getResObj(const std::string& name);
        DomHandle getDomHandle(const std::string& name);
        // ResHandle getResHandle(const std::string& name);
        DomHandle getStageHandle();

        // --- Display Object Management --- //
        void addDisplayObject(const std::string& name, std::unique_ptr<IDisplayObject> displayObject); // Consider refactoring/removal
        void destroyDisplayObject(const std::string& name);

        // --- Orphan Management --- //
        int countOrphanedDisplayObjects() const;
        std::vector<DomHandle> getOrphanedDisplayObjects();
        void destroyOrphanedDisplayObjects();
        void detachOrphans();

        // --- Future Child Management --- //
        void attachFutureChildren();
        void addToOrphanList(const DomHandle orphan);
        void addToFutureChildrenList(const DomHandle child, const DomHandle parent,
            bool useWorld=false, int worldX=0, int worldY=0);

        // --- Utility Methods --- //
        std::vector<std::string> listDisplayObjectNames() const;
        // std::vector<std::string> listResourceNames() const;
        void clear();
        void printObjectRegistry() const;

        // --- Internal Lua Integration --- //
        void initFromLua(const sol::table& lua);
        // void initLuaProcessResource(const sol::table& resource);

        // --- New Factory Methods --- //
        std::vector<std::string> getPropertyNamesForType(const std::string& typeName) const;
        std::vector<std::string> getCommandNamesForType(const std::string& typeName) const;
        std::vector<std::string> getFunctionNamesForType(const std::string& typeName) const;

    private:
        // --- Internal Storage --- //
        std::unordered_map<std::string, std::unique_ptr<IDisplayObject>> displayObjects_;
        // std::unordered_map<std::string, std::unique_ptr<IResourceObject>> resources_;
        std::unordered_map<std::string, TypeCreators> creators_;

        // --- Orphan & Future Child Lists --- //
        std::vector<DomHandle> orphanList_;
        struct futureChild 
        {
            DomHandle child;
            DomHandle parent;
            bool preserveWorldPosition;
            int dragStartWorldX;
            int dragStartWorldY;
        };
        std::vector<futureChild> futureChildrenList_;

    public:
        // --- Lua Object Type Registry (public) --- //
        struct ObjectTypeRegistryEntry
        {
            std::string typeName;

            struct PropertyEntry {
                std::string propertyName;
                IDataObject::Getter getter;
                IDataObject::Setter setter;
            };
            std::vector<PropertyEntry> properties;

            struct CommandEntry {
                std::string commandName;
                IDataObject::Command command;
            };
            std::vector<CommandEntry> commands;
            
            struct FunctionEntry {
                std::string functionName;
                IDataObject::Function function;
            };
            std::vector<FunctionEntry> functions;
        };

        void registerLuaProperty(const std::string& typeName,
                                const std::string& propertyName,
                                IDataObject::Getter getter,
                                IDataObject::Setter setter);

        void registerLuaCommand(const std::string& typeName,
                            const std::string& commandName,
                            IDataObject::Command command);    

        void registerLuaFunction(const std::string& typeName,
                    const std::string& functionName,
                    IDataObject::Function function);

        ObjectTypeRegistryEntry* getTypeRegistryEntry(const std::string& typeName);
        ObjectTypeRegistryEntry::PropertyEntry* getPropertyEntry(const std::string& typeName, const std::string& propertyName);
        ObjectTypeRegistryEntry::CommandEntry* getCommandEntry(const std::string& typeName, const std::string& commandName);
        ObjectTypeRegistryEntry::FunctionEntry* getFunctionEntry(const std::string& typeName, const std::string& functionName);

        void registerLuaObjectTypes_test();

        template<typename T>
        void registerLuaPropertiesAndCommands(const std::string& typeName, sol::usertype<T>& usertypeTable) 
        {
            auto* entry = getTypeRegistryEntry(typeName);
            if (!entry) return;

            for (const auto& prop : entry->properties) 
            {
                // Only assign if a getter/setter isn't already present to
                // avoid overwriting direct usertype bindings created earlier.
                if (prop.getter) {
                    sol::object existing = usertypeTable[prop.propertyName];
                    if (!existing.valid()) usertypeTable[prop.propertyName] = prop.getter;
                }
                if (prop.setter) 
                {
                    std::string setterName = "set" + prop.propertyName;
                    setterName[3] = std::toupper(setterName[3]);
                    sol::object existingSetter = usertypeTable[setterName];
                    if (!existingSetter.valid()) usertypeTable[setterName] = prop.setter;
                }
            }
            for (const auto& cmd : entry->commands) 
            {
                if (cmd.command) { sol::object existingCmd = usertypeTable[cmd.commandName]; if (!existingCmd.valid()) usertypeTable[cmd.commandName] = cmd.command; }
            }
            for (const auto& fn : entry->functions)
            {
                if (fn.function) { sol::object existingFn = usertypeTable[fn.functionName]; if (!existingFn.valid()) usertypeTable[fn.functionName] = fn.function; }
            }
        }

    private:
        // --- Lua Object Type Registry (private) --- //
        std::unordered_map<std::string, ObjectTypeRegistryEntry> typeRegistry_;


    };
    

}


