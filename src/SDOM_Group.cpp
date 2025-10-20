// SDOM_Group.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_TruetypeFont.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_Group.hpp>
#include <SDOM/SDOM_LuaRegisterHelpers.hpp>



namespace SDOM
{

    Group::Group(const InitStruct& init) : IPanelObject(init)
    {
        // std::cout << "Box constructed with InitStruct: " << getName() 
        //           << " at address: " << this << std::endl;

        if (init.type != TypeName) {
            ERROR("Error: Group constructed with incorrect type: " + init.type);
        }
        // Group Specific
        text_ = init.text;
        base_index_ = init.base_index;
        font_size_ = init.font_size; 
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        font_resource_ = init.font_resource;
        icon_resource_ = init.icon_resource;

        setTabEnabled(false); // Groups are not tab-enabled by default
        setClickable(false); // Groups are not clickable by default

    }


    Group::Group(const sol::table& config) : IPanelObject(config, Group::InitStruct())
    {
        // std::cout << "Box constructed with Lua config: " << getName() 
        //         << " at address: " << this << std::endl;            

        std::string type = config["type"].valid() ? config["type"].get<std::string>() : "";

        // INFO("Group::Group(const sol::table& config) -- name: " << getName() 
        //         << " type: " << type << " typeName: " << TypeName << std::endl 
        // ); // END INFO()

        if (type != TypeName) {
            ERROR("Error: Group constructed with incorrect type: " + type);
        }
        
        InitStruct init;

        // Apply Lua-provided text property to the Group
        if (config["text"].valid()) {
            try {
                text_ = config["text"].get<std::string>();
            } catch (...) {
                // defensive: ignore non-string values
            }
        }       
        // Apply Lua-provided font properties to the Group (accept multiple key variants).
        // If a property is NOT present in the config, leave it as -1 to indicate
        // "unspecified" so the BitmapFont defaults helper can populate it.
        font_size_ = config["font_size"].valid() ? config["font_size"].get<int>() : -1;
        font_width_ = config["font_width"].valid() ? config["font_width"].get<int>() : -1;
        font_height_ = config["font_height"].valid() ? config["font_height"].get<int>() : -1;

        // font resource name - accept either 'font_resource' or 'font_resource_name'
        if (config["font_resource"].valid()) {
            try { font_resource_ = config["font_resource"].get<std::string>(); }
            catch (...) { /* ignore */ }
        } else if (config["font_resource_name"].valid()) {
            try { font_resource_ = config["font_resource_name"].get<std::string>(); }
            catch (...) { /* ignore */ }
        }     
        // icon resource name - accept either 'icon_resource' or 'icon_resource_name'
        if (config["icon_resource"].valid()) {
            try { icon_resource_ = config["icon_resource"].get<std::string>(); }
            catch (...) { /* ignore */ }
        } else if (config["icon_resource_name"].valid()) {
            try { icon_resource_ = config["icon_resource_name"].get<std::string>(); }
            catch (...) { /* ignore */ }
        }     

        // label color - accept snake_case/table { r=.., g=.., b=.., a=.. } or camelCase 'labelColor'
        try {
            if (config["label_color"].valid() && config["label_color"].get_type() == sol::type::table) {
                sol::table lc = config["label_color"];
                label_color_.r = lc["r"].get_or((int)label_color_.r);
                label_color_.g = lc["g"].get_or((int)label_color_.g);
                label_color_.b = lc["b"].get_or((int)label_color_.b);
                label_color_.a = lc["a"].get_or((int)label_color_.a);
            } else if (config["labelColor"].valid() && config["labelColor"].get_type() == sol::type::table) {
                sol::table lc = config["labelColor"];
                label_color_.r = lc["r"].get_or((int)label_color_.r);
                label_color_.g = lc["g"].get_or((int)label_color_.g);
                label_color_.b = lc["b"].get_or((int)label_color_.b);
                label_color_.a = lc["a"].get_or((int)label_color_.a);
            } else if (config["foreground_color"].valid() && config["foreground_color"].get_type() == sol::type::table) {
                sol::table lc = config["foreground_color"];
                label_color_.r = lc["r"].get_or((int)label_color_.r);
                label_color_.g = lc["g"].get_or((int)label_color_.g);
                label_color_.b = lc["b"].get_or((int)label_color_.b);
                label_color_.a = lc["a"].get_or((int)label_color_.a);
            } else if (config["foregroundColor"].valid() && config["foregroundColor"].get_type() == sol::type::table) {
                sol::table lc = config["foregroundColor"];
                label_color_.r = lc["r"].get_or((int)label_color_.r);
                label_color_.g = lc["g"].get_or((int)label_color_.g);
                label_color_.b = lc["b"].get_or((int)label_color_.b);
                label_color_.a = lc["a"].get_or((int)label_color_.a);
            }
        } catch (...) { /* ignore malformed color tables */ }

        // non-lua configurable essential init values
        base_index_ = init.base_index; // default to GroupUp
        
        setTabEnabled(false); // Groups are not tab-enabled by default
        setClickable(false); // Groups are not clickable by default
    }


    // --- Virtual Methods --- //
    bool Group::onInit()
    {
        // create the label object
        if (!labelObject_)
        {
            Label::InitStruct init;
            init.name = getName() + "_label";
            init.type = "Label";
            init.x = getX() + 8;
            init.y = (getY() - 2);
            init.width = getWidth() - 16;
            init.height = 12;   
            init.alignment = LabelAlign::TOP_LEFT;
            init.anchorLeft = AnchorPoint::TOP_LEFT;
            init.anchorTop = AnchorPoint::TOP_LEFT; 
            init.anchorRight = AnchorPoint::TOP_LEFT;
            init.anchorBottom = AnchorPoint::TOP_LEFT;
            init.resourceName = font_resource_;

            // init.color = label_color_;
            init.outlineColor = {0, 0, 0, 255};
            init.outlineThickness = 1;
            init.outline = true;
            init.dropshadow = true;
            init.foregroundColor = label_color_;
            init.color = label_color_;
            init.isClickable = false;
            init.tabEnabled = false;
            init.text = text_;
            init.fontSize = font_size_;
            init.fontWidth = font_width_;
            init.fontHeight = font_height_;

            // Fill missing font metrics from referenced BitmapFont asset so
            // 8x12 bitmap fonts render at native size instead of being
            // scaled down to 8x8.
            IFontObject::applyBitmapFontDefaults(getFactory(), init.resourceName,
                                                init.fontSize, init.fontWidth, init.fontHeight);

            // Debug: report what we will pass into the Label so we can trace
            // whether defaults were applied correctly.
            // INFO("Group::onInit() - Label init metrics for '" + init.name + "': resource='" + init.resourceName + "' fontSize=" + std::to_string(init.fontSize) + " fontWidth=" + std::to_string(init.fontWidth) + " fontHeight=" + std::to_string(init.fontHeight));
            labelObject_ = getFactory().create("Label", init);
            addChild(labelObject_);

            // Is this a BitmapFont or TruetypeFont? Use the label's resolved
            // font type and query the underlying font asset for an accurate
            // glyph metric. For Truetype, use the font ascent so the text
            // aligns visually with the top-line; for Bitmap, use the sprite
            // height.
            auto lbl = labelObject_.as<Label>();
            if (!lbl)
            {
                ERROR("Error: Group::onInit() - Failed to cast labelObject_ to Label* for '" + init.name + "'");
                return false;
            }
            IFontObject::FontType fontType = lbl->getFontType();

            // Center the label vertically within the upper bounds of the Group.
            // Avoid relying on the Label's glyph metrics which may not be
            // initialized yet; instead query the font asset directly and load
            // it if necessary so we can compute the correct glyph height.
            int glyphH = 0;
            if (!init.resourceName.empty())
            {
                AssetHandle fh = getFactory().getAssetObject(init.resourceName);
                if (fh.isValid())
                {
                    IFontObject* fo = fh.as<IFontObject>();
                    if (fo)
                    {
                        // Ensure font is loaded so metrics are available
                        if (!fo->isLoaded()) fo->onLoad();

                        if (fontType == IFontObject::FontType::Bitmap)
                        {
                            BitmapFont* bf = dynamic_cast<BitmapFont*>(fo);
                            if (bf) glyphH = bf->getGlyphHeight('W') / 0.75f; // use 75% of bitmap font height to approximate baseline
                        }
                        else if (fontType == IFontObject::FontType::Truetype)
                        {
                            TruetypeFont* tf = dynamic_cast<TruetypeFont*>(fo);
                            if (tf) glyphH = tf->getGlyphHeight('W') / 0.65f;  // use 65% of truetype font ascent to approximate baseline
                        }
                    }
                }
            }
            if (glyphH <= 0) glyphH = 8; // safe default

            // Determine the icon glyph height used for the panel's top row
            int iconH = getIconHeight();
            if (iconH <= 0) iconH = 8; // fallback default
            // INFO("Group::onInit() - Label glyph height for '" + init.name + "': " + std::to_string(glyphH)
            //      + " icon height: " + std::to_string(iconH)
            // );

            // center the label vertically within the top of the Group
            labelObject_->setY((getY() + (iconH / 2)) - (glyphH / 2));
            // labelObject_->setY(getY() - (glyphH/2));
        }

        return SUPER::onInit();
    } // END: bool Group::onInit()


    void Group::onQuit()
    {
        SUPER::onQuit();
    } // END: void Group::onQuit()


    void Group::onRender()
    {
        SUPER::onRender();

    } // END: void Group::onRender()


    void Group::onUpdate(float fElapsedTime)
    {
    } // END: void Group::onUpdate(float fElapsedTime)


    void Group::onEvent(const Event& event)
    {
        // only target phase
        if (event.getPhase() != Event::Phase::Target) 
        {
            return;
        }
    } // END: void Group::onEvent(const Event& event)

    bool Group::onUnitTest()
    {
        // run base checks first
        if (!SUPER::onUnitTest()) return false;

        // Basic test: ensure labelObject_ is valid and has correct text
        if (!labelObject_.isValid())
        {
            DEBUG_LOG("[UnitTest] Group '" << getName() << "' labelObject_ is not valid.");
            return false;
        }

        auto lbl = labelObject_.as<Label>();
        if (!lbl)
        {
            DEBUG_LOG("[UnitTest] Group '" << getName() << "' labelObject_ is not a Label.");
            return false;
        }

        if (lbl->getText() != text_)
        {
            DEBUG_LOG("[UnitTest] Group '" << getName() << "' label text mismatch. Expected: '"
                    << text_ << "' Got: '" << lbl->getText() << "'");
            return false;
        }

        // Additional cheap deterministic checks could be added here

        return true; // all tests passed
    } // END: bool Group::onUnitTest()



    // --- Label Helpers (C++ / LUA)--- //

    DisplayHandle Group::getLabel() const           { return labelObject_; }
    std::string Group::getLabelText() const         { auto* label = getLabelPtr(); return label ? label->getText() : ""; }
    SDL_Color Group::getLabelColor() const          { auto* label = getLabelPtr(); return label ? label->getColor() : SDL_Color{255,255,255,255}; }
    void Group::setLabelText(const std::string& t)  { if (auto* label = getLabelPtr()) label->setText(t); }
    void Group::setLabelColor(SDL_Color c)          { if (auto* label = getLabelPtr()) label->setColor(c); }
    void Group::setFontSize(int s)                  { if (auto* label = getLabelPtr()) label->setFontSize(s); }
    void Group::setFontWidth(int w)                 { if (auto* label = getLabelPtr()) label->setFontWidth(w); }
    void Group::setFontHeight(int h)                { if (auto* label = getLabelPtr()) label->setFontHeight(h); }
    int Group::getFontSize() const                  { auto* label = getLabelPtr(); return label ? label->getFontSize() : 0; }
    int Group::getFontWidth() const                 { auto* label = getLabelPtr(); return label ? label->getFontWidth() : 0; }
    int Group::getFontHeight() const                { auto* label = getLabelPtr(); return label ? label->getFontHeight() : 0; }


    // --- SpriteSheet Helpers (C++ / LUA)--- //           

    AssetHandle Group::getSpriteSheet() const       { return IPanelObject::getSpriteSheet(); }
    SDL_Color Group::getGroupColor() const          { return this->getColor(); }
    void Group::setGroupColor(const SDL_Color& c)   { this->setColor(c); }
    int Group::getSpriteWidth() const               { auto* ss = getSpriteSheetPtr(); return ss ? ss->getSpriteWidth() : 0; }
    int Group::getSpriteHeight() const              { auto* ss = getSpriteSheetPtr(); return ss ? ss->getSpriteHeight() : 0; }


    // --- Raw Pointer Accessors (for C++ only) --- //

    Label* Group::getLabelPtr() const               { return labelObject_ ? labelObject_.as<Label>() : nullptr; }
    SpriteSheet* Group::getSpriteSheetPtr() const   { auto handle = getSpriteSheet(); return handle ? handle.as<SpriteSheet>() : nullptr; }



    // --- Lua Registration --- //

    void Group::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "Group";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }
        if (DEBUG_REGISTER_LUA) {
            printf("[TRACE] Group::_registerLuaBindings called for type='%s' lua_state=%p\n", typeName.c_str(), (void*)lua.lua_state());
        }

        // Register bindings into a per-type binding table instead of the
        // global DisplayHandle table. The DisplayHandle dispatcher will
        // route lookups to this table at runtime.
        sol::table handle = DisplayHandle::ensure_type_bind_table(lua, typeName, SUPER::TypeName);

        // Diagnostic: report that Group::_registerLuaBindings ran and show
        // whether a few key bindings are already present on the per-type
        // table (raw access) so we can detect missing registrations.
        if (handle.valid() && DEBUG_REGISTER_LUA) {
            try {
                sol::table raw = DisplayHandle::ensure_type_bind_table(lua, typeName, SUPER::TypeName);
                auto has_key = [&](const char* n)->bool {
                    sol::object o = raw.raw_get_or(n, sol::lua_nil);
                    return o.valid() && o != sol::lua_nil;
                };
                std::cout << "[DEBUG] Group::_registerLuaBindings called for typeName='" << typeName << "' per-type table presence:"
                          << " getLabel=" << (has_key("getLabel")?"Y":"N")
                          << " getLabelText=" << (has_key("getLabelText")?"Y":"N")
                          << " getFontSize=" << (has_key("getFontSize")?"Y":"N")
                          << std::endl;
            } catch(...) { /* ignore */ }
        }
        // Check direct presence on the per-type table (bypass any __index
        // metamethods). Always re-query the exact per-type table so we don't
        // get false positives from fallbacks on the shared handle.
        auto absent_raw = [&](const char* name) -> bool
        {
            sol::table per = DisplayHandle::ensure_type_bind_table(lua, typeName, SUPER::TypeName);
            sol::object cur = per.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        // Note: do not register sol::property objects into the per-type
        // binding table. sol::property is meaningful when attached to a
        // usertype (it arranges metatable accessors); placing it into a
        // plain table stores a callable userdata which appears as a
        // function to Lua when accessed. Instead, expose function-style
        // getters/setters here (getLabel/getLabelText/setLabelText) and
        // rely on the minimal DisplayHandle to provide property-style
        // access (label_text) via its own sol::property registration.
        if (absent_raw("getLabelText")) {
            handle.set_function("getLabelText", [](SDOM::DisplayHandle h) -> std::string {
                if (!h.isValid()) return std::string();
                if (h->getType() != Group::TypeName) return std::string();
                Group* g = static_cast<Group*>(h.get());
                Label* l = g->getLabelPtr();
                return l ? l->getText() : std::string();
            });
        }
        if (absent_raw("setLabelText")) {
            handle.set_function("setLabelText", [](SDOM::DisplayHandle h, const std::string& v) {
                if (!h.isValid()) return;
                if (h->getType() != Group::TypeName) return;
                Group* g = static_cast<Group*>(h.get());
                Label* l = g->getLabelPtr();
                if (l) l->setText(v);
            });
        }

        // getLabel alias
        if (absent_raw("getLabel")) {
            handle.raw_set("getLabel", [](SDOM::DisplayHandle h) -> SDOM::DisplayHandle {
                if (!h.isValid()) return SDOM::DisplayHandle();
                try { if (h->getType() != Group::TypeName) return SDOM::DisplayHandle(); } catch(...) { return SDOM::DisplayHandle(); }
                Group* g = static_cast<Group*>(h.get());
                SDOM::DisplayHandle out = g ? g->getLabel() : SDOM::DisplayHandle();
                try {
                    sol::state_view L = SDOM::getLua();
                    sol::function tostring_fn = L["tostring"];
                    std::string s = out.isValid() ? std::string("valid") : std::string("invalid");
                    std::cout << "[Group::getLabel.binding] returning label handle: " << s << " type=" << (out.isValid() ? out.getType() : std::string("<nil>")) << " luaobj=" << (out.isValid() ? tostring_fn(out) : std::string("<nil>")) << std::endl;
                } catch(...) {}
                return out;
            });
        }

        // sprite width/height
        if (absent_raw("getSpriteWidth")) {
            handle.raw_set("getSpriteWidth", [](SDOM::DisplayHandle h) -> int {
                if (!h.isValid()) return 0;
                try { if (h->getType() != Group::TypeName) return 0; } catch(...) { return 0; }
                Group* g = static_cast<Group*>(h.get());
                return g ? g->getSpriteWidth() : 0;
            });
        }
        if (absent_raw("getSpriteHeight")) {
            handle.raw_set("getSpriteHeight", [](SDOM::DisplayHandle h) -> int {
                if (!h.isValid()) return 0;
                try { if (h->getType() != Group::TypeName) return 0; } catch(...) { return 0; }
                Group* g = static_cast<Group*>(h.get());
                return g ? g->getSpriteHeight() : 0;
            });
        }

        // --- Font metric accessors/mutators for Group --- //
        // Register via register_per_type to populate the authoritative C++ registry
        // as well as the legacy per-type Lua table.
        if (absent_raw("getFontSize")) {
            SDOM::register_per_type(lua, typeName, "getFontSize", [](SDOM::DisplayHandle h) -> int {
                if (!h.isValid()) return 0;
                try { if (h->getType() != Group::TypeName) return 0; } catch(...) { return 0; }
                Group* g = static_cast<Group*>(h.get());
                return g ? g->getFontSize() : 0;
            });
        }
        if (absent_raw("getFontWidth")) {
            SDOM::register_per_type(lua, typeName, "getFontWidth", [](SDOM::DisplayHandle h) -> int {
                if (!h.isValid()) return 0;
                try { if (h->getType() != Group::TypeName) return 0; } catch(...) { return 0; }
                Group* g = static_cast<Group*>(h.get());
                return g ? g->getFontWidth() : 0;
            });
        }
        if (absent_raw("getFontHeight")) {
            SDOM::register_per_type(lua, typeName, "getFontHeight", [](SDOM::DisplayHandle h) -> int {
                if (!h.isValid()) return 0;
                try { if (h->getType() != Group::TypeName) return 0; } catch(...) { return 0; }
                Group* g = static_cast<Group*>(h.get());
                return g ? g->getFontHeight() : 0;
            });
        }

        if (absent_raw("setFontSize")) {
            SDOM::register_per_type(lua, typeName, "setFontSize", [](SDOM::DisplayHandle h, int s) {
                if (!h.isValid()) return;
                try { if (h->getType() != Group::TypeName) return; } catch(...) { return; }
                Group* g = static_cast<Group*>(h.get());
                if (g) g->setFontSize(s);
            });
        }
        if (absent_raw("setFontWidth")) {
            SDOM::register_per_type(lua, typeName, "setFontWidth", [](SDOM::DisplayHandle h, int w) {
                if (!h.isValid()) return;
                try { if (h->getType() != Group::TypeName) return; } catch(...) { return; }
                Group* g = static_cast<Group*>(h.get());
                if (g) g->setFontWidth(w);
            });
        }
        if (absent_raw("setFontHeight")) {
            SDOM::register_per_type(lua, typeName, "setFontHeight", [](SDOM::DisplayHandle h, int hh) {
                if (!h.isValid()) return;
                try { if (h->getType() != Group::TypeName) return; } catch(...) { return; }
                Group* g = static_cast<Group*>(h.get());
                if (g) g->setFontHeight(hh);
            });
        }

        // group color accessors
        if (absent_raw("getGroupColor")) {
            handle.raw_set("getGroupColor", [](SDOM::DisplayHandle h) -> SDL_Color {
                if (!h.isValid()) return SDL_Color{0,0,0,0};
                try { if (h->getType() != Group::TypeName) return SDL_Color{0,0,0,0}; } catch(...) { return SDL_Color{0,0,0,0}; }
                Group* g = static_cast<Group*>(h.get());
                return g ? g->getGroupColor() : SDL_Color{0,0,0,0};
            });
        }
        if (absent_raw("setGroupColor")) {
            handle.raw_set("setGroupColor", [](SDOM::DisplayHandle h, SDL_Color c) {
                if (!h.isValid()) return;
                try { if (h->getType() != Group::TypeName) return; } catch(...) { return; }
                Group* g = static_cast<Group*>(h.get());
                if (g) g->setGroupColor(c);
            });
        }

        // expose sprite sheet and font resource getters/setters
        if (absent_raw("getSpriteSheet")) {
            handle.raw_set("getSpriteSheet", [](SDOM::DisplayHandle h) -> AssetHandle {
                if (!h.isValid()) return AssetHandle();
                if (h->getType() != Group::TypeName) return AssetHandle();
                Group* g = static_cast<Group*>(h.get());
                return g->getSpriteSheet();
            });
        }
        if (absent_raw("getFontResourceName")) {
            handle.raw_set("getFontResourceName", [](SDOM::DisplayHandle h) -> std::string {
                if (!h.isValid()) return std::string();
                if (h->getType() != Group::TypeName) return std::string();
                Group* g = static_cast<Group*>(h.get());
                return g->getFontResourceName();
            });
        }

        // Pragmatic fallback: ensure core Group methods are also present on the
        // minimal DisplayHandle global so colon-style calls (obj:getLabel())
        // resolve even if the per-type dispatcher doesn't find them for some
        // runtime reason. These are idempotent and check the handle type.
        try {
            sol::table minimal = DisplayHandle::ensure_handle_table(lua);
            auto absent_min = [&](const char* name)->bool {
                sol::object cur = minimal.raw_get_or(name, sol::lua_nil);
                return !cur.valid() || cur == sol::lua_nil;
            };
            if (absent_min("getLabel")) {
                minimal.set_function("getLabel", [](DisplayHandle h) -> DisplayHandle {
                    if (!h.isValid()) return DisplayHandle();
                    if (h->getType() != Group::TypeName) return DisplayHandle();
                    Group* g = static_cast<Group*>(h.get());
                    DisplayHandle out = g ? g->getLabel() : DisplayHandle();
                    try {
                        if (DEBUG_REGISTER_LUA) {
                            sol::state_view L = SDOM::getLua();
                            sol::function tostring_fn = L["tostring"];
                            std::string s = out.isValid() ? std::string("valid") : std::string("invalid");
                            std::cout << "[Group::getLabel.fallback] returning label handle: " << s << " type=" << (out.isValid() ? out.getType() : std::string("<nil>")) << " luaobj=" << (out.isValid() ? tostring_fn(out) : std::string("<nil>")) << std::endl;
                        }
                    } catch(...) {}
                    return out;
                });
            }
            if (absent_min("getLabelText")) {
                minimal.set_function("getLabelText", [](DisplayHandle h) -> std::string {
                    if (!h.isValid()) return std::string();
                    if (h->getType() != Group::TypeName) return std::string();
                    Group* g = static_cast<Group*>(h.get());
                    Label* l = g ? g->getLabelPtr() : nullptr;
                    return l ? l->getText() : std::string();
                });
            }
            if (absent_min("setLabelText")) {
                minimal.set_function("setLabelText", [](DisplayHandle h, const std::string& v) {
                    if (!h.isValid()) return;
                    if (h->getType() != Group::TypeName) return;
                    Group* g = static_cast<Group*>(h.get());
                    Label* l = g ? g->getLabelPtr() : nullptr;
                    if (l) l->setText(v);
                });
            }
            if (absent_min("getLabelColor")) {
                minimal.set_function("getLabelColor", [](DisplayHandle h) -> sol::object {
                    sol::state_view sv = getLua();
                    if (!h.isValid()) return sol::lua_nil;
                    if (h->getType() != Group::TypeName) return sol::lua_nil;
                    Group* g = static_cast<Group*>(h.get());
                    Label* l = g ? g->getLabelPtr() : nullptr;
                    SDL_Color c = l ? l->getColor() : SDL_Color{255,255,255,255};
                    sol::table t = sv.create_table(); t["r"] = c.r; t["g"] = c.g; t["b"] = c.b; t["a"] = c.a; return sol::make_object(sv, t);
                });
            }
            if (absent_min("setLabelColor")) {
                minimal.set_function("setLabelColor", [](DisplayHandle h, sol::table t) {
                    if (!h.isValid()) return;
                    if (h->getType() != Group::TypeName) return;
                    SDL_Color c = {255,255,255,255};
                    if (t["r"].valid()) c.r = static_cast<Uint8>(t["r"].get<int>());
                    if (t["g"].valid()) c.g = static_cast<Uint8>(t["g"].get<int>());
                    if (t["b"].valid()) c.b = static_cast<Uint8>(t["b"].get<int>());
                    if (t["a"].valid()) c.a = static_cast<Uint8>(t["a"].get<int>());
                    Group* g = static_cast<Group*>(h.get());
                    Label* l = g ? g->getLabelPtr() : nullptr;
                    if (l) l->setColor(c);
                });
            }
            // Intentionally omit registering `label_text` as a property on the
            // minimal DisplayHandle. Exposing `label_text` as a property on the
            // shared usertype leads to name collisions between types (Group,
            // Label, Button, etc.). If a property cannot be safely provided
            // per-type without polluting the shared namespace, prefer the
            // function-style accessors (getLabel/getLabelText/setLabelText)
            // which are registered above and avoid ambiguous semantics.
            if (absent_min("getSpriteWidth")) {
                minimal.set_function("getSpriteWidth", [](DisplayHandle h) -> int {
                    if (!h.isValid()) return 0;
                    if (h->getType() != Group::TypeName) return 0;
                    Group* g = static_cast<Group*>(h.get());
                    return g ? g->getSpriteWidth() : 0;
                });
            }
            if (absent_min("getSpriteHeight")) {
                minimal.set_function("getSpriteHeight", [](DisplayHandle h) -> int {
                    if (!h.isValid()) return 0;
                    if (h->getType() != Group::TypeName) return 0;
                    Group* g = static_cast<Group*>(h.get());
                    return g ? g->getSpriteHeight() : 0;
                });
            }
            // Minimal fallback for font metric accessors so Lua colon-style
            // calls resolve even if per-type bindings or registry lookups fail.
            if (absent_min("getFontSize")) {
                minimal.set_function("getFontSize", [](DisplayHandle h) -> int {
                    if (!h.isValid()) return 0;
                    if (h->getType() != Group::TypeName) return 0;
                    Group* g = static_cast<Group*>(h.get());
                    return g ? g->getFontSize() : 0;
                });
            }
            if (absent_min("getFontWidth")) {
                minimal.set_function("getFontWidth", [](DisplayHandle h) -> int {
                    if (!h.isValid()) return 0;
                    if (h->getType() != Group::TypeName) return 0;
                    Group* g = static_cast<Group*>(h.get());
                    return g ? g->getFontWidth() : 0;
                });
            }
            if (absent_min("getFontHeight")) {
                minimal.set_function("getFontHeight", [](DisplayHandle h) -> int {
                    if (!h.isValid()) return 0;
                    if (h->getType() != Group::TypeName) return 0;
                    Group* g = static_cast<Group*>(h.get());
                    return g ? g->getFontHeight() : 0;
                });
            }
            if (absent_min("setFontSize")) {
                minimal.set_function("setFontSize", [](DisplayHandle h, int s) {
                    if (!h.isValid()) return;
                    if (h->getType() != Group::TypeName) return;
                    Group* g = static_cast<Group*>(h.get());
                    if (g) g->setFontSize(s);
                });
            }
            if (absent_min("setFontWidth")) {
                minimal.set_function("setFontWidth", [](DisplayHandle h, int w) {
                    if (!h.isValid()) return;
                    if (h->getType() != Group::TypeName) return;
                    Group* g = static_cast<Group*>(h.get());
                    if (g) g->setFontWidth(w);
                });
            }
            if (absent_min("setFontHeight")) {
                minimal.set_function("setFontHeight", [](DisplayHandle h, int hh) {
                    if (!h.isValid()) return;
                    if (h->getType() != Group::TypeName) return;
                    Group* g = static_cast<Group*>(h.get());
                    if (g) g->setFontHeight(hh);
                });
            }
        } catch(...) {}


        // --- additional Group-specific bindings can go here --- //

        // (Diagnostics removed)

    } // END: void Group::_registerLuaBindings(const std::string& typeName, sol::state_view lua)



} // END: namespace SDOM