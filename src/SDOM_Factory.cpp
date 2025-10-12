// SDOM_Factory.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_IAssetObject.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_Utils.hpp>
#include <SDOM/SDOM_Texture.hpp>
#include <SDOM/SDOM_TTFAsset.hpp>
#include <SDOM/SDOM_TruetypeFont.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/SDOM_Label.hpp>

namespace SDOM
{

    Factory::Factory() : IDataObject(), initialized_(false)
    {
        // Seed the static factory in the DisplayHandle and AssetHandle handles
        DisplayHandle::factory_ = this;
        AssetHandle::factory_ = this;
    }

    bool Factory::onInit()
    {
        // Make onInit idempotent: if initialization already ran, do nothing.
        if (initialized_) {
            return true;
        }
        // --- Lua UserType Registration --- //
        Core& core = getCore();
        core._registerLuaBindings("Core", core.getLua());

        // // register the DisplayHandle last so other types can use it
        // DisplayHandle prototypeHandle; // Default DisplayHandle for registration
        // prototypeHandle._registerLuaBindings("DisplayHandle", core.getLua());

        // register the Stage
        registerDomType("Stage", TypeCreators{
            Stage::CreateFromLua, 
            Stage::CreateFromInitStruct
        });

        // register the Texture asset
        registerResType("Texture", AssetTypeCreators{
            Texture::CreateFromLua,
            Texture::CreateFromInitStruct
        });

        // Note: Do NOT pre-register a Texture under the name 'default_bmp_8x8'.
        // The BitmapFont asset will create (or reuse) the SpriteSheet/Texture for
        // that filename when initialized. Pre-creating a Texture with the same
        // registry name conflicts with BitmapFont's intended resource name.

        {   // register the default_icon_8x8 Texture (skip if already created)
            const std::string defaultIconName = "default_icon_8x8";
            if (!getResObj(defaultIconName)) {
                Texture::InitStruct init;
                init.name = defaultIconName;
                init.type = "Texture";
                init.filename = defaultIconName;
                AssetHandle spriteSheet = createAsset("Texture", init);
                Texture* spriteSheetPtr = spriteSheet.as<Texture>();
                if (spriteSheetPtr) spriteSheetPtr->_registerLuaBindings("Texture", core.getLua());
            } else {
                // Ensure Lua bindings are registered for existing instance
                IAssetObject* existing = getResObj(defaultIconName);
                Texture* spriteSheetPtr = dynamic_cast<Texture*>(existing);
                if (spriteSheetPtr) {
                    try { spriteSheetPtr->_registerLuaBindings("Texture", core.getLua()); } catch(...) {}
                }
            }
        }

        // register a TTFAsset
        registerResType("TTFAsset", AssetTypeCreators{
            TTFAsset::CreateFromLua,
            TTFAsset::CreateFromInitStruct
        });    

        // register the TruetypeFont resource type so factory can create truetype font assets
        registerResType(TruetypeFont::TypeName, AssetTypeCreators{
            TruetypeFont::CreateFromLua,
            TruetypeFont::CreateFromInitStruct
        });
        // Also register a friendly alias matching config usage
        registerResType("TruetypeFont", AssetTypeCreators{
            TruetypeFont::CreateFromLua,
            TruetypeFont::CreateFromInitStruct
        });

        // Create an internal TTFAsset for the default TrueType font (do not register under the same public name)
        if (!getResObj("default_ttf_asset")) {
            TTFAsset::InitStruct ttf_init;
            ttf_init.name = "default_ttf_asset";     // internal registry key
            ttf_init.type = TTFAsset::TypeName;     // concrete type name
            ttf_init.filename = "default_ttf";      // internal ttf filename
            ttf_init.isInternal = true;             // is internal
            ttf_init.internalFontSize = 10;         // member name in InitStruct
            AssetHandle ttfFontAsset = createAsset("TTFAsset", ttf_init);
            (void)ttfFontAsset;
        }

        // Create a public truetype font asset named "default_ttf" which references the internal TTFAsset
        if (!getResObj("default_ttf")) {
            TruetypeFont::InitStruct ttInit;
            ttInit.name = "default_ttf";           // public registry key expected by Label
            ttInit.type = TruetypeFont::TypeName;
            ttInit.filename = "default_ttf_asset";      // filename used by TruetypeFont to find the TTFAsset

            // Attempt to create the truetype font; if TTF isn't available or creation fails, fall back to bitmap font
            AssetHandle defaultTTFont;
            try {
                defaultTTFont = createAsset(TruetypeFont::TypeName, ttInit);
            } catch (...) {
                // If creation failed, leave defaultTTFont invalid and fall back below
                defaultTTFont = AssetHandle();
            }

            if (!defaultTTFont.isValid()) {
                // Fallback: create a bitmap font named "default_ttf" that reuses the default 8x8 sprite
                BitmapFont::InitStruct fbInit;
                fbInit.name = "default_ttf";
                fbInit.type = BitmapFont::TypeName;
                fbInit.filename = "default_bmp_8x8";
                fbInit.isInternal = true;
                fbInit.fontSize = 8;
                AssetHandle fb = createAsset("BitmapFont", fbInit);
                (void)fb; // silence unused
            }
        }
            

        // register the SpriteSheet asset
        registerResType("SpriteSheet", AssetTypeCreators{
            SpriteSheet::CreateFromLua,
            SpriteSheet::CreateFromInitStruct
        });

        // register the BitmapFont asset
        registerResType("BitmapFont", AssetTypeCreators{
            BitmapFont::CreateFromLua,
            BitmapFont::CreateFromInitStruct
        });

        // Create the default 8x8 bitmap font asset if it doesn't exist.
        // Use the filename as the canonical resource name so Lua can refer to
        // the font simply as "default_bmp_8x8". The BitmapFont will create
        // (or reuse) an internal SpriteSheet named '<filename>_SpriteSheet'.
        if (!getResObj("default_bmp_8x8")) {
            BitmapFont::InitStruct init;
            init.name = "default_bmp_8x8";      // registry key == filename
            init.type = BitmapFont::TypeName;   // concrete type name
            init.filename = "default_bmp_8x8";  // underlying texture filename
            init.isInternal = true;             // is an internal resource
            init.fontSize = 8;                  // member name in InitStruct
            init.fontWidth = 8;                 // font_width for this resource
            init.fontHeight = 8;                // font_height for this resource            
            AssetHandle bmpFont = createAsset("BitmapFont", init);
            (void)bmpFont;
        }
        if (!getResObj("default_bmp_8x12")) {
            BitmapFont::InitStruct init;
            init.name = "default_bmp_8x12";     // registry key == filename
            init.type = BitmapFont::TypeName;   // concrete type name
            init.filename = "default_bmp_8x12"; // underlying texture filename
            init.isInternal = true;             // is an internal resource
            // Set both BitmapFont-specific fontSize and the base IFontObject's
            // fontSize_ so the inherited constructor sees the correct value.
            init.fontSize = 12;                 // BitmapFont::InitStruct field
            init.fontSize_ = 12;                // IFontObject::InitStruct field used by base ctor
            init.fontWidth = 8;                 // font_width for this resource
            init.fontHeight = 12;               // font_height for this resource
            AssetHandle bmpFont = createAsset("BitmapFont", init);
            (void)bmpFont;
        }
        // register the Label display object
        registerDomType("Label", TypeCreators{
            Label::CreateFromLua,
            // Wrap the init-struct creator to accept the base InitStruct
            [](const IDisplayObject::InitStruct& baseInit) -> std::unique_ptr<IDisplayObject> {
                const auto& init = static_cast<const Label::InitStruct&>(baseInit);
                return Label::CreateFromInitStruct(init);
            }
        });

        initialized_ = true;
        return true;
    }


    // Maintenance orphaned objects based on their retention policy
    void Factory::collectGarbage()
    {
        constexpr bool SHOW_DEBUG = false;
        // Get the current list of orphaned display objects
        std::vector<DisplayHandle> orphanList_ = getOrphanedDisplayObjects();

        if (orphanList_.empty()) 
        {
            // No orphans to collect
            return;
        }

        // Default grace period for GracePeriod policy (ms). Adjust or make configurable if desired.
        constexpr auto defaultGrace = std::chrono::milliseconds(ORPHAN_GRACE_PERIOD);
        auto now = std::chrono::steady_clock::now();

        // Collect names to destroy to avoid mutating registries while iterating
        std::vector<std::string> toDestroy;
        if (SHOW_DEBUG) std::cout << "Factory::collectGarbage: " << orphanList_.size() << " orphan(s) found\n";

        for (const auto& orphan : orphanList_) 
        {
            if (!orphan || !orphan.get()) continue;

            // IDisplayObject is friend of Factory so we can access its internals if needed.
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(orphan.get());
            if (!obj) continue;

            auto policy = obj->getOrphanRetentionPolicy();
            switch (policy) 
            {
                case IDisplayObject::OrphanRetentionPolicy::AutoDestroy:
                    if (SHOW_DEBUG) std::cout << "  AutoDestroy -> scheduling: " << orphan.getName() << " (type=" << orphan->getType() << ")\n";
                    toDestroy.push_back(orphan.getName());
                    break;

                case IDisplayObject::OrphanRetentionPolicy::GracePeriod: 
                {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - obj->orphanedAt_);
                    auto until = obj->getOrphanGrace();
                    if (elapsed >= until) 
                    {
                        if (SHOW_DEBUG) std::cout << "  GracePeriod expired (" << elapsed.count() << "ms) -> scheduling: " << orphan.getName() << "\n";
                        toDestroy.push_back(orphan.getName());
                    }
                    else 
                    {
                        if (SHOW_DEBUG) std::cout << "  GracePeriod active (" << elapsed.count() << "ms) -> retaining: " << orphan.getName() << "\n";
                    }
                    break;
                }

                case IDisplayObject::OrphanRetentionPolicy::RetainUntilManual:
                default:
                    if (SHOW_DEBUG) std::cout << "  RetainUntilManual -> keeping: " << orphan.getName() << "\n";
                    break;
            }
        }

        if (toDestroy.empty()) 
        {
            if (SHOW_DEBUG) std::cout << "No orphaned objects eligible for destruction at this time.\n";
            return;
        }

        for (const auto& name : toDestroy) 
        {
            if (SHOW_DEBUG) std::cout << "Destroying orphaned DisplayHandle: " << name << "\n";
            destroyDisplayObject(name);
        }
    } // end:   void Factory::collectGarbage()


    
    void Factory::registerDomType(const std::string& typeName, const TypeCreators& creators)
    {
        creators_[typeName] = creators;

        // Create a lightweight prototype for Lua registration.
        // Avoid calling the InitStruct-based creator with a base InitStruct
        // which may be static_cast'd to a derived InitStruct by the
        // concrete creator (undefined behaviour). Prefer to use the
        // fromLua creator to construct a safe temporary prototype.
        if (creators.fromLua) {
            try {
                sol::state_view lua = SDOM::getLua();
                sol::table proto = lua.create_table();
                proto["name"] = std::string("__proto:") + typeName;
                proto["type"] = typeName;
                proto["x"] = 0;
                proto["y"] = 0;
                proto["width"] = 1;
                proto["height"] = 1;
                auto prototype = creators.fromLua(proto);
                if (prototype) {
                    // Register Lua bindings for the concrete instance so methods
                    // are available on the shared userdata/handle.
                    try {
                        prototype->_registerLuaBindings(typeName, SDOM::getLua());
                    } catch(...) {
                        // swallow registration errors for prototypes
                    }
                    // prototype unique_ptr will be destroyed here; no need to
                    // insert into the factory registry.
                }
            } catch(...) {
                // ignore prototype creation failures; registration can proceed
            }
        }
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

    void Factory::registerResType(const std::string& typeName, const AssetTypeCreators& creators)
    {
        // Register resource type with creators
        assetCreators_[typeName] = creators;

        // Create a prototype AssetHandle for Lua registration
        AssetHandle prototypeHandle(typeName, typeName, typeName);
        prototypeHandle._registerLuaBindings(typeName, SDOM::getLua());
    }

    IAssetObject* Factory::getResObj(const std::string& name)
    {
        auto it = assetObjects_.find(name);
        if (it != assetObjects_.end()) 
        {
            return dynamic_cast<IAssetObject*>(it->second.get());
        }
        return nullptr;
    }
    
    DisplayHandle Factory::getDisplayObject(const std::string& name) 
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) 
        {
            // Use the resource's type for the DisplayHandle
            return DisplayHandle(name, it->second->getType());
        }
        // Return an empty DisplayHandle if not found
        return DisplayHandle();
    }

    AssetHandle Factory::getAssetObject(const std::string& name) 
    {
        auto it = assetObjects_.find(name);
        if (it != assetObjects_.end()) 
        {
            AssetHandle out;
            out.name_ = name;
            out.type_ = it->second->getType();
            return out;
        }
        // Return an empty AssetHandle if not found
        return AssetHandle();
    }
    
    DisplayHandle Factory::getStageHandle() 
    {
        Stage* stage = getStage();
        if (!stage) {
            ERROR("Factory::getStageHandle: No stage available");
            return DisplayHandle(); // Return an empty/null handle if stage is not available
        }
        std::string stageName = stage->getName();
        return getDisplayObject(stageName);
    }

    // String (LUA text) based object creator
    DisplayHandle Factory::create(const std::string& typeName, const sol::table& config)
    {
        // Get main texture size for defaults
        // int maxStageWidth = 0;
        // int maxStageHeight = 0;
        // SDL_Texture* texture = getTexture();
        // if (texture) {
        //     maxStageWidth = texture->w;
        //     maxStageHeight = texture->h;
        // }

        int maxStageWidth = 0;
        int maxStageHeight = 0;
        SDL_Texture* texture = getTexture();
        if (texture) 
        {
            float texW = 0.0f, texH = 0.0f;
            if (SDL_GetTextureSize(texture, &texW, &texH)) 
            {
                maxStageWidth  = static_cast<int>(texW);
                maxStageHeight = static_cast<int>(texH);
            } 
            else 
            {
                // fallback: keep 0s (or log)
            }
        }

        // Minimum required properties (x, y, width, height now optional)
        std::vector<std::string> missing;
        if (!config["name"].valid()) missing.push_back("name");
        if (!config["type"].valid()) missing.push_back("type");

        if (!missing.empty()) 
        {
            std::string msg = "Factory::create: Missing required property(s) in Lua config: ";
            for (size_t i = 0; i < missing.size(); ++i) 
            {
                msg += "'" + missing[i] + "'";
                if (i < missing.size() - 1) msg += ", ";
            }
            ERROR(msg); // Throws and aborts creation
            return DisplayHandle(); // This line is not reached if ERROR throws
        }

        // Prevent creating an object with a name that already exists in the factory
        std::string requestedName = config["name"];
        if (!requestedName.empty()) 
        {
            auto existing = displayObjects_.find(requestedName);
            if (existing != displayObjects_.end()) 
            {
                ERROR(std::string("Factory::create: Display object with name '") + requestedName + "' already exists");
                return DisplayHandle();
            }
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
            if (!displayObject) 
            {
                std::cout << "Factory::create: Failed to create display object of type '" << typeName
                        << "' from Lua. Display object is nullptr.\n";
                return DisplayHandle(); // Invalid handle
            }
            std::string name = config["name"];
            // Double-check before inserting (race-safe within this thread)
            if (!name.empty() && displayObjects_.find(name) != displayObjects_.end()) 
            {
                ERROR(std::string("Factory::create: Display object with name '") + name + "' already exists (insertion aborted)");
                return DisplayHandle();
            }
            displayObjects_[name] = std::move(displayObject);
            displayObjects_[name]->setType(typeName); // Ensure type is set

            // --- Set common properties from config ---
            if (config["color"].valid())
            {
                SDL_Color color = parseColor(config["color"]);
                displayObjects_[name]->color_ = color;
            }
            // set anchorPoints if present
            auto setAnchorFromConfig = [](const sol::object& obj, auto setter) 
            {
                if (obj.valid()) 
                {
                    if (obj.is<std::string>()) 
                    {
                        std::string key = SDOM::normalizeAnchorString(obj.as<std::string>());
                        auto it = SDOM::stringToAnchorPoint_.find(key);
                        if (it != SDOM::stringToAnchorPoint_.end())
                            setter(it->second);
                    } 
                    else if (obj.is<int>()) 
                    {
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

            // Dispatch EventType::OnInit so global or pre-registered listeners can observe creation
            // Included for completeness.  There may still be room to add an event listener.
            {
                auto& eventManager = getCore().getEventManager();
                std::unique_ptr<Event> initEvent = std::make_unique<Event>(EventType::OnInit, DisplayHandle(name, typeName));
                // relatedTarget: stage handle when available
                DisplayHandle stageHandle = getCore().getRootNode();
                if (stageHandle) initEvent->setRelatedTarget(stageHandle);
                eventManager.dispatchEvent(std::move(initEvent), DisplayHandle(name, typeName));
            }

            // If the Lua config included a parent, attach this new object to it
            if (config["parent"].valid()) {
                attachCreatedObjectToParentFromConfig(name, typeName, config["parent"]);
            }

            return DisplayHandle(name, typeName);
        }
        return DisplayHandle(); // Invalid handle
    }

    // InitStruct based object creator
    DisplayHandle Factory::create(const std::string& typeName, const IDisplayObject::InitStruct& init)
    {
        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromInitStruct) 
        {
            // If an explicit name was provided, ensure it doesn't already exist
            if (!init.name.empty() && displayObjects_.find(init.name) != displayObjects_.end()) {
                ERROR(std::string("Factory::create(init): Display object with name '") + init.name + "' already exists");
                return DisplayHandle();
            }

            auto displayObject = it->second.fromInitStruct(init);
            if (displayObject) 
            {
                std::string name = init.name;
                displayObject->onInit(); // Initialize the display object
                // Dispatch EventType::OnInit for this newly-initialized object
                // Included for completeness.  There may still be room to add an event listener.
                {
                    auto& eventManager = getCore().getEventManager();
                    std::unique_ptr<Event> initEvent = std::make_unique<Event>(EventType::OnInit, DisplayHandle(name, typeName));
                    DisplayHandle stageHandle = getCore().getRootNode();
                    if (stageHandle) 
                        initEvent->setRelatedTarget(stageHandle);
                    eventManager.dispatchEvent(std::move(initEvent), DisplayHandle(name, typeName));
                }
                displayObject->setType(typeName); // Ensure type is set
                displayObjects_[name] = std::move(displayObject);
                return DisplayHandle(name, typeName);
            }
        }
        return DisplayHandle(); // Invalid handle
    }

    DisplayHandle Factory::create(const std::string& typeName, const std::string& luaScript) 
    {
        sol::state lua;
        lua.open_libraries(sol::lib::base);

        // Try to parse and execute the string as a Lua table
        sol::object result = lua.script("return " + luaScript, sol::script_pass_on_error);

        if (!result.valid() || !result.is<sol::table>()) 
        {
            std::cout << "Factory::create: Provided string is not a valid Lua table.\n";
            return DisplayHandle();
        }

        sol::table config = result.as<sol::table>();
        return create(typeName, config);
    }

    AssetHandle Factory::createAsset(const std::string& typeName, const sol::table& config)
    {
        // Check required fields
        if (!config["name"].valid() || !config["type"].valid() || !config["filename"].valid()) 
        {
            ERROR("Factory::createAsset: Missing required property(s) in Lua config: 'name' or 'type' or 'filename'");
            return AssetHandle();
        }
        std::string requestedName = config["name"];
        if (assetObjects_.find(requestedName) != assetObjects_.end()) {
            ERROR("Factory::createAsset: Asset object with name '" + requestedName + "' already exists");
            return AssetHandle();
        }
        std::string filename = config["filename"];
        if (filename.empty()) {
            ERROR("Factory::createAsset: 'filename' cannot be empty");
            return AssetHandle();
        }
            
        auto it = assetCreators_.find(typeName);
        if (it != assetCreators_.end() && it->second.fromLua) 
        {
            auto assetObj = it->second.fromLua(config);
            if (!assetObj) 
            {
                ERROR("Factory::createAsset: Failed to create asset object of type '" + typeName + "' from Lua.");
                return AssetHandle();
            }
            assetObj->setType(typeName);
            assetObjects_[requestedName] = std::move(assetObj);
            assetObjects_[requestedName]->onInit();

            // Register Lua bindings for this concrete asset instance so its methods are available
            try {
                assetObjects_[requestedName]->registerLuaBindings(typeName, SDOM::getLua());
            } catch(...) {
                ERROR("Factory::createAsset: Failed to register Lua bindings for asset: " + requestedName);
            }
            
            return AssetHandle(requestedName, typeName, filename);
        }
        return AssetHandle();
    }

    AssetHandle Factory::createAsset(const std::string& typeName, const IAssetObject::InitStruct& init)
    {
        if (assetObjects_.find(init.name) != assetObjects_.end()) {
            ERROR("Factory::createAsset(init): Asset object with name '" + init.name + "' already exists");
            return AssetHandle();
        }
        auto it = assetCreators_.find(typeName);
        if (it != assetCreators_.end() && it->second.fromInitStruct) {
            auto assetObj = it->second.fromInitStruct(init);
            if (assetObj) {
                assetObj->setType(typeName);
                assetObjects_[init.name] = std::move(assetObj);
                assetObjects_[init.name]->onInit();
                // Register per-instance Lua bindings so Lua-visible methods exist on the handle
                try {
                    assetObjects_[init.name]->registerLuaBindings(typeName, SDOM::getLua());
                } catch(...) {
                    ERROR("Factory::createAsset(init): Failed to register Lua bindings for asset: " + init.name);
                }
                
                return AssetHandle(init.name, typeName, init.filename);
            }
        }
        return AssetHandle();
    }

    AssetHandle Factory::createAsset(const std::string& typeName, const std::string& luaScript)
    {
        sol::state lua;
        lua.open_libraries(sol::lib::base);
        sol::object result = lua.script("return " + luaScript, sol::script_pass_on_error);
        if (!result.valid() || !result.is<sol::table>()) {
            ERROR("Factory::createAsset: Provided string is not a valid Lua table.");
            return AssetHandle();
        }
        sol::table config = result.as<sol::table>();
        return createAsset(typeName, config);
    }


    bool Factory::attachCreatedObjectToParentFromConfig(const std::string& name, const std::string& typeName, const sol::object& parentConfig)
    {
        DisplayHandle parentHandle;
        // Try to coerce the parentConfig into a DisplayHandle in several ways.
        try {
            // First, attempt a direct conversion (works even if is<> returned false)
            parentHandle = parentConfig.as<DisplayHandle>();
        } catch(...) {
            // ignored
        }
        if (!parentHandle) {
            try {
                if (parentConfig.is<std::string>()) {
                    parentHandle = getDisplayObject(parentConfig.as<std::string>());
                }
            } catch(...) {}
        }
        if (!parentHandle && parentConfig.is<sol::table>()) {
            try {
                // Prefer the centralized resolver on DisplayHandle which knows how to
                // interpret userdata, string names, and nested tables. This avoids
                // unsafe direct get<DisplayHandle>() calls on userdata that may be a
                // different concrete usertype (e.g. Stage*) and which can trigger
                // sol2 type-errors.
                parentHandle = DisplayHandle::resolveChildSpec(parentConfig);

                // If resolveChildSpec didn't find a handle, try a conservative
                // approach: if the table exposes a getName() function, call it
                // to obtain a name to look up in the factory.
                if (!parentHandle) {
                    sol::table t = parentConfig.as<sol::table>();
                    sol::object maybeGetName = t["getName"];
                    if (maybeGetName.valid() && maybeGetName.get_type() == sol::type::function) {
                        try {
                            sol::protected_function f = maybeGetName.as<sol::protected_function>();
                            sol::protected_function_result r = f(t);
                            if (r.valid()) {
                                try {
                                    std::string resolved = r.get<std::string>();
                                    parentHandle = getDisplayObject(resolved);
                                } catch(...) {}
                            }
                        } catch(...) {}
                    }
                }
            } catch(...) {}
        }
        // If parentConfig is a string, interpret as a name lookup
        if (!parentHandle) {
            try {
                if (parentConfig.is<std::string>()) {
                    parentHandle = getDisplayObject(parentConfig.as<std::string>());
                }
            } catch(...) {}
        }

        // If it's a table (or any other form), prefer the centralized resolver
        // which understands tables like { parent = <handle|name> } and nested
        // descriptors. This avoids unsafe direct get<DisplayHandle>() calls on
        // userdata that may be a different concrete usertype.
        if (!parentHandle && parentConfig.is<sol::table>()) {
            try {
                parentHandle = DisplayHandle::resolveChildSpec(parentConfig);

                // If resolveChildSpec didn't find a handle, try a conservative
                // approach: if the table exposes a getName() function, call it
                // to obtain a name to look up in the factory.
                if (!parentHandle) {
                    sol::table t = parentConfig.as<sol::table>();
                    sol::object maybeGetName = t.get<sol::object>("getName");
                    if (maybeGetName.valid() && maybeGetName.get_type() == sol::type::function) {
                        try {
                            sol::protected_function f = maybeGetName.as<sol::protected_function>();
                            sol::protected_function_result r = f(t);
                            if (r.valid()) {
                                try {
                                    std::string resolved = r.get<std::string>();
                                    parentHandle = getDisplayObject(resolved);
                                } catch(...) {}
                            }
                        } catch(...) {}
                    }
                }
            } catch(...) {}
        }

        if (!parentHandle || !parentHandle.get()) {
            return false;
        }

        if (auto* newParentObj = dynamic_cast<IDisplayObject*>(parentHandle.get())) 
        {
            // Use addChild to ensure parent/child bookkeeping and ordering occurs
            newParentObj->addChild(DisplayHandle(name, typeName));
            return true;
        }
        return false;
    }


    void Factory::addDisplayObject(const std::string& name, 
        std::unique_ptr<IDisplayObject> displayObject) 
    {
        if (name.empty()) {
            ERROR("Factory::addDisplayObject: cannot add object with empty name");
            return;
        }
        if (displayObjects_.find(name) != displayObjects_.end()) {
            ERROR(std::string("Factory::addDisplayObject: Display object with name '") + name + "' already exists (add aborted)");
            return;
        }
        displayObjects_[name] = std::move(displayObject);
    }
    
    void Factory::destroyDisplayObject(const std::string& name) 
    {
        displayObjects_.erase(name);
    }

    void Factory::destroyAssetObject(const std::string& name) 
    {
        auto it = assetObjects_.find(name);
        if (it == assetObjects_.end()) return;

        // Ask the object to release runtime resources first
        try {
            if (it->second) {
                it->second->onUnload();
                it->second->onQuit();
            }
        } catch(...) {
            ERROR("Factory::destroyAssetObject: exception while shutting down asset: " + name);
        }

        // Finally remove from registry (unique_ptr destructor will run)
        assetObjects_.erase(it);
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

    std::vector<DisplayHandle> Factory::getOrphanedDisplayObjects() {
        std::vector<DisplayHandle> orphans;
        for (const auto& [name, objPtr] : displayObjects_) {
            auto obj = dynamic_cast<IDisplayObject*>(objPtr.get());
            if (obj && !obj->getParent() && obj->getType() != "Stage") {
                orphans.push_back(getDisplayObject(name));
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
        for (const auto& pair : displayObjects_) 
        {
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
        for (const auto& pair : displayObjects_) 
        {
            const auto& name = pair.first;
            const auto& displayObject = pair.second;
            std::cout << "  Name: " << name
                    << ", Type: " << (displayObject ? displayObject->getType() : "Unknown")
                    << "\n";
        }
        std::cout << "Total display objects: " << displayObjects_.size() << std::endl;
    }

    void Factory::printAssetRegistry() const
    {
        std::cout << "Factory Asset Object Registry:\n";
        for (const auto& pair : assetObjects_) 
        {
            const auto& name = pair.first;
            const auto& assetObject = pair.second;
            std::cout << "  Name: " << name
                    << ", Type: " << (assetObject ? assetObject->getType() : "Unknown")
                    << ", \n  Filename: " << (assetObject ? assetObject->getFilename() : "Unknown")
                    << "\n";
        }
        std::cout << "Total asset objects: " << assetObjects_.size() << std::endl;
    }

    

    // ----- Orphan and Future Child Management -----

    void Factory::detachOrphans()
    {
        for (auto& orphanHandle : orphanList_)
        {
            DisplayHandle orphan = orphanHandle;
            if (orphan)
            {
                DisplayHandle parent = orphan->getParent();
                if (parent)
                {
                    // Remove orphan from parent's children using public removeChild method
                    parent->removeChild(orphanHandle);
                    // Set orphan's parent to nullptr using public setParent method
                    orphan->setParent(DisplayHandle());
                }
            }
        }
        orphanList_.clear();
    }

    void Factory::attachFutureChildren() 
    {
        for (auto& futureChild : futureChildrenList_) 
        {
            DisplayHandle child = futureChild.child;
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

    void Factory::addToOrphanList(const DisplayHandle orphan) 
    {
        if (orphan) 
        {
            orphanList_.push_back(orphan);
        }
    }

    void Factory::addToFutureChildrenList(const DisplayHandle child, const DisplayHandle parent, bool useWorld, int worldX, int worldY) 
    {
        if (child && parent) 
        {

            // std::cout << "child added to queue: " << child->getName() << " to (" << 
            //     worldX << ", " << worldY << ", w:" << child->getWidth() 
            //     << ", h:" << child->getHeight() << ")" << std::endl; 

            // Prevent identical future child entries (same child name and parent name)
            bool duplicate = false;
            try {
                std::string childName = child.getName();
                std::string parentName = parent.getName();
                for (const auto& fc : futureChildrenList_) {
                    if (fc.child && fc.parent) {
                        try {
                            if (fc.child.getName() == childName && fc.parent.getName() == parentName) { duplicate = true; break; }
                        } catch(...) {}
                    }
                }
            } catch(...) { duplicate = false; }
            if (!duplicate) futureChildrenList_.push_back({child, parent, useWorld, worldX, worldY});
        }
    }


    // Provide a simple onUnitTest implementation to satisfy linker and
    // maintain the IDataObject interface. This was accidentally removed
    // during cleanup; keep it lightweight for now.
    bool Factory::onUnitTest()
    {
        // No special unit test behavior required for Factory at the moment.
        return true;
    }




} // namespace SDOM

