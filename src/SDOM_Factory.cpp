// SDOM_Factory.cpp

#include <SDOM/SDOM.hpp>

// #include <SDOM/SDOM_Handle.hpp>
#include <SDOM/SDOM_DomHandle.hpp>
#include <SDOM/SDOM_ResHandle.hpp>

#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_Utils.hpp> // for parseColor

namespace SDOM
{

    Factory::Factory() : IDataObject()
    {
        // make sure the DomHandles can access this Factory
        DomHandle::factory_ = this;
        ResHandle::factory_ = this;      
    }

    bool Factory::onInit()
    {
        // --- Lua UserType Registration --- //
        Core& core = Core::getInstance();
        // core._registerLua_Usertype(core.getLua());
        core._registerLua("Core", core.getLua());
        // core._registerLua_Commands(core.getLua());

        // ===== Register built-in types =====

        // register the Stage
        registerDomType("Stage", TypeCreators{
            Stage::CreateFromLua, 
            Stage::CreateFromInitStruct
        });

        // Register other built-in types here as needed ...   
        DomHandle prototypeHandle; // Default DomHandle for registration 
        // prototypeHandle._registerLua_Usertype(core.getLua());
        prototypeHandle._registerLua("DomHandle", core.getLua());
        // ResHandle prototypeResHandle; // Default ResHandle for registration 
        // prototypeResHandle._registerLua_Usertype(core.getLua());
        // prototypeResHandle._registerLua("ResHandle", core.getLua());



        return true;
    }


    void Factory::registerDomType(const std::string& typeName, const TypeCreators& creators)
    {
        creators_[typeName] = creators;

        IDisplayObject::InitStruct init; // Default init struct
        init.type = typeName;
        DomHandle prototypeHandle = create(typeName, init);
        prototypeHandle->setType(typeName);
        if (prototypeHandle)
        {
            // prototypeHandle->_registerLua_Usertype(SDOM::getLua());
            prototypeHandle->_registerLua(typeName, SDOM::getLua());
            destroyDisplayObject(prototypeHandle.get()->getName()); // Clean up prototype
        }   
    }

    IResourceObject* Factory::getResObj(const std::string& name)
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) 
        {
            return dynamic_cast<IResourceObject*>(it->second.get());
        }
        return nullptr;
    }

    IDisplayObject* Factory::getDomObj(const std::string& name)
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) 
        {
            return dynamic_cast<IDisplayObject*>(it->second.get());
        }
        return nullptr;
    }
    
    DomHandle Factory::getDomHandle(const std::string& name) 
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) 
        {
            // Use the resource's type for the DomHandle
            return DomHandle(name, it->second->getType());
        }
        // Return an empty DomHandle if not found
        return DomHandle();
    }

    DomHandle Factory::getStageHandle() 
    {
        Stage* stage = getStage();
        if (!stage) {
            ERROR("Factory::getStageHandle: No stage available");
            return DomHandle(); // Return an empty/null handle if stage is not available
        }
        std::string stageName = stage->getName();
        return getDomHandle(stageName);
    }

    // String (LUA text) based object creator
    DomHandle Factory::create(const std::string& typeName, const sol::table& config)
    {
        // Get main texture size for defaults
        int maxStageWidth = 0;
        int maxStageHeight = 0;
        SDL_Texture* texture = getTexture();
        if (texture) {
            maxStageWidth = texture->w;
            maxStageHeight = texture->h;
        }

        // Minimum required properties (x, y, width, height now optional)
        std::vector<std::string> missing;
        if (!config["name"].valid()) missing.push_back("name");
        if (!config["type"].valid()) missing.push_back("type");

        if (!missing.empty()) {
            std::string msg = "Factory::create: Missing required property(s) in Lua config: ";
            for (size_t i = 0; i < missing.size(); ++i) {
                msg += "'" + missing[i] + "'";
                if (i < missing.size() - 1) msg += ", ";
            }
            ERROR(msg); // Throws and aborts creation
            return DomHandle(); // This line is not reached if ERROR throws
        }

        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromLua) 
        {
            // Set defaults for x, y, width, height if not present
            int x = config["x"].valid() ? static_cast<int>(config["x"].get<double>()) : 0;
            int y = config["y"].valid() ? static_cast<int>(config["y"].get<double>()) : 0;
            int width = config["width"].valid() ? static_cast<int>(config["width"].get<double>()) : maxStageWidth - x;
            int height = config["height"].valid() ? static_cast<int>(config["height"].get<double>()) : maxStageHeight - y;

            // Optionally, you may want to inject these values into config for the display object constructor:
            sol::table configCopy = config;
            configCopy["x"] = x;
            configCopy["y"] = y;
            configCopy["width"] = width;
            configCopy["height"] = height;

            auto displayObject = it->second.fromLua(configCopy);
            if (!displayObject) {
                std::cout << "Factory::create: Failed to create display object of type '" << typeName
                        << "' from Lua. Display object is nullptr.\n";
                return DomHandle(); // Invalid handle
            }
            std::string name = config["name"];
            displayObjects_[name] = std::move(displayObject);
            displayObjects_[name]->setType(typeName); // Ensure type is set

            // --- Set common properties from config ---
            if (config["color"].valid()) {
                SDL_Color color = parseColor(config["color"]);
                displayObjects_[name]->color_ = color;
            }
            // set anchorPoints if present
            auto setAnchorFromConfig = [](const sol::object& obj, auto setter) {
                if (obj.valid()) {
                    if (obj.is<std::string>()) {
                        std::string key = SDOM::normalizeAnchorString(obj.as<std::string>());
                        auto it = SDOM::stringToAnchorPoint_.find(key);
                        if (it != SDOM::stringToAnchorPoint_.end())
                            setter(it->second);
                    } else if (obj.is<int>()) {
                        setter(static_cast<SDOM::AnchorPoint>(obj.as<int>()));
                    }
                }
            };
            setAnchorFromConfig(config["anchorLeft"],   [&](SDOM::AnchorPoint ap){ displayObjects_[name]->setAnchorLeft(ap); });
            setAnchorFromConfig(config["anchorTop"],    [&](SDOM::AnchorPoint ap){ displayObjects_[name]->setAnchorTop(ap); });
            setAnchorFromConfig(config["anchorRight"],  [&](SDOM::AnchorPoint ap){ displayObjects_[name]->setAnchorRight(ap); });
            setAnchorFromConfig(config["anchorBottom"], [&](SDOM::AnchorPoint ap){ displayObjects_[name]->setAnchorBottom(ap); });
            
            // set additional properties if present
            if (config["z_order"].valid())      displayObjects_[name]->setZOrder(static_cast<int>(config["z_order"].get<double>()));
            if (config["priority"].valid())     displayObjects_[name]->setPriority(static_cast<int>(config["priority"].get<double>()));
            if (config["isClickable"].valid())  displayObjects_[name]->setClickable(config["isClickable"].get<bool>());
            if (config["isEnabled"].valid())    displayObjects_[name]->setEnabled(config["isEnabled"].get<bool>());
            if (config["isHidden"].valid())     displayObjects_[name]->setHidden(config["isHidden"].get<bool>());
            if (config["tabPriority"].valid())  displayObjects_[name]->setTabPriority(static_cast<int>(config["tabPriority"].get<double>()));
            if (config["tabEnabled"].valid())   displayObjects_[name]->setTabEnabled(config["tabEnabled"].get<bool>());

            // Set x, y, width, height
            displayObjects_[name]->setX(x);
            displayObjects_[name]->setY(y);
            displayObjects_[name]->setWidth(width);
            displayObjects_[name]->setHeight(height);

            // --- Finally initialize the display object ---
            displayObjects_[name]->onInit();

            return DomHandle(name, typeName);
        }
        return DomHandle(); // Invalid handle
    }

    // InitStruct based object creator
    DomHandle Factory::create(const std::string& typeName, const IDisplayObject::InitStruct& init)
    {
        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromInitStruct) 
        {
            auto displayObject = it->second.fromInitStruct(init);
            if (displayObject) 
            {
                std::string name = init.name;
                displayObject->onInit(); // Initialize the display object
                displayObject->setType(typeName); // Ensure type is set
                displayObjects_[name] = std::move(displayObject);
                return DomHandle(name, typeName);
            }
        }
        return DomHandle(); // Invalid handle
    }

    DomHandle Factory::create(const std::string& typeName, const std::string& luaScript) {
        sol::state lua;
        lua.open_libraries(sol::lib::base);

        // Try to parse and execute the string as a Lua table
        sol::object result = lua.script("return " + luaScript, sol::script_pass_on_error);

        if (!result.valid() || !result.is<sol::table>()) {
            std::cout << "Factory::create: Provided string is not a valid Lua table.\n";
            return DomHandle();
        }

        sol::table config = result.as<sol::table>();
        return create(typeName, config);
    }


    void Factory::addDisplayObject(const std::string& name, 
        std::unique_ptr<IDisplayObject> displayObject) 
    {
        displayObjects_[name] = std::move(displayObject);
    }
    
    void Factory::destroyDisplayObject(const std::string& name) 
    {
        displayObjects_.erase(name);
    }

    int Factory::countOrphanedDisplayObjects() const {
        int count = 0;
        for (const auto& [name, objPtr] : displayObjects_) {
            auto obj = dynamic_cast<IDisplayObject*>(objPtr.get());
            if (obj && !obj->getParent() && obj->getType() != "Stage") {
                ++count;
            }
        }
        return count;
    }

    std::vector<DomHandle> Factory::getOrphanedDisplayObjects() {
        std::vector<DomHandle> orphans;
        for (const auto& [name, objPtr] : displayObjects_) {
            auto obj = dynamic_cast<IDisplayObject*>(objPtr.get());
            if (obj && !obj->getParent() && obj->getType() != "Stage") {
                orphans.push_back(getDomHandle(name));
            }
        }
        return orphans;
    }

    void Factory::destroyOrphanedDisplayObjects() 
    {
        while (countOrphanedDisplayObjects())
        {
            auto orphans = getOrphanedDisplayObjects();
            for (const auto& orphan : orphans) {
                destroyDisplayObject(orphan->getName());
            }
        }
    }    


    std::vector<std::string> Factory::listDisplayObjectNames() const
    {
        std::vector<std::string> names;
        for (const auto& pair : displayObjects_) {
            names.push_back(pair.first);
        }
        return names;
    }

    void Factory::clear()
    {
        displayObjects_.clear();
    }

    void Factory::printObjectRegistry() const
    {
        std::cout << "Factory Display Object Registry:\n";
        for (const auto& pair : displayObjects_) {
            const auto& name = pair.first;
            const auto& displayObject = pair.second;
            std::cout << "  Name: " << name
                    << ", Type: " << (displayObject ? displayObject->getType() : "Unknown")
                    << "\n";
        }
        std::cout << "Total display objects: " << displayObjects_.size() << std::endl;
    }

    

    // ----- Orphan and Future Child Management -----

    void Factory::detachOrphans()
    {
        for (auto& orphanHandle : orphanList_)
        {
            DomHandle orphan = orphanHandle;
            if (orphan)
            {
                DomHandle parent = orphan->getParent();
                if (parent)
                {
                    // Remove orphan from parent's children using public removeChild method
                    parent->removeChild(orphanHandle);
                    // Set orphan's parent to nullptr using public setParent method
                    orphan->setParent(DomHandle());
                }
            }
        }
        orphanList_.clear();
    }

    void Factory::attachFutureChildren() 
    {
        for (auto& futureChild : futureChildrenList_) 
        {
            DomHandle child = futureChild.child;
            if (child)
            {
                child->attachChild_(
                    futureChild.child,
                    futureChild.parent,
                    futureChild.preserveWorldPosition,
                    futureChild.dragStartWorldX,
                    futureChild.dragStartWorldY
                );
            }
        }
        futureChildrenList_.clear();
    }

    void Factory::addToOrphanList(const DomHandle orphan) 
    {
        if (orphan) 
        {
            orphanList_.push_back(orphan);
        }
    }

    void Factory::addToFutureChildrenList(const DomHandle child, const DomHandle parent, bool useWorld, int worldX, int worldY) 
    {
        if (child && parent) 
        {

            // std::cout << "child added to queue: " << child->getName() << " to (" << 
            //     worldX << ", " << worldY << ", w:" << child->getWidth() 
            //     << ", h:" << child->getHeight() << ")" << std::endl; 

            futureChildrenList_.push_back({child, parent, useWorld, worldX, worldY});
        }
    }


    void Factory::initFromLua(const sol::table& lua)
    {
        // Lambda for processing a single resource entry
        auto processResource = [this](const sol::table& resource) {
            if (!resource["type"].valid() || !resource["name"].valid() || !resource["config"].valid())
            {
                ERROR("Resource entry is missing required fields: 'type', 'name', or 'config'.");
                return;
            }
            std::string type = resource["type"];
            std::string name = resource["name"];
            sol::table config = resource["config"];
            auto creatorIt = creators_.find(type);
            if (creatorIt != creators_.end())
            {
                const TypeCreators& creators = creatorIt->second;
                std::unique_ptr<IDisplayObject> displayObj;
                if (creators.fromLua) {
                    displayObj = creators.fromLua(config);
                }
                if (displayObj)
                {
                    addDisplayObject(name, std::move(displayObj));
                }
                else
                {
                    ERROR("Failed to create display object: " + name + " of type: " + type);
                }
            }
            else
            {
                ERROR("Unknown display object type: " + type);
            }
        };

        // Handle array of resources
        if (lua["resources"].valid() && lua["resources"].is<sol::table>())
        {
            sol::table resources = lua["resources"];
            for (auto& kv : resources)
            {
                sol::table resource = kv.second.as<sol::table>();
                processResource(resource);
            }
        }

        // Handle single resource object
        if (lua["resource"].valid() && lua["resource"].is<sol::table>())
        {
            processResource(lua["resource"]);
        }

        // Debug: List installed display objects
        auto names = listDisplayObjectNames();
        std::cout << "Factory::initFromLua() --> Installed display objects: ";
        for (const auto& name : names)
        {
            std::cout << name << " ";
        }
        std::cout << std::endl;
    }    

    bool Factory::onUnitTest()
    {
        // std::cout << CLR::WHITE << "Factory Default Unit Tests:" << CLR::RESET << std::endl;
        return true;       
    } // END Factory::onUnitTest()


    std::vector<std::string> Factory::getPropertyNamesForType(const std::string& typeName) const
    {
        std::vector<std::string> names;
        auto it = typeRegistry_.find(typeName);
        if (it == typeRegistry_.end()) return names;
        const auto& entry = it->second;
        // Adjust the following to the actual property container field name in ObjectTypeRegistryEntry
        for (const auto& prop : entry.properties) {
            names.push_back(prop.propertyName);
        }
        return names;
    }

    std::vector<std::string> Factory::getCommandNamesForType(const std::string& typeName) const
    {
        std::vector<std::string> names;
        auto it = typeRegistry_.find(typeName);
        if (it == typeRegistry_.end()) return names;
        const auto& entry = it->second;
        // Adjust to actual command container field name
        for (const auto& cmd : entry.commands) {
            names.push_back(cmd.commandName);
        }
        return names;
    }

    std::vector<std::string> Factory::getFunctionNamesForType(const std::string& typeName) const
    {
        std::vector<std::string> names;
        auto it = typeRegistry_.find(typeName);
        if (it == typeRegistry_.end()) return names;
        const auto& entry = it->second;
        // Adjust to actual function container field name
        for (const auto& fn : entry.functions) {
            names.push_back(fn.functionName);
        }
        return names;
    }





    void Factory::registerLuaProperty(const std::string& typeName,
                                    const std::string& propertyName,
                                    IDataObject::Getter getter,
                                    IDataObject::Setter setter)
    {
        auto& entry = typeRegistry_[typeName];
        entry.typeName = typeName;

        // Check if property already exists, update if so
        for (auto& prop : entry.properties) {
            if (prop.propertyName == propertyName) {
                prop.getter = getter;
                prop.setter = setter;
                return;
            }
        }
        // Otherwise, add new property
        entry.properties.push_back({propertyName, getter, setter});
    }

    void Factory::registerLuaCommand(const std::string& typeName,
                                    const std::string& commandName,
                                    IDataObject::Command command)
    {
        auto& entry = typeRegistry_[typeName];
        entry.typeName = typeName;

        // Check if command already exists, update if so
        for (auto& cmd : entry.commands) {
            if (cmd.commandName == commandName) {
                cmd.command = command;
                return;
            }
        }
        // Otherwise, add new command
        entry.commands.push_back({commandName, command});
    }

    void Factory::registerLuaFunction(const std::string& typeName,
                                            const std::string& functionName,
                                            IDataObject::Function function)
    {
        auto& entry = typeRegistry_[typeName];
        entry.typeName = typeName;
        // If function already exists with same name, replace it
        for (auto &fe : entry.functions) {
            if (fe.functionName == functionName) { fe.function = function; return; }
        }
        entry.functions.push_back({functionName, function});
    }

    Factory::ObjectTypeRegistryEntry* 
            Factory::getTypeRegistryEntry(const std::string& typeName) 
    {
        auto it = typeRegistry_.find(typeName);
        if (it != typeRegistry_.end())
            return &it->second;
        return nullptr;
    }

    Factory::ObjectTypeRegistryEntry::PropertyEntry* 
            Factory::getPropertyEntry(const std::string& typeName, 
                                    const std::string& propertyName) 
    {
        auto entry = getTypeRegistryEntry(typeName);
        if (!entry) return nullptr;
        for (auto& prop : entry->properties) {
            if (prop.propertyName == propertyName)
                return &prop;
        }
        return nullptr;
    }

    Factory::ObjectTypeRegistryEntry::CommandEntry* 
            Factory::getCommandEntry(const std::string& typeName, 
                                    const std::string& commandName) 
    {
        auto entry = getTypeRegistryEntry(typeName);
        if (!entry) return nullptr;
        for (auto& cmd : entry->commands) {
            if (cmd.commandName == commandName)
                return &cmd;
        }
        return nullptr;
    }

    void Factory::registerLuaObjectTypes_test()
    {
        // --- Box ---
        std::string boxType = "Box";
        registerLuaProperty(boxType, "x", nullptr, nullptr);
        registerLuaProperty(boxType, "y", nullptr, nullptr);
        registerLuaProperty(boxType, "width", nullptr, nullptr);
        registerLuaProperty(boxType, "height", nullptr, nullptr);
        registerLuaProperty(boxType, "color", nullptr, nullptr);
        registerLuaProperty(boxType, "visible", nullptr, nullptr);
        registerLuaProperty(boxType, "enabled", nullptr, nullptr);
        registerLuaProperty(boxType, "zOrder", nullptr, nullptr);

        registerLuaCommand(boxType, "moveTo", nullptr);
        registerLuaCommand(boxType, "resize", nullptr);
        registerLuaCommand(boxType, "setColor", nullptr);
        registerLuaCommand(boxType, "show", nullptr);
        registerLuaCommand(boxType, "hide", nullptr);

        // --- Stage ---
        std::string stageType = "Stage";
        registerLuaProperty(stageType, "width", nullptr, nullptr);
        registerLuaProperty(stageType, "height", nullptr, nullptr);
        registerLuaProperty(stageType, "backgroundColor", nullptr, nullptr);
        registerLuaProperty(stageType, "name", nullptr, nullptr);

        registerLuaCommand(stageType, "addChild", nullptr);
        registerLuaCommand(stageType, "removeChild", nullptr);
        registerLuaCommand(stageType, "setBackgroundColor", nullptr);
        registerLuaCommand(stageType, "resize", nullptr);

        // --- Button ---
        std::string buttonType = "Button";
        registerLuaProperty(buttonType, "x", nullptr, nullptr);
        registerLuaProperty(buttonType, "y", nullptr, nullptr);
        registerLuaProperty(buttonType, "width", nullptr, nullptr);
        registerLuaProperty(buttonType, "height", nullptr, nullptr);
        registerLuaProperty(buttonType, "label", nullptr, nullptr);
        registerLuaProperty(buttonType, "enabled", nullptr, nullptr);
        registerLuaProperty(buttonType, "visible", nullptr, nullptr);

        registerLuaCommand(buttonType, "click", nullptr);
        registerLuaCommand(buttonType, "setLabel", nullptr);
        registerLuaCommand(buttonType, "show", nullptr);
        registerLuaCommand(buttonType, "hide", nullptr);

        // --- Label ---
        std::string labelType = "Label";
        registerLuaProperty(labelType, "x", nullptr, nullptr);
        registerLuaProperty(labelType, "y", nullptr, nullptr);
        registerLuaProperty(labelType, "text", nullptr, nullptr);
        registerLuaProperty(labelType, "fontSize", nullptr, nullptr);
        registerLuaProperty(labelType, "color", nullptr, nullptr);
        registerLuaProperty(labelType, "visible", nullptr, nullptr);

        registerLuaCommand(labelType, "setText", nullptr);
        registerLuaCommand(labelType, "setColor", nullptr);
        registerLuaCommand(labelType, "show", nullptr);
        registerLuaCommand(labelType, "hide", nullptr);

        // Print simulated registration code for all types
        for (const auto& [typeName, entry] : typeRegistry_) {
            std::cout << "lua.new_usertype<" << typeName << ">(\"" << typeName << "\",\n";
            for (const auto& prop : entry.properties) {
                std::cout << "    \"" << prop.propertyName << "\", /* getter/setter */,\n";
                std::cout << "    \"set" << char(std::toupper(prop.propertyName[0])) << prop.propertyName.substr(1)
                        << "\", /* setter */,\n";
            }
            for (const auto& cmd : entry.commands) {
                std::cout << "    \"" << cmd.commandName << "\", /* command */,\n";
            }
            std::cout << ");\n\n";
        }
    }





} // namespace SDOM

