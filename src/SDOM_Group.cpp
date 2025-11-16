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
        label_color_ = init.label_color;

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
            labelObject_ = getFactory().createDisplayObject("Label", init);
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
                        if (!fo->isLoaded()) fo->load();

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


    void Group::onUpdate([[maybe_unused]] float fElapsedTime)
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

        
    bool Group::onUnitTest(int frame)
    {
        // Run base class tests first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Only register once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Validate that labelObject_ exists
            ut.add_test(objName, "LabelObject Validity", [this](std::vector<std::string>& errors)
            {
                if (!labelObject_.isValid())
                    errors.push_back("Group '" + getName() + "' labelObject_ is not valid.");
                return true; // ✅ single-frame test
            });

            // 🔹 2. Validate that labelObject_ is actually a Label
            ut.add_test(objName, "LabelObject Type Check", [this](std::vector<std::string>& errors)
            {
                if (labelObject_.isValid())
                {
                    auto lbl = labelObject_.as<Label>();
                    if (!lbl)
                        errors.push_back("Group '" + getName() + "' labelObject_ is not a Label.");
                }
                return true; // ✅ single-frame test
            });

            // 🔹 3. Validate that label text matches Group’s text_
            ut.add_test(objName, "Label Text Consistency", [this](std::vector<std::string>& errors)
            {
                if (labelObject_.isValid())
                {
                    auto lbl = labelObject_.as<Label>();
                    if (lbl && lbl->getText() != text_)
                    {
                        errors.push_back(
                            "Group '" + getName() + "' label text mismatch. Expected: '" + text_ +
                            "' Got: '" + lbl->getText() + "'");
                    }
                }
                return true; // ✅ single-frame test
            });

            registered = true;
        }

        // ✅ Return false so it remains active for frame synchronization
        return false;
    } // END: Group::onUnitTest()





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




    
    void Group::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "Group");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    


} // END: namespace SDOM
