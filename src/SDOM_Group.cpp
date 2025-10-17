// SDOM_Group.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_TruetypeFont.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_Group.hpp>



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

        // Augment the single shared DisplayHandle handle usertype
        sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);

        // Helper to check if a property/command is already registered
        auto absent = [&](const char* name) -> bool 
        {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        // expose 'name' property for Group (maps to getName / setName on the display object)
        if (absent("name"))
        {
            handle.set("name", sol::property(
                // getter
                [](SDOM::DisplayHandle h) -> std::string 
                {
                    if (h.isValid()) return h->getName();
                    return std::string();
                },
                // setter
                [](SDOM::DisplayHandle h, const std::string& v) 
                {
                    if (h.isValid()) h->setName(v);
                }
            ));
        }

        // expose 'label' property that returns the attached label DisplayHandle (read-only)
        if (absent("label"))
        {
            handle.set("label", sol::property(
                // getter
                [](SDOM::DisplayHandle h) -> SDOM::DisplayHandle 
                {
                    if (!h.isValid()) return SDOM::DisplayHandle();
                    // ensure this is a Group before casting
                    if (h->getType() != Group::TypeName) return SDOM::DisplayHandle();
                    Group* btn = static_cast<Group*>(h.get());
                    return btn->getLabelObject();
                }
            ));
        }

        // --- additional Group-specific bindings can go here --- //

    } // END: void Group::_registerLuaBindings(const std::string& typeName, sol::state_view lua)



} // END: namespace SDOM