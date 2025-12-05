// SDOM_Factory.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_CAPI_BindGenerator.hpp>
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
#include <SDOM/SDOM_GeometryAPI.hpp>
#include <SDOM/SDOM_VariantAPI.hpp>
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
#include <cstdlib>
#include <SDL3/SDL.h>

namespace SDOM
{

    Factory::Factory() : initialized_(false)
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

        const bool headlessBindingGen = (std::getenv("SDOM_GENERATE_BINDINGS") != nullptr);
                                    // // --- Lua UserType Registration --- //
                                    // Core& core = getCore();

                                    // // Register the DisplayHandle early so any returned handles to Lua
                                    // // have a valid usertype/metatable for indexing minimal helpers.
                                    // try {
                                    //     DisplayHandle prototypeHandle; // Default DisplayHandle for registration
                                    //     prototypeHandle.registerLuaBindings("DisplayHandle", core.getLua());
                                    // } catch(...) {}

        TTFAsset::InitStruct ttf_init;
        ttf_init.name = "internal_ttf_asset";     // internal registry key

        // Register the Event System
        try {
            // Register IDataObject bindings for Event into the Factory's registry
            Event evt; // temporary to call registerBindings
            evt.registerBindings("Event", data_registry_);
        } catch(...) {
            ERROR("Factory::onInit: failed to register Event bindings");
        }        

        // Surface SDOM::Variant metadata/functions to the central registry so
        // the C API generator can emit SDOM_CAPI_Variant.{h,cpp}.
        VariantAPI::registerTypes(data_registry_);

        // Make AnchorPoint/Bounds metadata + helpers available to the binding generator.
        GeometryAPI::registerTypes(data_registry_);

        // Ensure EventType has a TypeInfo descriptor even though it does not expose
        // callable exports yet. The manifest consumers (C API generator + unit tests)
        // rely on this descriptor to confirm dispatch-family metadata for the
        // event_router subject.
        if (!data_registry_.lookupType("EventType")) {
            SDOM::TypeInfo ti;
            ti.name        = "EventType";
            ti.kind        = SDOM::EntryKind::Object;
            ti.cpp_type_id = "SDOM::EventType";
            ti.file_stem   = "EventType";
            ti.export_name = "SDOM_EventType";
            ti.subject_kind = "EventType";
            ti.subject_uses_handle = false;
            ti.has_handle_override = true;
            ti.dispatch_family_override = "event_router";
            ti.doc = "Logical subject descriptor for SDOM::EventType dispatch metadata.";
            data_registry_.registerType(ti);
        }

        // register the Stage
        registerDisplayObjectType("Stage", TypeCreators{
            Stage::CreateFromInitStruct, 
            Stage::CreateFromJson
        });

        // register the Texture asset
        registerAssetObjectType("Texture", AssetTypeCreators{
            Texture::CreateFromInitStruct,
            Texture::CreateFromJson
        });

        // Note: Do NOT pre-register a Texture under the name 'default_bmp_8x8'.
        // The BitmapFont asset will create (or reuse) the SpriteSheet/Texture for
        // that filename when initialized. Pre-creating a Texture with the same
        // registry name conflicts with BitmapFont's intended resource name.

        // --- Internal Icon Texture Registration (Lua-Free) ---
        {
            auto ensureTexture = [&](const std::string& name) {
                // Defer creation until renderer exists
                if (!getCore().getRenderer()) {
                    INFO("Factory::onInit: Renderer not available; deferring internal texture '" << name << "'");
                    return;
                }

                // Create only if not already registered
                if (!getAssetObjectPtr(name)) {
                    Texture::InitStruct init;
                    init.name     = name;
                    init.type     = Texture::TypeName;
                    init.filename = name;        // internal embedded image name
                    init.is_internal = true;

                    // Create the asset
                    AssetHandle h = createAssetObject("Texture", init);
                    if (auto* tex = h.as<Texture>()) {
                        tex->registerBindings("Texture", data_registry_);   // DataRegistry only
                    }
                } else {
                    // Already exists — just ensure C++ DataRegistry bindings exist
                    if (auto* existing = dynamic_cast<Texture*>(getAssetObjectPtr(name))) {
                        existing->registerBindings("Texture", data_registry_);
                    }
                }
            };

            ensureTexture("internal_icon_8x8");
            ensureTexture("internal_icon_12x12");
            ensureTexture("internal_icon_16x16");
        }

        // register a TTFAsset
        registerAssetObjectType("TTFAsset", AssetTypeCreators{
            TTFAsset::CreateFromInitStruct,
            TTFAsset::CreateFromJson,
        });    
        
        // register the TruetypeFont resource type so factory can create truetype font assets
        {
            registerAssetObjectType("TruetypeFont", AssetTypeCreators{
                TruetypeFont::CreateFromInitStruct,
                TruetypeFont::CreateFromJson
            });

            if (!headlessBindingGen) {
                // Create an internal TTFAsset for the default TrueType font (do not register under the same public name)
                if (!getAssetObjectPtr("internal_ttf_asset")) {
                    TTFAsset::InitStruct ttf_init;
                    ttf_init.name = "internal_ttf_asset";     // internal registry key
                    ttf_init.type = TTFAsset::TypeName;     // concrete type name
                    ttf_init.filename = "internal_ttf";      // internal ttf filename
                    ttf_init.is_internal = true;             // is internal
                    ttf_init.internal_font_size = 10;         // member name in InitStruct
                    AssetHandle ttfFontAsset = createAssetObject("TTFAsset", ttf_init);
                    (void)ttfFontAsset;
                }

                // Create a public truetype font asset named "default_ttf" which references the internal TTFAsset
                if (!getAssetObjectPtr("internal_ttf")) {
                    TruetypeFont::InitStruct ttInit;
                    ttInit.name = "internal_ttf";           // public registry key expected by Label
                    ttInit.type = TruetypeFont::TypeName;
                    ttInit.filename = "internal_ttf_asset";      // filename used by TruetypeFont to find the TTFAsset

                    AssetHandle defaultTTFont = createAssetObject(TruetypeFont::TypeName, ttInit);
                    (void)defaultTTFont;
                }
            }
        }

        // register the SpriteSheet asset
        registerAssetObjectType("SpriteSheet", AssetTypeCreators{
            SpriteSheet::CreateFromInitStruct,
            SpriteSheet::CreateFromJson
        });
        // Also ensure there is a SpriteSheet wrapper for the internal icon texture
        if (!headlessBindingGen) {
            auto ensureSpriteSheet = [&](const std::string& spriteSheetName, const std::string& textureFilename, int spriteW, int spriteH) {
                if (!getAssetObjectPtr(spriteSheetName)) {
                    SpriteSheet::InitStruct ssInit;
                    ssInit.name = spriteSheetName;
                    ssInit.type = SpriteSheet::TypeName;
                    ssInit.filename = textureFilename; // underlying texture filename
                    ssInit.spriteWidth = spriteW;
                    ssInit.spriteHeight = spriteH;
                    ssInit.is_internal = true;
                    AssetHandle ss = createAssetObject("SpriteSheet", ssInit);
                    (void)ss;
                }
            };
            ensureSpriteSheet("internal_icon_8x8_SpriteSheet",  "internal_icon_8x8",  8,  8);
            ensureSpriteSheet("internal_icon_12x12_SpriteSheet", "internal_icon_12x12", 12, 12);
            ensureSpriteSheet("internal_icon_16x16_SpriteSheet", "internal_icon_16x16", 16, 16);
        }

        // register the BitmapFont asset
        {
            registerAssetObjectType("BitmapFont", AssetTypeCreators{
                BitmapFont::CreateFromInitStruct,
                BitmapFont::CreateFromJson
            });

            if (!headlessBindingGen) {
                // Create the default 8x8 bitmap font asset if it doesn't exist.
                if (!getAssetObjectPtr("internal_font_8x8")) {
                    BitmapFont::InitStruct init;
                    init.name = "internal_font_8x8";      // registry key == filename
                    init.type = BitmapFont::TypeName;   // concrete type name
                    init.filename = "internal_font_8x8";  // underlying texture filename
                    init.is_internal = true;             // is an internal resource
                    init.fontSize = 8;                  // member name in InitStruct
                    init.font_width = 8;                 // font_width for this resource
                    init.font_height = 8;                // font_height for this resource            
                    AssetHandle bmpFont = createAssetObject("BitmapFont", init);
                    (void)bmpFont;
                }
                if (!getAssetObjectPtr("internal_font_8x12")) {
                    BitmapFont::InitStruct init;
                    init.name = "internal_font_8x12";     // registry key == filename
                    init.type = BitmapFont::TypeName;   // concrete type name
                    init.filename = "internal_font_8x12"; // underlying texture filename
                    init.is_internal = true;             // is an internal resource
                    // Set both BitmapFont-specific fontSize and the base IFontObject's
                    // fontSize_ so the inherited constructor sees the correct value.
                    init.fontSize = 12;                 // BitmapFont::InitStruct field
                    init.font_size = 12;                // IFontObject::InitStruct field used by base ctor
                    init.font_width = 8;                 // font_width for this resource
                    init.font_height = 12;               // font_height for this resource
                    AssetHandle bmpFont = createAssetObject("BitmapFont", init);
                    (void)bmpFont;
                }
            }
        }

        // register the Label display object
        registerDisplayObjectType("Label", TypeCreators{
            Label::CreateFromInitStruct_Base,   // InitStruct path
            Label::CreateFromJson               // JSON path
        });

        // --- Register the IPanelObject Decendants --- //

        // register Frame
        registerDisplayObjectType("Frame", TypeCreators{
            Frame::CreateFromInitStruct,
            Frame::CreateFromJson 
        });

        // register Button
        registerDisplayObjectType("Button", TypeCreators{
            Button::CreateFromInitStruct,   // C++ / InitStruct path
            Button::CreateFromJson          // JSON loader path
        });

        // register Group
        registerDisplayObjectType("Group", TypeCreators{
            Group::CreateFromInitStruct,   // JSON → InitStruct → Group
            Group::CreateFromJson          // InitStruct → Group
        });

        // register the IconButton
        registerDisplayObjectType("IconButton", TypeCreators{
            IconButton::CreateFromInitStruct,
            IconButton::CreateFromJson
        });

        // register the ArrowButton
        registerDisplayObjectType("ArrowButton", TypeCreators{
            ArrowButton::CreateFromInitStruct,
            ArrowButton::CreateFromJson
        });

        // register the TristateButton
        registerDisplayObjectType("TristateButton", TypeCreators{
            TristateButton::CreateFromInitStruct,
            TristateButton::CreateFromJson
        });

        // register CheckButton
        registerDisplayObjectType("CheckButton", TypeCreators{
            CheckButton::CreateFromInitStruct,
            CheckButton::CreateFromJson
        });

        // register RadioButton
        registerDisplayObjectType("RadioButton", TypeCreators{
            RadioButton::CreateFromInitStruct,
            RadioButton::CreateFromJson
        });


        // register the Slider
        registerDisplayObjectType("Slider", TypeCreators{
            Slider::CreateFromInitStruct,
            Slider::CreateFromJson
        });

        // register the ProgressBar
        registerDisplayObjectType("ProgressBar", TypeCreators{
            ProgressBar::CreateFromInitStruct,
            ProgressBar::CreateFromJson
        });

        // register the ScrollBar
        registerDisplayObjectType("ScrollBar", TypeCreators{
            ScrollBar::CreateFromInitStruct,
            ScrollBar::CreateFromJson
        });

#if defined(SDOM_ENABLE_RUNTIME_BINDING_EXPORT)
        // Add the CAPI_BindGenerator to the DataRegistry for runtime regeneration
        data_registry_.addGenerator(
            std::make_unique<CAPI_BindGenerator>(),
            "../../include/SDOM/CAPI",
            "../../src/CAPI",
            true,
            true
        );
        data_registry_.generateBindings();
#endif

        // return initialized state
        initialized_ = true;

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


    
    void Factory::registerDisplayObjectType(const std::string& typeName, const TypeCreators& creators)
    {
        creators_[typeName] = creators;

        try
        {
            nlohmann::json empty_json = nlohmann::json::object();

            std::unique_ptr<IDisplayObject> prototype;

            // --- Prefer JSON constructor if available ---
            if (creators.fromJson)
            {
                try {
                    prototype = creators.fromJson(empty_json);
                }
                catch (...) {
                    ERROR("Prototype JSON constructor failed for type '" + typeName + "'");
                    return;
                }
            }
            // --- Fallback: InitStruct path ---
            else if (creators.fromInitStruct)
            {
                try {
                    IDisplayObject::InitStruct baseInit;
                    baseInit.name = "__proto:" + typeName;
                    baseInit.type = typeName;
                    prototype = creators.fromInitStruct(baseInit);
                }
                catch (...) {
                    ERROR("Prototype InitStruct constructor failed for type '" << typeName << "'");
                    return;
                }
            }
            if (!prototype)
            {
                ERROR("Prototype creation returned null for type '" << typeName << "'");
                return;
            }
            // --- Register DataRegistry bindings ---
            try {
                prototype->IDataObject::registerBindings(typeName, data_registry_);
            }
            catch (...) {
                ERROR("DataRegistry binding failure for type '" << typeName << "'");
            }
        }
        catch (...)
        {
            ERROR("Unknown exception during registerDisplayObjectType('" << typeName << "')");
        }
    }


    IDisplayObject* Factory::getDisplayObjectPtr(const std::string& name)
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) 
        {
            return dynamic_cast<IDisplayObject*>(it->second->obj.get());
        }
        return nullptr;
    }

    void Factory::registerAssetObjectType(const std::string& typeName, const AssetTypeCreators& creators)
    {
        // Register resource type with creators
        assetCreators_[typeName] = creators;
    }

    IAssetObject* Factory::getAssetObjectPtr(const std::string& name)
    {
        auto it = assetObjects_.find(name);
        if (it != assetObjects_.end()) 
        {
            return dynamic_cast<IAssetObject*>(it->second->obj.get());
        }
        return nullptr;
    }
    
    DisplayHandle Factory::getDisplayObject(const std::string& name) 
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) 
        {
            // Use the resource's type for the DisplayHandle
            return DisplayHandle(name, it->second->type);
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
            out.type_ = it->second->type;
            return out;
        }
        // Return an empty AssetHandle if not found
        return AssetHandle();
    }

    // ID registry helpers
    uint64_t Factory::registerDisplayObject(const std::string& name, const DisplayHandle& handle)
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) {
            uint64_t existing = it->second->id;
            if (existing != 0) {
                // Already has an id; ensure mapping exists
                std::shared_lock<std::shared_mutex> rlock(id_map_mutex_);
                if (display_id_map_.find(existing) != display_id_map_.end()) return existing;
                // fall through to register new mapping
            }
        }
        uint64_t id = next_object_id_.fetch_add(1);
        {
            std::unique_lock<std::shared_mutex> lock(id_map_mutex_);
            display_id_map_[id] = handle;
        }
        if (it != displayObjects_.end()) it->second->id = id;
        return id;
    }

    DisplayHandle Factory::resolveDisplayObject(uint64_t id) const
    {
        std::shared_lock<std::shared_mutex> lock(id_map_mutex_);
        auto it = display_id_map_.find(id);
        if (it != display_id_map_.end()) return it->second;
        return DisplayHandle();
    }

    void Factory::unregisterDisplayObject(uint64_t id)
    {
        std::unique_lock<std::shared_mutex> lock(id_map_mutex_);
        display_id_map_.erase(id);
    }

    uint64_t Factory::registerAssetObject(const std::string& name, const AssetHandle& handle)
    {
        auto it = assetObjects_.find(name);
        if (it != assetObjects_.end()) {
            uint64_t existing = it->second->id;
            if (existing != 0) {
                std::shared_lock<std::shared_mutex> rlock(id_map_mutex_);
                if (asset_id_map_.find(existing) != asset_id_map_.end()) return existing;
            }
        }
        uint64_t id = next_object_id_.fetch_add(1);
        {
            std::unique_lock<std::shared_mutex> lock(id_map_mutex_);
            asset_id_map_[id] = handle;
        }
        if (it != assetObjects_.end()) it->second->id = id;
        return id;
    }

    AssetHandle Factory::resolveAssetObject(uint64_t id) const
    {
        std::shared_lock<std::shared_mutex> lock(id_map_mutex_);
        auto it = asset_id_map_.find(id);
        if (it != asset_id_map_.end()) return it->second;
        return AssetHandle();
    }

    void Factory::unregisterAssetObject(uint64_t id)
    {
        std::unique_lock<std::shared_mutex> lock(id_map_mutex_);
        asset_id_map_.erase(id);
    }

    SDL_Texture* Factory::retainTexture(const std::string& key,
                                        const std::function<SDL_Texture*()>& loader)
    {
        auto& rec = textureCache_[key];
        if (rec.texture) {
            rec.refcount++;
            return rec.texture;
        }
        SDL_Texture* tex = loader ? loader() : nullptr;
        rec.texture = tex;
        rec.refcount = tex ? 1 : 0;
        return tex;
    }

    void Factory::releaseTexture(const std::string& key, SDL_Texture* ptr)
    {
        auto it = textureCache_.find(key);
        if (it == textureCache_.end()) {
            return;
        }
        auto& rec = it->second;
        if (rec.texture != ptr) {
            return;
        }
        if (rec.refcount > 0) {
            rec.refcount--;
        }
        if (rec.refcount == 0 && rec.texture) {
            SDL_DestroyTexture(rec.texture);
            rec.texture = nullptr;
        }
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


    DisplayHandle Factory::createDisplayObject(const std::string& typeName,
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
                // Wrap and insert
                displayObjects_[name] = std::make_unique<DisplayRecord>(std::move(displayObject), typeName, 0);
                auto& entry = displayObjects_[name];
                // Run initialization callback now that registry entry exists
                if (entry->obj) entry->obj->startup();

                // Dispatch OnInit event
                auto& eventManager = getCore().getEventManager();
                std::unique_ptr<Event> initEvent =
                    std::make_unique<Event>(EventType::OnInit, DisplayHandle{name, typeName});

                DisplayHandle stageHandle = getCore().getRootNode();
                if (stageHandle)
                    initEvent->setRelatedTarget(stageHandle);

                eventManager.dispatchEvent(std::move(initEvent), DisplayHandle(name, typeName));

                try {
                    registerDisplayObject(name, DisplayHandle(name, typeName));
                } catch(...) {}

                return DisplayHandle(name, typeName);
            }
        }

        return DisplayHandle(); // invalid
    }

    DisplayHandle Factory::createDisplayObjectFromJson(
        const std::string& typeName,
        const nlohmann::json& j)
    {
        auto it = creators_.find(typeName);
        if (it == creators_.end())
            return DisplayHandle{};

        auto obj = it->second.fromJson(j);
        if (!obj)
            return DisplayHandle{};

        // We want InitStruct to own the name/type parsing,
        // so we can either:
        // 1) let fromJson() construct via InitStruct and use that,
        // 2) or pull "name" from JSON here.
        // I recommend using InitStruct::from_json inside the type's CreateFromJson
        // and having the concrete type ensure `init.name` is correct.

        const std::string name = obj->getName(); // assuming IDisplayObject has getName()
        addDisplayObject(name, std::move(obj));
        return getDisplayObject(name);
    }



    AssetHandle Factory::createAssetObject(const std::string& typeName, const IAssetObject::InitStruct& init)
    {
        // If the asset name already exists, return a handle to it.
        if (auto itExisting = assetObjects_.find(init.name); itExisting != assetObjects_.end())
        {
            return AssetHandle(init.name, itExisting->second->type, itExisting->second->obj->getFilename());
        }

        // Reuse by filename for Texture / TTFAsset
        if (!init.filename.empty())
        {
            if (typeName == TTFAsset::TypeName)
            {
                try {
                    const auto& ttfInit = static_cast<const TTFAsset::InitStruct&>(init);
                    int desiredSize = ttfInit.internal_font_size;

                    for (const auto& [key, assetEntry] : assetObjects_)
                    {
                        if (!assetEntry || !assetEntry->obj) continue;
                        if (assetEntry->obj->getFilename() == init.filename && assetEntry->obj->getType() == TTFAsset::TypeName)
                        {
                            if (auto* existingTTF = dynamic_cast<TTFAsset*>(assetEntry->obj.get()))
                            {
                                if (existingTTF->getFontSize() == desiredSize)
                                    return AssetHandle(key, assetEntry->obj->getType(), assetEntry->obj->getFilename());
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

            assetObjects_[init.name] = std::make_unique<AssetRecord>(sharedAsset, typeName, 0);
            auto& entry = assetObjects_[init.name];
            if (entry && entry->obj) entry->obj->startup();

            try {
                if (!entry->obj->isLoaded()) {
                    if (!entry->obj->load()) {
                        ERROR("Factory::createAsset(init): load() failed for asset: " + init.name);
                    }
                }
            } catch(const std::exception& ex) {
                ERROR(std::string("Factory::createAsset(init): load() threw for asset: ") + init.name + " reason: " + ex.what());
            } catch(...) {
                ERROR("Factory::createAsset(init): load() threw for asset: " + init.name + " (unknown cause)");
            }

            try {
                registerAssetObject(init.name, AssetHandle(init.name, typeName, init.filename));
            } catch(...) {}
            return AssetHandle(init.name, typeName, init.filename);
        }

        return AssetHandle(); // invalid
    }

    AssetHandle Factory::createAssetObjectFromJson(const std::string& typeName, const nlohmann::json& j)
    {
        auto it = assetCreators_.find(typeName);
        if (it == assetCreators_.end() || !it->second.fromJson)
        {
            WARNING(std::string("Factory::createAssetObjectFromJson: no JSON creator registered for type '") + typeName + "'");
            return AssetHandle();
        }

        auto asset = it->second.fromJson(j);
        if (!asset)
        {
            WARNING(std::string("Factory::createAssetObjectFromJson: JSON creator returned null for type '") + typeName + "'");
            return AssetHandle();
        }

        const std::string name = asset->getName();
        if (name.empty())
        {
            ERROR(std::string("Factory::createAssetObjectFromJson: asset type '") + typeName + "' did not supply a name");
            return AssetHandle();
        }

        if (auto existing = assetObjects_.find(name); existing != assetObjects_.end())
        {
            return AssetHandle(name, existing->second->type,
                existing->second->obj ? existing->second->obj->getFilename() : std::string{});
        }

        asset->setType(typeName);
        std::shared_ptr<IAssetObject> sharedAsset = std::move(asset);
        assetObjects_[name] = std::make_unique<AssetRecord>(sharedAsset, typeName, 0);
        auto& entry = assetObjects_[name];
        if (entry && entry->obj)
        {
            entry->obj->startup();
            if (!entry->obj->isLoaded())
            {
                try
                {
                    if (!entry->obj->load())
                    {
                        ERROR(std::string("Factory::createAssetObjectFromJson: load() failed for asset '") + name + "'");
                    }
                }
                catch (const std::exception& ex)
                {
                    ERROR(std::string("Factory::createAssetObjectFromJson: load() threw for asset '") + name + "': " + ex.what());
                }
                catch (...)
                {
                    ERROR(std::string("Factory::createAssetObjectFromJson: load() threw for asset '") + name + "' (unknown cause)");
                }
            }
        }

        try
        {
            registerAssetObject(name, AssetHandle(name, typeName,
                entry && entry->obj ? entry->obj->getFilename() : std::string{}));
        }
        catch (...)
        {
        }

        const std::string filename = (entry && entry->obj) ? entry->obj->getFilename() : std::string{};
        return AssetHandle(name, typeName, filename);
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
        std::string type = "";
        if (displayObject) type = displayObject->getType();
        displayObjects_[name] = std::make_unique<DisplayRecord>(std::move(displayObject), type, 0);

        auto& entry = displayObjects_[name];
        if (entry && entry->obj)
        {
            if (entry->type.empty())
            {
                entry->type = entry->obj->getType();
                type = entry->type;
            }

            entry->obj->startup();

            auto& eventManager = getCore().getEventManager();
            std::unique_ptr<Event> initEvent =
                std::make_unique<Event>(EventType::OnInit, DisplayHandle{name, type});

            DisplayHandle stageHandle = getCore().getRootNode();
            if (stageHandle)
                initEvent->setRelatedTarget(stageHandle);

            eventManager.dispatchEvent(std::move(initEvent), DisplayHandle(name, type));
        }

        try {
            registerDisplayObject(name, DisplayHandle(name, type));
        } catch(...) {}
    }
    
    void Factory::destroyDisplayObject(const std::string& name) 
    {
        auto it = displayObjects_.find(name);
        if (it != displayObjects_.end()) {
            uint64_t id = it->second ? it->second->id : 0;
            if (id != 0) {
                try { unregisterDisplayObject(id); } catch(...) {}
            }
        }
        displayObjects_.erase(name);
    }

    void Factory::destroyAssetObject(const std::string& name) 
    {
        auto it = assetObjects_.find(name);
        if (it == assetObjects_.end()) return;

        // Ask the object to release runtime resources first
            try {
            if (it->second && it->second->obj) {
                // Only unload if the asset reports itself as loaded
                if (it->second->obj->isLoaded()) {
                    it->second->obj->unload();
                }
                // Use owner-controlled shutdown so virtual cleanup runs while
                // object is still fully-formed.
                it->second->obj->shutdown();
            }
        } catch(...) {
            ERROR("Factory::destroyAssetObject: exception while shutting down asset: " + name);
        }

        // Finally remove from registry (unique_ptr destructor will run)
        uint64_t id = it->second ? it->second->id : 0;
        assetObjects_.erase(it);
        if (id != 0) {
            try { unregisterAssetObject(id); } catch(...) {}
        }
    }
 

    int Factory::countOrphanedDisplayObjects() const {
        int count = 0;
        for (const auto& [name, objPtr] : displayObjects_) {
            auto obj = dynamic_cast<IDisplayObject*>(objPtr->obj.get());
            if (obj && !obj->getParent() && obj->getType() != "Stage") {
                ++count;
            }
        }
        return count;
    }

    std::vector<DisplayHandle> Factory::getOrphanedDisplayObjects() {
        std::vector<DisplayHandle> orphans;
        for (const auto& [name, objPtr] : displayObjects_) {
            auto obj = dynamic_cast<IDisplayObject*>(objPtr->obj.get());
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
            const auto& entry = pair.second;
            if (!entry || !entry->obj) continue;
            try {
                std::string fn = entry->obj->getFilename();
                if (fn == filename) {
                    if (typeName.empty() || entry->obj->getType() == typeName) {
                        AssetHandle out;
                        out.name_ = name;
                        out.type_ = entry->obj->getType();
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
            const auto& entry = pair.second;
            if (!entry || !entry->obj) continue;
            try {
                if (entry->obj->getType() == SpriteSheet::TypeName && entry->obj->getFilename() == filename) {
                    const SpriteSheet* ss = dynamic_cast<const SpriteSheet*>(entry->obj.get());
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

        // Destroy cached textures owned by the factory
        for (auto& kv : textureCache_) {
            auto& rec = kv.second;
            if (rec.texture) {
                SDL_DestroyTexture(rec.texture);
                rec.texture = nullptr;
            }
            rec.refcount = 0;
        }
        textureCache_.clear();
    }

    void Factory::printObjectRegistry() const
    {
        std::cout << "Factory Display Object Registry:\n";
    for (const auto& pair : displayObjects_) 
    {
        const auto& name = pair.first;
        const auto& displayObject = pair.second;
        std::cout << "  Name: " << name
            << ", Type: " << (displayObject && displayObject->obj ? displayObject->obj->getType() : "Unknown")
            << "\n";
    }
        std::cout << "Total display objects: " << displayObjects_.size() << std::endl;
    }

    void Factory::printAssetRegistry() const
    {
        std::cout << "Factory Asset Object Registry:\n";
    for (const auto& kv : assetObjects_) {
        const auto& name = kv.first;
        const IAssetObject* obj = kv.second->obj.get();
        if (!obj) continue;
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
            const IAssetObject* obj = it->second->obj.get();
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
            const IAssetObject* obj = kv.second->obj.get();
            if (!obj) continue;
            if (obj->getType() == Texture::TypeName) textureRoots.push_back(name);
            if (obj->getType() == TTFAsset::TypeName) ttfRoots.push_back(name);
        }

        // Helper lambdas to find dependents
        auto findSpriteSheetsForTexture = [&](const std::string& texName) {
            std::vector<std::string> out;
            for (const auto& kv : assetObjects_) {
                const IAssetObject* obj = kv.second->obj.get();
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
                const IAssetObject* obj = kv.second->obj.get();
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
                const IAssetObject* obj = kv.second->obj.get();
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
            if (kv.second && kv.second->obj && kv.second->obj->getType() == TTFAsset::TypeName) {
                truetypeNames.push_back(kv.first);
            }
        }

        for (const auto& name : truetypeNames) {
            destroyAssetObject(name);
        }
    }


    // --- Performance Test Helpers --- //

    void Factory::start_update_timer(const IDisplayObject* obj) {
        if (!obj) return;
        update_start_times[obj] = std::chrono::steady_clock::now();
    }

    void Factory::stop_update_timer(const IDisplayObject* obj, const std::string* name_snapshot) {
        if (!obj) return;
        auto end = std::chrono::steady_clock::now();
        auto it = update_start_times.find(obj);
        if (it != update_start_times.end()) {
            auto delta = end - it->second;
            auto &stats = perf_map[obj];
            stats.update_time_ns += delta;
            stats.last_update_ns = delta;
            stats.update_calls++;
            if (name_snapshot) {
                // store a copy of the last known name for reporting
                // (keeps reporting decoupled from object lifetime)
                // Assuming PerfStats has no 'name' field; we won't output names from here
                (void)name_snapshot; // reserved for future
            }
            update_start_times.erase(it);
        }
    }

    void Factory::abandon_update_timer(const IDisplayObject* obj) {
        if (!obj) return;
        auto it = update_start_times.find(obj);
        if (it != update_start_times.end()) update_start_times.erase(it);
    }

    void Factory::start_render_time(const IDisplayObject* obj) {
        if (!obj) return;
        render_start_times[obj] = std::chrono::steady_clock::now();
    }

    void Factory::stop_render_time(const IDisplayObject* obj, const std::string* name_snapshot) {
        if (!obj) return;
        auto end = std::chrono::steady_clock::now();
        auto it = render_start_times.find(obj);
        if (it != render_start_times.end()) {
            auto delta = end - it->second;
            auto &stats = perf_map[obj];
            stats.render_time_ns += delta;
            stats.last_render_ns = delta;
            stats.render_calls++;
            (void)name_snapshot; // reserved for future use
            render_start_times.erase(it);
        }
    }

    void Factory::abandon_render_time(const IDisplayObject* obj) {
        if (!obj) return;
        auto it = render_start_times.find(obj);
        if (it != render_start_times.end()) render_start_times.erase(it);
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
        std::vector<std::pair<const IDisplayObject*, const PerfStats*>> items;
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
        for (std::size_t i = 0; i < limit; ++i) {
            const IDisplayObject* obj = items[i].first;
            std::string nm = obj ? obj->getName() : std::string("<dead>");
            name_w = std::max(name_w, nm.size());
        }
        name_w = std::min<std::size_t>(name_w, 48);

        std::cout << "Update Times (last frame, top " << limit << " of " << total << "):\n";
        std::cout << std::left << std::setw(static_cast<int>(name_w)) << "Name" << "  "
                  << std::right << std::setw(12) << "Update (µs)" << "\n";
        std::cout << std::string(name_w, '-') << "  " << std::string(12, '-') << "\n";
        std::cout << std::fixed << std::setprecision(3);
        for (std::size_t i = 0; i < limit; ++i)
        {
            const IDisplayObject* obj = items[i].first;
            const auto* stats = items[i].second;
            std::string name = obj ? obj->getName() : std::string("<dead>");
            const double last_us = std::chrono::duration<double, std::micro>(stats->last_update_ns).count();
            std::cout << std::left << std::setw(static_cast<int>(name_w)) << name << "  "
                      << std::right << std::setw(12) << last_us << "\n";
        }
    }

    void Factory::report_render_stats(std::size_t topN) const
    {
        std::vector<std::pair<const IDisplayObject*, const PerfStats*>> items;
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
        for (std::size_t i = 0; i < limit; ++i) {
            const IDisplayObject* obj = items[i].first;
            std::string nm = obj ? obj->getName() : std::string("<dead>");
            name_w = std::max(name_w, nm.size());
        }
        name_w = std::min<std::size_t>(name_w, 48);

        std::cout << "Render Times (last frame, top " << limit << " of " << total << "):\n";
        std::cout << std::left << std::setw(static_cast<int>(name_w)) << "Name" << "  "
                  << std::right << std::setw(12) << "Render (µs)" << "\n";
        std::cout << std::string(name_w, '-') << "  " << std::string(12, '-') << "\n";
        std::cout << std::fixed << std::setprecision(3);
        for (std::size_t i = 0; i < limit; ++i)
        {
            const IDisplayObject* obj = items[i].first;
            const auto* stats = items[i].second;
            std::string name = obj ? obj->getName() : std::string("<dead>");
            const double last_us = std::chrono::duration<double, std::micro>(stats->last_render_ns).count();
            std::cout << std::left << std::setw(static_cast<int>(name_w)) << name << "  "
                      << std::right << std::setw(12) << last_us << "\n";
        }
    }

    void Factory::begin_frame_metrics()
    {
        // Reset per-object last-frame deltas so that the report reflects only
        // objects that were actually updated/rendered during this frame.
        for (auto& kv : perf_map) {
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
        // Name-based query deprecated: scan for first object with matching name
        for (const auto& kv : perf_map) {
            const IDisplayObject* obj = kv.first;
            if (obj && obj->getName() == obj_name) {
                return std::chrono::duration<float, std::micro>(kv.second.last_update_ns).count();
            }
        }
        return 0.0f;
    }

    float Factory::getLastRenderDelta(const std::string& obj_name) const
    {
        for (const auto& kv : perf_map) {
            const IDisplayObject* obj = kv.first;
            if (obj && obj->getName() == obj_name) {
                return std::chrono::duration<float, std::micro>(kv.second.last_render_ns).count();
            }
        }
        return 0.0f;
    }

    // Owner-controlled lifecycle helpers
    bool Factory::startup()
    {
        return onInit();
    }

    void Factory::shutdown()
    {
        if (!initialized_) return;
        try {
            clear();
        } catch(...) {}
        initialized_ = false;
    }


} // namespace SDOM
