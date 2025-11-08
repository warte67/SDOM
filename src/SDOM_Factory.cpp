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
#include <iomanip>
#include <algorithm>
#include <SDOM/SDOM_Frame.hpp>
#include <SDOM/SDOM_Button.hpp>
#include <SDOM/SDOM_Group.hpp>
#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_ArrowButton.hpp>
#include <SDOM/SDOM_CheckButton.hpp>
#include <SDOM/SDOM_RadioButton.hpp>
#include <SDOM/SDOM_TristateButton.hpp>
#include <SDOM/SDOM_Slider.hpp>
#include <SDOM/SDOM_ProgressBar.hpp>
#include <SDOM/SDOM_ScrollBar.hpp>

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
        // Ensure the central Lua bindings root exists before any types register
        try {
            sol::state& lua = SDOM::getLua();
            if (!lua["SDOM_Bindings"].valid()) {
                sol::table bindingsRoot = lua.create_table();
                lua["SDOM_Bindings"] = bindingsRoot;
                // std::cout << "[Factory::onInit] created SDOM_Bindings root table" << std::endl;
            } else {
                // std::cout << "[Factory::onInit] SDOM_Bindings root already present" << std::endl;
            }
        } catch(...) {}
        // --- Lua UserType Registration --- //
        Core& core = getCore();
        // core._registerLuaBindings("Core", core.getLua());

        // Register the DisplayHandle early so any returned handles to Lua
        // have a valid usertype/metatable for indexing minimal helpers.
        try {
            DisplayHandle prototypeHandle; // Default DisplayHandle for registration
            prototypeHandle.registerLuaBindings("DisplayHandle", core.getLua());
        } catch(...) {}

            TTFAsset::InitStruct ttf_init;
            ttf_init.name = "internal_ttf_asset";     // internal registry key
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

        // register a few internal icon Textures (skip if already created)
        {
            auto ensureTexture = [&](const std::string& defaultIconName) {
                if (!getResObj(defaultIconName)) {
                    Texture::InitStruct init;
                    init.name = defaultIconName;
                    init.type = "Texture";
                    init.filename = defaultIconName;
                    AssetHandle spriteSheet = createAsset("Texture", init);
                    if (auto* spriteSheetPtr = spriteSheet.as<Texture>()) {
                        try { spriteSheetPtr->_registerLuaBindings("Texture", core.getLua()); } catch(...) {}
                    }
                } else {
                    // Ensure Lua bindings are registered for existing instance
                    IAssetObject* existing = getResObj(defaultIconName);
                    if (auto* spriteSheetPtr = dynamic_cast<Texture*>(existing)) {
                        try { spriteSheetPtr->_registerLuaBindings("Texture", core.getLua()); } catch(...) {}
                    }
                }
            };

            ensureTexture("internal_icon_8x8");
            ensureTexture("internal_icon_12x12");
            ensureTexture("internal_icon_16x16");
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
        if (!getResObj("internal_ttf_asset")) {
            TTFAsset::InitStruct ttf_init;
            ttf_init.name = "internal_ttf_asset";     // internal registry key
            ttf_init.type = TTFAsset::TypeName;     // concrete type name
            ttf_init.filename = "internal_ttf";      // internal ttf filename
            ttf_init.isInternal = true;             // is internal
            ttf_init.internalFontSize = 10;         // member name in InitStruct
            AssetHandle ttfFontAsset = createAsset("TTFAsset", ttf_init);
            (void)ttfFontAsset;
        }

        // Create a public truetype font asset named "default_ttf" which references the internal TTFAsset
        if (!getResObj("internal_ttf")) {
            TruetypeFont::InitStruct ttInit;
            ttInit.name = "internal_ttf";           // public registry key expected by Label
            ttInit.type = TruetypeFont::TypeName;
            ttInit.filename = "internal_ttf_asset";      // filename used by TruetypeFont to find the TTFAsset

            // Attempt to create the truetype font; if TTF isn't available or creation fails, fall back to bitmap font
            AssetHandle defaultTTFont;
            try {
                defaultTTFont = createAsset(TruetypeFont::TypeName, ttInit);
            } catch (...) {
                // If creation failed, leave defaultTTFont invalid and fall back below
                defaultTTFont = AssetHandle();
            }
        }
            

        // register the SpriteSheet asset
        registerResType("SpriteSheet", AssetTypeCreators{
            SpriteSheet::CreateFromLua,
            SpriteSheet::CreateFromInitStruct
        });

        // Also ensure there is a SpriteSheet wrapper for the internal icon texture
        {
            auto ensureSpriteSheet = [&](const std::string& spriteSheetName, const std::string& textureFilename, int spriteW, int spriteH) {
                if (!getResObj(spriteSheetName)) {
                    SpriteSheet::InitStruct ssInit;
                    ssInit.name = spriteSheetName;
                    ssInit.type = SpriteSheet::TypeName;
                    ssInit.filename = textureFilename; // underlying texture filename
                    ssInit.spriteWidth = spriteW;
                    ssInit.spriteHeight = spriteH;
                    ssInit.isInternal = true;
                    AssetHandle ss = createAsset("SpriteSheet", ssInit);
                    (void)ss;
                }
            };

            ensureSpriteSheet("internal_icon_8x8_SpriteSheet",  "internal_icon_8x8",  8,  8);
            ensureSpriteSheet("internal_icon_12x12_SpriteSheet", "internal_icon_12x12", 12, 12);
            ensureSpriteSheet("internal_icon_16x16_SpriteSheet", "internal_icon_16x16", 16, 16);
        }



        // register the BitmapFont asset
        registerResType("BitmapFont", AssetTypeCreators{
            BitmapFont::CreateFromLua,
            BitmapFont::CreateFromInitStruct
        });

        // Create the default 8x8 bitmap font asset if it doesn't exist.
        // Use the filename as the canonical resource name so Lua can refer to
        // the font simply as "internal_font_8x8". The BitmapFont will create
        // (or reuse) an internal SpriteSheet named '<filename>_SpriteSheet'.
        if (!getResObj("internal_font_8x8")) {
            BitmapFont::InitStruct init;
            init.name = "internal_font_8x8";      // registry key == filename
            init.type = BitmapFont::TypeName;   // concrete type name
            init.filename = "internal_font_8x8";  // underlying texture filename
            init.isInternal = true;             // is an internal resource
            init.fontSize = 8;                  // member name in InitStruct
            init.font_width = 8;                 // font_width for this resource
            init.font_height = 8;                // font_height for this resource            
            AssetHandle bmpFont = createAsset("BitmapFont", init);
            (void)bmpFont;
        }
        if (!getResObj("internal_font_8x12")) {
            BitmapFont::InitStruct init;
            init.name = "internal_font_8x12";     // registry key == filename
            init.type = BitmapFont::TypeName;   // concrete type name
            init.filename = "internal_font_8x12"; // underlying texture filename
            init.isInternal = true;             // is an internal resource
            // Set both BitmapFont-specific fontSize and the base IFontObject's
            // fontSize_ so the inherited constructor sees the correct value.
            init.fontSize = 12;                 // BitmapFont::InitStruct field
            init.fontSize_ = 12;                // IFontObject::InitStruct field used by base ctor
            init.font_width = 8;                 // font_width for this resource
            init.font_height = 12;               // font_height for this resource
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

        // --- Register the IPanelObject Decendants --- //

        // register Frame
        registerDomType("Frame", TypeCreators{
            Frame::CreateFromLua,
            Frame::CreateFromInitStruct
        });

        // register Button
        registerDomType("Button", TypeCreators{
            Button::CreateFromLua,
            Button::CreateFromInitStruct
        }); 

        // register Group
        registerDomType("Group", TypeCreators{
            Group::CreateFromLua,
            Group::CreateFromInitStruct
        });

        // register the IconButton
        registerDomType("IconButton", TypeCreators{
            IconButton::CreateFromLua,
            IconButton::CreateFromInitStruct
        });

        // register the ArrowButton
        registerDomType("ArrowButton", TypeCreators{
            ArrowButton::CreateFromLua,
            ArrowButton::CreateFromInitStruct
        });

        // register CheckButton
        registerDomType("CheckButton", TypeCreators{
            CheckButton::CreateFromLua,
            CheckButton::CreateFromInitStruct
        });

        // register RadioButton
        registerDomType("RadioButton", TypeCreators{
            RadioButton::CreateFromLua,
            RadioButton::CreateFromInitStruct
        });

        // register the TristateButton
        registerDomType("TristateButton", TypeCreators{
            TristateButton::CreateFromLua,
            TristateButton::CreateFromInitStruct
        });

        // register the Slider
        registerDomType("Slider", TypeCreators{
            Slider::CreateFromLua,
            Slider::CreateFromInitStruct
        });

        // register the ProgressBar
        registerDomType("ProgressBar", TypeCreators{
            ProgressBar::CreateFromLua,
            ProgressBar::CreateFromInitStruct
        });

        // register the ScrollBar
        registerDomType("ScrollBar", TypeCreators{
            ScrollBar::CreateFromLua,
            ScrollBar::CreateFromInitStruct
        });


        // return initialized state
        initialized_ = true;

        // Forbid global reassignment of SDOM_Bindings (root swap). Deny any
        // attempt to set _G["SDOM_Bindings"] after initialization.
        try {
            sol::state& lua = SDOM::getLua();
            sol::table globals = lua.globals();
            sol::table gmt = lua.create_table();
            gmt[sol::meta_function::new_index] = [](sol::table g, const sol::object& key, const sol::object& value) mutable {
                try {
                    if (key.valid() && key.is<std::string>() && key.as<std::string>() == std::string("SDOM_Bindings")) {
                        std::cout << "[_G Guard] Deny reassignment of SDOM_Bindings" << std::endl;
                        ERROR("Attempted reassignment of global SDOM_Bindings");
                        return;
                    }
                } catch(...) {}
                g.raw_set(key, value);
            };
            globals[sol::metatable_key] = gmt;
        } catch(...) {}

        // Protect SDOM_Bindings from replacement/deletion of per-type tables by
        // wrapping it in a proxy with strict __index/__newindex behavior.
        try {
            sol::state& lua = SDOM::getLua();
            sol::object rootObj = lua["SDOM_Bindings"];
            sol::table storage;
            if (rootObj.valid() && rootObj.is<sol::table>()) {
                storage = rootObj.as<sol::table>();
            } else {
                storage = lua.create_table();
            }

            // Proxy table that exposes storage but intercepts writes
            sol::table proxy = lua.create_table();
            sol::table mt = lua.create_table();

            // __index forwards to storage
            mt[sol::meta_function::index] = [storage](sol::table /*t*/, const sol::object& key, sol::this_state s) mutable -> sol::object {
                sol::state_view L(s);
                if (!key.valid() || !key.is<std::string>()) return sol::make_object(L, sol::lua_nil);
                std::string ks;
                try { ks = key.as<std::string>(); } catch(...) { return sol::make_object(L, sol::lua_nil); }
                sol::object v = storage.raw_get_or(ks, sol::lua_nil);
                if (v.valid() && v != sol::lua_nil) return v;
                return sol::make_object(L, sol::lua_nil);
            };

            // __newindex only allows first-time assignment of a table; denies replacement/deletion
            mt[sol::meta_function::new_index] = [storage](sol::table /*t*/, const sol::object& key, const sol::object& value) mutable {
                try {
                    if (!key.valid() || !key.is<std::string>()) {
                        // Non-string key: forward raw
                        return; // ignore silently
                    }
                    std::string ks = key.as<std::string>();
                    sol::object existed = storage.raw_get_or(ks, sol::lua_nil);
                    if (value == sol::lua_nil) {
                        std::cout << "[SDOM_Bindings] Deny delete key='" << ks << "'" << std::endl;
                        ERROR(std::string("Attempted to delete per-type table '") + ks + "' in SDOM_Bindings");
                        return;
                    }
                    if (existed.valid() && existed != sol::lua_nil) {
                        std::cout << "[SDOM_Bindings] Deny replace key='" << ks << "'" << std::endl;
                        ERROR(std::string("Attempted to replace per-type table '") + ks + "' in SDOM_Bindings");
                        return;
                    }
                    if (!value.is<sol::table>()) {
                        int kind = static_cast<int>(value.get_type());
                        std::cout << "[SDOM_Bindings] Deny non-table assign key='" << ks << "' kind=" << kind << std::endl;
                        ERROR(std::string("Non-table assigned to SDOM_Bindings['") + ks + "']");
                        return;
                    }
                    sol::table vtbl = value.as<sol::table>();
                    // Stamp guard if absent
                    try {
                        sol::object guard = vtbl.raw_get_or("__sdom_guard", sol::lua_nil);
                        if (!guard.valid() || guard == sol::lua_nil) vtbl["__sdom_guard"] = true;
                    } catch(...) {}
                    storage.raw_set(ks, vtbl);
                } catch(...) {}
            };

            proxy[sol::metatable_key] = mt;
            // Expose storage for diagnostics only (read-only by convention)
            lua["_SDOM_Bindings_Storage"] = storage;
            lua["SDOM_Bindings"] = proxy;
        } catch(...) {}

        // Diagnostic: list everything in SDOM_Bindings["Group"] (disabled by default)
        if (false)
        {
            try {
                sol::state& lua = SDOM::getLua();
                sol::table bindingsRoot = lua["SDOM_Bindings"];
                if (bindingsRoot.valid() && bindingsRoot["Group"].valid()) {
                    sol::table g = bindingsRoot["Group"];
                    sol::function tostring_fn = lua["tostring"];
                    std::cout << "[Factory::onInit] SDOM_Bindings['Group'] contents:\n";
                    for (auto& kv : g) {
                        sol::object key = kv.first;
                        sol::object val = kv.second;
                        std::string keyStr;
                        try {
                            if (key.valid()) {
                                if (key.is<std::string>()) keyStr = key.as<std::string>();
                                else keyStr = tostring_fn(key);
                            } else {
                                keyStr = "<invalid key>";
                            }
                        } catch(...) { keyStr = "<key?>"; }

                        std::string valStr;
                        try {
                            if (val.valid()) valStr = tostring_fn(val);
                            else valStr = "<nil>";
                        } catch(...) { valStr = "<value?>"; }

                        bool rawPresent = false;
                        try {
                            if (key.is<std::string>()) {
                                sol::object raw = g.raw_get_or(key.as<std::string>(), sol::lua_nil);
                                rawPresent = (raw.valid() && raw != sol::lua_nil);
                            }
                        } catch(...) { rawPresent = false; }

                        int valType = static_cast<int>(val.get_type());
                        std::cout << "  key='" << keyStr << "' raw=" << (rawPresent ? "Y" : "N")
                                << " type=" << valType << " val=" << valStr << "\n";
                    }
                } else {
                    std::cout << "[Factory::onInit] Group bindings table missing entirely\n";
                }
            } catch(...) {}
        }

        return initialized_;
    } // END: bool Factory::onInit()


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
                    if (DEBUG_REGISTER_LUA) {
                        printf("[TRACE] Factory::registerDomType prototype->_registerLuaBindings for type='%s' lua_state=%p\n",
                               typeName.c_str(), (void*)SDOM::getLua().lua_state());
                    }
                    // Fail-fast: do not swallow errors
                    prototype->_registerLuaBindings(typeName, SDOM::getLua());
                    // prototype unique_ptr will be destroyed here; no need to
                    // insert into the factory registry.
                }
            } catch(const std::exception& e) {
                ERROR(std::string("Factory::registerDomType binding failed for type '") + typeName + "': " + e.what());
            } catch(...) {
                ERROR(std::string("Factory::registerDomType binding failed for type '") + typeName + "' (unknown error)");
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

        // // Prevent creating an object with a name that already exists in the factory
        // std::string requestedName = config["name"];
        // if (!requestedName.empty()) 
        // {
        //     auto existing = displayObjects_.find(requestedName);
        //     if (existing != displayObjects_.end()) 
        //     {
        //         ERROR(std::string("Factory::create: Display object with name '") + requestedName + "' already exists");
        //         return DisplayHandle();
        //     }
        // }

        // If the name already exists, return a DisplayHandle to the existing object
        std::string requestedName = config["name"];
        if (!requestedName.empty())
        {
            auto existing = displayObjects_.find(requestedName);
            if (existing != displayObjects_.end())
            {
                // ✅ Return reference/alias without re-initializing
                return DisplayHandle(requestedName, typeName);
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
                // std::cout << "Factory::create: Failed to create display object of type '" << typeName
                //         << "' from Lua. Display object is nullptr.\n";
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

    // // InitStruct based object creator
    // DisplayHandle Factory::create(const std::string& typeName, const IDisplayObject::InitStruct& init)
    // {
    //     auto it = creators_.find(typeName);
    //     if (it != creators_.end() && it->second.fromInitStruct) 
    //     {
    //         // If an explicit name was provided, ensure it doesn't already exist
    //         if (!init.name.empty() && displayObjects_.find(init.name) != displayObjects_.end()) {
    //             ERROR(std::string("Factory::create(init): Display object with name '") + init.name + "' already exists");
    //             return DisplayHandle();
    //             //return DisplayHandle(init.name, typeName);
    //         }

    //         auto displayObject = it->second.fromInitStruct(init);
    //         if (displayObject) 
    //         {
    //             std::string name = init.name;
    //             // Insert into the registry and set type BEFORE calling onInit so
    //             // child objects created during onInit can locate their parent
    //             // via the Factory (getDomObj/getDisplayObject).
    //             displayObject->setType(typeName); // Ensure type is set
    //             displayObjects_[name] = std::move(displayObject);

    //             // Initialize the display object now that it's in the registry
    //             displayObjects_[name]->onInit();

    //             // Dispatch EventType::OnInit for this newly-initialized object
    //             {
    //                 auto& eventManager = getCore().getEventManager();
    //                 std::unique_ptr<Event> initEvent = std::make_unique<Event>(EventType::OnInit, DisplayHandle(name, typeName));
    //                 DisplayHandle stageHandle = getCore().getRootNode();
    //                 if (stageHandle) 
    //                     initEvent->setRelatedTarget(stageHandle);
    //                 eventManager.dispatchEvent(std::move(initEvent), DisplayHandle(name, typeName));
    //             }

    //             return DisplayHandle(name, typeName);
    //         }
    //     }
    //     return DisplayHandle(); // Invalid handle
    // }

    DisplayHandle Factory::create(const std::string& typeName,
                                const IDisplayObject::InitStruct& init)
    {
        auto it = creators_.find(typeName);
        if (it != creators_.end() && it->second.fromInitStruct)
        {
            // If name already exists, return an alias (no re-init)
            if (!init.name.empty())
            {
                auto existing = displayObjects_.find(init.name);
                if (existing != displayObjects_.end())
                {
                    return DisplayHandle(init.name, typeName);
                }
            }

            // Otherwise create a new object
            auto displayObject = it->second.fromInitStruct(init);
            if (displayObject)
            {
                std::string name = init.name;

                displayObject->setType(typeName);
                displayObjects_[name] = std::move(displayObject);

                // Run initialization callback now that registry entry exists
                displayObjects_[name]->onInit();

                // Dispatch OnInit event
                auto& eventManager = getCore().getEventManager();
                std::unique_ptr<Event> initEvent =
                    std::make_unique<Event>(EventType::OnInit, DisplayHandle{name, typeName});

                DisplayHandle stageHandle = getCore().getRootNode();
                if (stageHandle)
                    initEvent->setRelatedTarget(stageHandle);

                eventManager.dispatchEvent(std::move(initEvent), DisplayHandle(name, typeName));

                return DisplayHandle(name, typeName);
            }
        }

        return DisplayHandle(); // invalid
    }



    DisplayHandle Factory::create(const std::string& typeName, const std::string& luaScript) 
    {
        // Use the global SDOM Lua state instead of creating a fresh state. This
        // ensures created tables/functions are visible to the rest of the
        // application and preserves registered bindings and globals.
        sol::state& lua = SDOM::getLua();

        // Try to parse and execute the string as a Lua table
        // sol::object result = lua.script("return " + luaScript, sol::script_pass_on_error);

        // sol::table result = lua.safe_script(luaScript, sol::script_pass_on_error).get<sol::table>();        
        sol::object result = lua.safe_script("return {" + luaScript + "}", sol::script_pass_on_error).get<sol::object>();        

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

        
        // std::string requestedName = config["name"];
        // if (assetObjects_.find(requestedName) != assetObjects_.end()) {
        //     // ERROR("Factory::createAsset: Asset object with name '" + requestedName + "' already exists");
        //     return AssetHandle();
        // }
        std::string requestedName = config["name"];
        if (auto itExisting = assetObjects_.find(requestedName); itExisting != assetObjects_.end())
        {
            // Return a new handle pointing to the existing asset
            return AssetHandle(requestedName, itExisting->second->getType(), itExisting->second->getFilename());
        }    
        
        
        std::string filename = config["filename"];
        if (filename.empty()) {
            ERROR("Factory::createAsset: 'filename' cannot be empty");
            return AssetHandle();
        }
        // If an asset already exists for this filename, consider reusing it.
        // Special-case TTFAsset: only reuse if font size matches the requested size
            // Only certain asset types should be reused by filename. Avoid reusing
            // wrapper types like 'TruetypeFont' which are per-resource wrappers.
            auto shouldReuseByFilename = [&](const std::string& t) -> bool {
                return (t == "Texture" || t == TTFAsset::TypeName);
            };

            if (shouldReuseByFilename(typeName) && typeName == TTFAsset::TypeName) {
            int desiredSize = 0;
            try {
                if (config["internalFontSize"].valid()) desiredSize = config["internalFontSize"].get<int>();
                else if (config["internal_font_size"].valid()) desiredSize = config["internal_font_size"].get<int>();
                else if (config["fontSize"].valid()) desiredSize = config["fontSize"].get<int>();
                else if (config["font_size"].valid()) desiredSize = config["font_size"].get<int>();
                else if (config["size"].valid()) desiredSize = config["size"].get<int>();
            } catch(...) { desiredSize = 0; }

            if (desiredSize > 0) {
                for (const auto& pair : assetObjects_) {
                    const auto& assetPtr = pair.second;
                    if (!assetPtr) continue;
                    try {
                        if (assetPtr->getFilename() == filename && assetPtr->getType() == TTFAsset::TypeName) {
                            TTFAsset* existingTTF = dynamic_cast<TTFAsset*>(assetPtr.get());
                            if (existingTTF && existingTTF->getFontSize() == desiredSize) {
                                AssetHandle out;
                                out.name_ = pair.first;
                                out.type_ = assetPtr->getType();
                                out.filename_ = assetPtr->getFilename();
                                // std::cout << "Factory::createAsset: Reusing existing TTFAsset '" << out.getName() << "' for filename '" << filename << "' (size=" << desiredSize << ")\n";
                                return out;
                            }
                        }
                    } catch(...) {}
                }
            }
            // If desiredSize == 0 we fall through and do not reuse to avoid size-mismatch surprises
        } else if (shouldReuseByFilename(typeName)) {
            AssetHandle existing = findAssetByFilename(filename, typeName);
            if (existing) {
                // std::cout << "Factory::createAsset: Reusing existing asset '" << existing.getName() << "' for filename '" << filename << "' (type=" << typeName << ")\n";
                return existing;
            }
        }
            
        // NOTE: Do not reuse SpriteSheet objects here. Even if a SpriteSheet with
        // matching params exists, create a fresh SpriteSheet instance so the
        // requested registry name becomes the asset's logical name. Texture
        // reuse (by filename) remains in place so heavy GPU resources are not
        // duplicated. This preserves existing unit-test expectations.

        auto it = assetCreators_.find(typeName);
        if (it != assetCreators_.end() && it->second.fromLua) 
        {
                auto uniqueAssetObj = it->second.fromLua(config);
                if (!uniqueAssetObj) 
                {
                    ERROR("Factory::createAsset: Failed to create asset object of type '" + typeName + "' from Lua.");
                    return AssetHandle();
                }
                uniqueAssetObj->setType(typeName);
                // convert to shared_ptr for aliasing support
                std::shared_ptr<IAssetObject> sharedAsset = std::move(uniqueAssetObj);
                assetObjects_[requestedName] = sharedAsset;
                assetObjects_[requestedName]->onInit();

                // Register Lua bindings for this concrete asset instance so its methods are available
                try {
                    assetObjects_[requestedName]->registerLuaBindings(typeName, SDOM::getLua());
                } catch(...) {
                    ERROR("Factory::createAsset: Failed to register Lua bindings for asset: " + requestedName);
                }

                // Eagerly load the asset now that it has been initialized and registered.
                try {
                    if (!assetObjects_[requestedName]->isLoaded()) {
                        assetObjects_[requestedName]->onLoad();
                    }
                } catch(...) {
                    ERROR("Factory::createAsset: onLoad() failed for asset: " + requestedName);
                }

                return AssetHandle(requestedName, typeName, filename);
        }
        return AssetHandle();
    }


    AssetHandle Factory::createAsset(const std::string& typeName, const IAssetObject::InitStruct& init)
    {
        // If the asset name already exists, return a handle to it.
        if (auto itExisting = assetObjects_.find(init.name); itExisting != assetObjects_.end())
        {
            return AssetHandle(init.name, itExisting->second->getType(), itExisting->second->getFilename());
        }

        // Reuse by filename for Texture / TTFAsset
        if (!init.filename.empty())
        {
            if (typeName == TTFAsset::TypeName)
            {
                try {
                    const auto& ttfInit = static_cast<const TTFAsset::InitStruct&>(init);
                    int desiredSize = ttfInit.internalFontSize;

                    for (const auto& [key, assetPtr] : assetObjects_)
                    {
                        if (!assetPtr) continue;
                        if (assetPtr->getFilename() == init.filename && assetPtr->getType() == TTFAsset::TypeName)
                        {
                            if (auto* existingTTF = dynamic_cast<TTFAsset*>(assetPtr.get()))
                            {
                                if (existingTTF->getFontSize() == desiredSize)
                                    return AssetHandle(key, assetPtr->getType(), assetPtr->getFilename());
                            }
                        }
                    }
                } catch(...) {}
            }
            else if (typeName == "Texture")
            {
                if (AssetHandle existing = findAssetByFilename(init.filename, typeName))
                    return existing;
            }
            else if (typeName == SpriteSheet::TypeName)
            {
                // Correct behavior: DO NOT reuse SpriteSheets, but allow texture reuse under the hood.
            }
        }

        // --- Create New Asset ---
        auto it = assetCreators_.find(typeName);
        if (it != assetCreators_.end() && it->second.fromInitStruct)
        {
            auto uniqueAssetObj = it->second.fromInitStruct(init);
            if (!uniqueAssetObj)
                return AssetHandle();

            uniqueAssetObj->setType(typeName);
            std::shared_ptr<IAssetObject> sharedAsset = std::move(uniqueAssetObj);

            assetObjects_[init.name] = sharedAsset;
            assetObjects_[init.name]->onInit();

            try {
                assetObjects_[init.name]->registerLuaBindings(typeName, SDOM::getLua());
            } catch(...) {
                ERROR("Factory::createAsset(init): Failed to register Lua bindings for asset: " + init.name);
            }

            try {
                if (!assetObjects_[init.name]->isLoaded())
                    assetObjects_[init.name]->onLoad();
            } catch(...) {
                ERROR("Factory::createAsset(init): onLoad() failed for asset: " + init.name);
            }

            return AssetHandle(init.name, typeName, init.filename);
        }

        return AssetHandle(); // invalid
    }


    AssetHandle Factory::createAsset(const std::string& typeName, const std::string& luaScript)
    {
        // Use global Lua state so created tables are visible to the rest of the app
        sol::state& lua = SDOM::getLua();
        // sol::object result = lua.script("return " + luaScript, sol::script_pass_on_error);
        sol::object result = lua.safe_script("return {" + luaScript + "}", sol::script_pass_on_error).get<sol::object>();     

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
                // Only unload if the asset reports itself as loaded
                if (it->second->isLoaded()) {
                    it->second->onUnload();
                }
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


    std::vector<std::string> Factory::getDisplayObjectNames() const
    {
        std::vector<std::string> names;
        for (const auto& pair : displayObjects_) 
        {
            names.push_back(pair.first);
        }
        return names;
    }

    AssetHandle Factory::findAssetByFilename(const std::string& filename, const std::string& typeName) const
    {
        // Search assetObjects_ for an object whose getFilename() matches the provided filename.
        for (const auto& pair : assetObjects_) {
            const std::string& name = pair.first;
            const auto& assetPtr = pair.second;
            if (!assetPtr) continue;
            try {
                std::string fn = assetPtr->getFilename();
                if (fn == filename) {
                    if (typeName.empty() || assetPtr->getType() == typeName) {
                        AssetHandle out;
                        out.name_ = name;
                        out.type_ = assetPtr->getType();
                        out.filename_ = fn;
                        return out;
                    }
                }
            } catch(...) {
                // ignore errors from getFilename/getType
            }
        }
        return AssetHandle();
    }

    AssetHandle Factory::findSpriteSheetByParams(const std::string& filename, int spriteW, int spriteH) const
    {
        for (const auto& pair : assetObjects_) {
            const std::string& name = pair.first;
            const auto& assetPtr = pair.second;
            if (!assetPtr) continue;
            try {
                if (assetPtr->getType() == SpriteSheet::TypeName && assetPtr->getFilename() == filename) {
                    const SpriteSheet* ss = dynamic_cast<const SpriteSheet*>(assetPtr.get());
                    if (!ss) continue;
                    if (ss->getSpriteWidth() == spriteW && ss->getSpriteHeight() == spriteH) {
                        AssetHandle out;
                        out.name_ = name;
                        out.type_ = ss->getType();
                        out.filename_ = ss->getFilename();
                        return out;
                    }
                }
            } catch(...) {}
        }
        return AssetHandle();
    }

    void Factory::clear()
    {
        // Destroy assets first so dependent objects (e.g., fonts, textures)
        // release their runtime resources via onUnload()/onQuit().
        try {
            std::vector<std::string> assetNames;
            assetNames.reserve(assetObjects_.size());
            for (const auto& kv : assetObjects_) assetNames.push_back(kv.first);
            for (const auto& name : assetNames) {
                // Calls onUnload()/onQuit() and erases from registry safely
                destroyAssetObject(name);
            }
        } catch(...) {
            // swallow any exceptions during shutdown
        }

        // Destroy display objects by name so any custom teardown in the
        // object (via destructor/onQuit) runs before the registry is cleared.
        try {
            std::vector<std::string> objNames;
            objNames.reserve(displayObjects_.size());
            for (const auto& kv : displayObjects_) objNames.push_back(kv.first);
            for (const auto& name : objNames) {
                destroyDisplayObject(name);
            }
        } catch(...) {
            // swallow any exceptions during shutdown
        }

        // Clear auxiliary registries/lists
        orphanList_.clear();
        futureChildrenList_.clear();
        creators_.clear();
        assetCreators_.clear();
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
        for (const auto& kv : assetObjects_) {
            const auto& name = kv.first;
            const IAssetObject* obj = kv.second.get();
            std::cout << "  Name: " << name << ", Type: " << obj->getType()
                    << ", Filename: " << obj->getFilename();
            if (auto ss = dynamic_cast<const SpriteSheet*>(obj)) {
                std::cout << ", spriteW=" << ss->getSpriteWidth()
                        << ", spriteH=" << ss->getSpriteHeight()
                        << ", count=" << ss->getSpriteCount();
            }
            if (auto bf = dynamic_cast<const BitmapFont*>(obj)) {
                std::cout << ", glyphW=" << bf->getGlyphWidth('W')
                        << ", glyphH=" << bf->getGlyphHeight('W');
            }
            std::cout << '\n';
        }
        std::cout << "Total asset objects: " << assetObjects_.size() << "\n";
    }


    void Factory::printAssetTree() const
    {
        std::cout << "Factory Asset Dependency Tree:\n";
        // To avoid cycles, track visited asset names
        std::unordered_set<std::string> visited;

        // Helper to print a single asset and its immediate children
        auto printNode = [&](const std::string& name, int indent) {
            auto it = assetObjects_.find(name);
            if (it == assetObjects_.end()) return;
            const IAssetObject* obj = it->second.get();
            if (!obj) return;
            for (int i = 0; i < indent; ++i) std::cout << "  ";
            std::cout << "- " << name << " (type=" << obj->getType() << ", file=" << obj->getFilename() << ")\n";

            // If BitmapFont, show SpriteSheet child (use public getter getResourceHandle)
            if (obj->getType() == BitmapFont::TypeName) {
                const BitmapFont* bf = dynamic_cast<const BitmapFont*>(obj);
                if (bf) {
                    AssetHandle ss = bf->getResourceHandle();
                    if (ss.isValid()) {
                        for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
                        std::cout << "-> SpriteSheet: " << ss.getName() << " (file=" << ss.getFilename() << ")\n";
                        // follow through to texture
                        IAssetObject* ssObj = ss.get();
                        if (ssObj) {
                            const SpriteSheet* ssp = dynamic_cast<const SpriteSheet*>(ssObj);
                            if (ssp) {
                                AssetHandle t = ssp->getTextureAsset();
                                if (t.isValid()) {
                                    for (int i = 0; i < indent + 2; ++i) std::cout << "  ";
                                    std::cout << "-> Texture: " << t.getName() << " (file=" << t.getFilename() << ")\n";
                                }
                            }
                        }
                    }
                }
            }

            // If TruetypeFont, show TTFAsset child (use public getter getResourceHandle)
            if (obj->getType() == TruetypeFont::TypeName) {
                const TruetypeFont* tf = dynamic_cast<const TruetypeFont*>(obj);
                if (tf) {
                    AssetHandle ttf = tf->getResourceHandle();
                    if (ttf.isValid()) {
                        for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
                        std::cout << "-> TTFAsset: " << ttf.getName() << " (file=" << ttf.getFilename() << ")\n";
                    }
                }
            }
        };

        // Iterate over all top-level assets and print their dependencies
        for (const auto& kv : assetObjects_) {
            const std::string& name = kv.first;
            if (visited.count(name)) continue;
            printNode(name, 0);
            visited.insert(name);
        }
        std::cout << std::flush;
    }


    void Factory::printAssetTreeGrouped() const
    {
        std::cout << "Factory Asset Tree (grouped by root resources):\n";

        // Find roots: Textures and TTFAssets
        std::vector<std::string> textureRoots;
        std::vector<std::string> ttfRoots;
        for (const auto& kv : assetObjects_) {
            const std::string& name = kv.first;
            const IAssetObject* obj = kv.second.get();
            if (!obj) continue;
            if (obj->getType() == Texture::TypeName) textureRoots.push_back(name);
            if (obj->getType() == TTFAsset::TypeName) ttfRoots.push_back(name);
        }

        // Helper lambdas to find dependents
        auto findSpriteSheetsForTexture = [&](const std::string& texName) {
            std::vector<std::string> out;
            for (const auto& kv : assetObjects_) {
                const IAssetObject* obj = kv.second.get();
                if (!obj) continue;
                if (obj->getType() == SpriteSheet::TypeName) {
                    const SpriteSheet* ss = dynamic_cast<const SpriteSheet*>(obj);
                    if (ss && ss->getFilename() == texName) out.push_back(kv.first);
                }
            }
            return out;
        };

        auto findBitmapFontsForSpriteSheet = [&](const std::string& sheetName) {
            std::vector<std::string> out;
            for (const auto& kv : assetObjects_) {
                const IAssetObject* obj = kv.second.get();
                if (!obj) continue;
                if (obj->getType() == BitmapFont::TypeName) {
                    const BitmapFont* bf = dynamic_cast<const BitmapFont*>(obj);
                    if (bf) {
                        AssetHandle sh = bf->getResourceHandle();
                        if (sh.isValid() && sh.getName() == sheetName) out.push_back(kv.first);
                    }
                }
            }
            return out;
        };

        auto findTruetypesForTTF = [&](const std::string& ttfName) {
            std::vector<std::string> out;
            for (const auto& kv : assetObjects_) {
                const IAssetObject* obj = kv.second.get();
                if (!obj) continue;
                if (obj->getType() == TruetypeFont::TypeName) {
                    const TruetypeFont* tf = dynamic_cast<const TruetypeFont*>(obj);
                    if (tf) {
                        AssetHandle rh = tf->getResourceHandle();
                        if (rh.isValid() && rh.getName() == ttfName) out.push_back(kv.first);
                    }
                }
            }
            return out;
        };

        // Print textures and their dependents
        for (size_t i = 0; i < textureRoots.size(); ++i) {
            const std::string& tex = textureRoots[i];
            bool isLastTex = (i + 1 == textureRoots.size() && ttfRoots.empty());
            std::cout << (isLastTex ? "└── " : "├── ") << tex << "\n";

            auto sheets = findSpriteSheetsForTexture(tex);
            for (size_t j = 0; j < sheets.size(); ++j) {
                bool lastSheet = (j + 1 == sheets.size());
                std::cout << (isLastTex ? "    " : "│   ");
                std::cout << (lastSheet ? "└── " : "├── ") << sheets[j] << "\n";

                auto fonts = findBitmapFontsForSpriteSheet(sheets[j]);
                for (size_t k = 0; k < fonts.size(); ++k) {
                    bool lastFont = (k + 1 == fonts.size());
                    std::cout << (isLastTex ? "    " : "│   ");
                    std::cout << (lastSheet ? "    " : "│   ");
                    std::cout << (lastFont ? "└── " : "├── ") << fonts[k] << "\n";
                }
            }
        }

        // Print TTF roots and their dependents
        for (size_t i = 0; i < ttfRoots.size(); ++i) {
            const std::string& ttf = ttfRoots[i];
            bool isLast = (i + 1 == ttfRoots.size());
            // Determine connector depending on whether textures were printed
            bool texturesPrinted = !textureRoots.empty();
            if (texturesPrinted) std::cout << "├── "; else std::cout << (isLast ? "└── " : "├── ");
            std::cout << ttf << "\n";

            auto tts = findTruetypesForTTF(ttf);
            for (size_t j = 0; j < tts.size(); ++j) {
                bool last = (j + 1 == tts.size());
                std::cout << (texturesPrinted ? "│   " : "    ");
                std::cout << (last ? "└── " : "├── ") << tts[j] << "\n";
            }
        }

        std::cout << std::flush;
    }

    

    // ----- Orphan and Future Child Management -----

    // Detach all orphans in the orphan list from their parents.
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


    // -----------------------------------------------------------------------------
    // Factory::onUnitTest()
    // -----------------------------------------------------------------------------
    // Purpose:
    //   Provides a minimal onUnitTest() implementation to maintain the IDataObject
    //   interface and ensure Factory participates correctly in the unit test
    //   dispatch chain.
    //
    // Notes:
    //   - This was reintroduced after a cleanup pass removed it accidentally.
    //   - Currently, no specific Factory subsystem tests are required.
    //   - The structure is left in place as a scaffold for future expansion.
    //
    // Usage:
    //   The Factory may later register subsystem-level tests here, such as
    //   object creation performance, memory tracking, or resource pool integrity.
    // -----------------------------------------------------------------------------
    bool Factory::onUnitTest(int frame)
    {
        (void)frame; // Suppress unused parameter warning

        const std::string objName = "Factory";

        // Only register once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 Placeholder for future Factory-level tests
            // Example:
            // UnitTests& ut = UnitTests::getInstance();
            // ut.add_test(objName, "Factory Initialization", [this](std::vector<std::string>& errors)
            // {
            //     if (!isInitialized())
            //         errors.push_back("Factory failed to initialize properly.");
            //     return true;
            // });

            registered = true;
        }

        // ✅ No active tests yet; return false for consistent frame lifecycle
        return false;
    } // END: Factory::onUnitTest()



    // Calls onUnload()/onQuit() through destroyAssetObject() for all Truetype assets.
    // Avoids iterator invalidation by collecting names first and lets each
    // asset's onUnload() own its TTF_CloseFont call.
    void Factory::closeAllTruetypeAssets_()
    {
        std::vector<std::string> truetypeNames;
        truetypeNames.reserve(assetObjects_.size());
        for (const auto& kv : assetObjects_) {
            if (dynamic_cast<TTFAsset*>(kv.second.get()) != nullptr) {
                truetypeNames.push_back(kv.first);
            }
        }

        for (const auto& name : truetypeNames) {
            destroyAssetObject(name);
        }
    }


    // --- Performance Test Helpers --- //

    void Factory::start_update_timer(const std::string& objName) {
        // Start timing this object's update; avoid console I/O to reduce measurement noise
        update_start_times[objName] = std::chrono::steady_clock::now();
    }

    void Factory::stop_update_timer(const std::string& objName) {
        auto end = std::chrono::steady_clock::now();
        auto it = update_start_times.find(objName);
        if (it != update_start_times.end()) {
            auto delta = end - it->second;
            perf_map[objName].update_time_ns += delta;
            perf_map[objName].last_update_ns = delta;
            perf_map[objName].update_calls++;
            update_start_times.erase(it);
        }
    }

    void Factory::start_render_time(const std::string& objName) {
        render_start_times[objName] = std::chrono::steady_clock::now();
    }

    void Factory::stop_render_time(const std::string& objName) {
        auto end = std::chrono::steady_clock::now();
        auto it = render_start_times.find(objName);
        if (it != render_start_times.end()) {
            auto delta = end - it->second;
            perf_map[objName].render_time_ns += delta;
            perf_map[objName].last_render_ns = delta;
            perf_map[objName].render_calls++;
            render_start_times.erase(it);
        }
    }
    void Factory::report_performance_stats() const 
    {
        // Print compact, separated views sorted by total time
        std::cout << "---- Performance Statistics ----\n";
        report_update_stats(10);
        std::cout << std::endl;
        report_render_stats(10);
        std::cout << "total frame time: " << (getCore().getElapsedTime()*1'000'000.0f) << "µs" << std::endl;
    }

    void Factory::report_update_stats(std::size_t topN) const
    {
        std::vector<std::pair<std::string, const PerfStats*>> items;
        items.reserve(perf_map.size());
        for (const auto& kv : perf_map) items.emplace_back(kv.first, &kv.second);

        // Sort by last frame's update delta (descending)
        std::sort(items.begin(), items.end(), [](const auto& a, const auto& b){
            return a.second->last_update_ns > b.second->last_update_ns;
        });

        const std::size_t total = items.size();
        const std::size_t limit = (topN == 0 || topN > total) ? total : topN;

        // Determine a reasonable column width for names
        std::size_t name_w = 4; // min width to fit "Name"
        for (std::size_t i = 0; i < limit; ++i) name_w = std::max(name_w, items[i].first.size());
        name_w = std::min<std::size_t>(name_w, 48);

        std::cout << "Update Times (last frame, top " << limit << " of " << total << "):\n";
        std::cout << std::left << std::setw(static_cast<int>(name_w)) << "Name" << "  "
                  << std::right << std::setw(12) << "Update (µs)" << "\n";
        std::cout << std::string(name_w, '-') << "  " << std::string(12, '-') << "\n";
        std::cout << std::fixed << std::setprecision(3);
        for (std::size_t i = 0; i < limit; ++i)
        {
            const auto& name = items[i].first;
            const auto* stats = items[i].second;
            const double last_us = std::chrono::duration<double, std::micro>(stats->last_update_ns).count();
            std::cout << std::left << std::setw(static_cast<int>(name_w)) << name << "  "
                      << std::right << std::setw(12) << last_us << "\n";
        }
    }

    void Factory::report_render_stats(std::size_t topN) const
    {
        std::vector<std::pair<std::string, const PerfStats*>> items;
        items.reserve(perf_map.size());
        for (const auto& kv : perf_map) items.emplace_back(kv.first, &kv.second);

        // Sort by last frame's render delta (descending)
        std::sort(items.begin(), items.end(), [](const auto& a, const auto& b){
            return a.second->last_render_ns > b.second->last_render_ns;
        });

        const std::size_t total = items.size();
        const std::size_t limit = (topN == 0 || topN > total) ? total : topN;

        // Determine a reasonable column width for names
        std::size_t name_w = 4; // min width to fit "Name"
        for (std::size_t i = 0; i < limit; ++i) name_w = std::max(name_w, items[i].first.size());
        name_w = std::min<std::size_t>(name_w, 48);

        std::cout << "Render Times (last frame, top " << limit << " of " << total << "):\n";
        std::cout << std::left << std::setw(static_cast<int>(name_w)) << "Name" << "  "
                  << std::right << std::setw(12) << "Render (µs)" << "\n";
        std::cout << std::string(name_w, '-') << "  " << std::string(12, '-') << "\n";
        std::cout << std::fixed << std::setprecision(3);
        for (std::size_t i = 0; i < limit; ++i)
        {
            const auto& name = items[i].first;
            const auto* stats = items[i].second;
            const double last_us = std::chrono::duration<double, std::micro>(stats->last_render_ns).count();
            std::cout << std::left << std::setw(static_cast<int>(name_w)) << name << "  "
                      << std::right << std::setw(12) << last_us << "\n";
        }
    }

    void Factory::begin_frame_metrics()
    {
        // Reset per-object last-frame deltas so that the report reflects only
        // objects that were actually updated/rendered during this frame.
        for (auto& kv : perf_map)
        {
            kv.second.last_update_ns = std::chrono::nanoseconds{0};
            kv.second.last_render_ns = std::chrono::nanoseconds{0};
        }
    }

    // Reset all performance metrics; used to clear warm-up frames before measuring
    void Factory::reset_performance_stats()
    {
        perf_map.clear();
        update_start_times.clear();
        render_start_times.clear();
    }

    // Return last deltas (microseconds) by object pointer or name
    float Factory::getLastUpdateDelta(const IDisplayObject* obj) const
    {
        if (!obj) return 0.0f;
        return getLastUpdateDelta(obj->getName());
    }

    float Factory::getLastRenderDelta(const IDisplayObject* obj) const
    {
        if (!obj) return 0.0f;
        return getLastRenderDelta(obj->getName());
    }

    float Factory::getLastUpdateDelta(const std::string& obj_name) const
    {
        auto it = perf_map.find(obj_name);
        if (it == perf_map.end()) return 0.0f;
        return std::chrono::duration<float, std::micro>(it->second.last_update_ns).count();
    }

    float Factory::getLastRenderDelta(const std::string& obj_name) const
    {
        auto it = perf_map.find(obj_name);
        if (it == perf_map.end()) return 0.0f;
        return std::chrono::duration<float, std::micro>(it->second.last_render_ns).count();
    }


} // namespace SDOM
