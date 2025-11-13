// ============================================================================
// SDOM_TristateButton.cpp
// ----------------------------------------------------------------------------
// Implementation for SDOM::TristateButton.
//  Purpose:
//    Composite three-state button with an icon and label. Handles creation of
//    its child IconButton/Label, tri-state cycling on click, icon selection by
//    state, and Lua registration glue.
//  Notes:
//    • Keep helpers in an anonymous namespace or mark them `static`.
//    • Prefer documenting in headers (Doxygen lives in .hpp).
//    • Include order: C++ stdlib → SDL → SDOM core → SDOM_TristateButton.hpp.
//
// ----------------------------------------------------------------------------
// License: ZLIB
// ----------------------------------------------------------------------------
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
// ----------------------------------------------------------------------------
// Author: Jay Faries (https://github.com/warte67)
// ============================================================================


#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_IconIndex.hpp>
#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_Button.hpp>
#include <SDOM/SDOM_ArrowButton.hpp>

#include <SDOM/SDOM_TristateButton.hpp>

namespace SDOM
{
    namespace {
        inline IconIndex iconIndexFromState(ButtonState state) {
            switch (state) {
                case ButtonState::Inactive:   return IconIndex::Checkbox_Empty;
                case ButtonState::Active:     return IconIndex::Checkbox_Checked;
                case ButtonState::Mixed:      return IconIndex::Checkbox_X;
                default:                      return IconIndex::Checkbox_Empty;
            }
        }
    }
    // --------------------------------------------------------------------
    // 🏭 Constructors
    // --------------------------------------------------------------------

    TristateButton::TristateButton(const InitStruct& init) : IDisplayObject(init)
    {
        // std::cout << "Box constructed with InitStruct: " << getName() 
        //           << " at address: " << this << std::endl;

        // if (init.type != TypeName) {
        //     // INFO("TristateButton::TristateButton(init) - constructed with derived type: " << init.type);
        // }

        // from IDisplayObject (this should already be set by IDisplayObject constructor; verify and remove)
        setName(init.name);
        setType(init.type);
        setColor(init.color);
        setTabEnabled(init.tabEnabled);
        setClickable(init.isClickable);

        // TristateButton Initialization Properties
        text_ = init.text;
        // icon_index_ = init.icon_index;
        buttonState_ = init.state;
        // Use a local helper to compute icon index from state. Avoid calling
        // the virtual accessor during construction to satisfy static analyzers.
        icon_index_ = iconIndexFromState(buttonState_);

        font_size_ = init.font_size;
        label_color_ = init.label_color;
        border_color_ = init.border_color;
        buttonState_ = init.state;
        icon_resource_ = init.icon_resource;
        icon_width_ = init.icon_width;
        icon_height_ = init.icon_height;
        font_resource_ = init.font_resource; 
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        use_border_ = init.border;

    } // END: TristateButton::TristateButton(const InitStruct& init)

    TristateButton::TristateButton(const sol::table& config) : TristateButton(config, InitStruct())
    {
    }

    TristateButton::TristateButton(const sol::table& config, const InitStruct& defaults) : IDisplayObject(config, defaults)
    {
        // std::cout << "Box constructed with Lua config: " << getName() 
        //         << " at address: " << this << std::endl;            

        InitStruct init;

        // std::string type = config["type"].valid() ? config["type"].get<std::string>() : init.type;
        // if (type != "TypeName") {
        //     // See note above: accept derived type names and only log the mismatch.
        //     INFO("TristateButton::TristateButton(lua) - constructed with derived type: " << type);
        // } 

        // Lambda Helpers
        auto get_string_if_valid = [](const sol::table& tbl, const char* key) -> std::string {
            if (tbl[key].valid()) {
                try {
                    return tbl[key].get<std::string>();
                } catch (...) {
                    // ignore non-string values
                }
            }
            return {};
        };  
        auto get_string_with_fallback = [&](const sol::table& tbl, const char* key1, const char* key2) -> std::string {
            std::string v = get_string_if_valid(tbl, key1);
            if (!v.empty()) return v;
            return get_string_if_valid(tbl, key2);
        };

        // Apply Lua-provided strings (fall back to InitStruct defaults when absent)
        text_ = get_string_if_valid(config, "text");
        font_resource_ = get_string_with_fallback(config, "font_resource", "font_resource_name");
        icon_resource_ = get_string_with_fallback(config, "icon_resource", "icon_resource_name");

        // If the helpers returned empty (no config provided), fall back to InitStruct defaults
        if (font_resource_.empty()) font_resource_ = init.font_resource;
        if (icon_resource_.empty()) icon_resource_ = init.icon_resource;

        // Apply Lua-provided font properties to the TriStateCheckbox (accept multiple key variants).
        font_size_ = config["font_size"].valid() ? config["font_size"].get<int>() : init.font_size;
        font_width_ = config["font_width"].valid() ? config["font_width"].get<int>() : init.font_width;
        font_height_ = config["font_height"].valid() ? config["font_height"].get<int>() : init.font_height;
        icon_width_ = config["icon_width"].valid() ? config["icon_width"].get<int>() : init.icon_width;
        icon_height_ = config["icon_height"].valid() ? config["icon_height"].get<int>() : init.icon_height;
        use_border_ = config["border"].valid() ? config["border"].get<bool>() : init.border;

        // // icon_index - accept int or string aliases (default to Checkbox_X if invalid)
        // if (config["icon_index"].valid()) {
        //     try {
        //         sol::object o = config["icon_index"];
        //         if (o.get_type() == sol::type::number) {
        //             icon_index_ = static_cast<IconIndex>(o.as<int>());
        //         } else if (o.get_type() == sol::type::string) {
        //             std::string s = o.as<std::string>();
        //             for (auto &c : s) c = static_cast<char>(std::tolower(c));
        //             // If you have a string-to-icon-index map, use it here:
        //             auto opt = icon_index_from_name(s); // e.g., returns std::optional<int>
        //             if (opt) {
        //                 icon_index_ = *opt;
        //             } else {
        //                 icon_index_ = IconIndex::Checkbox_X; // fallback default
        //             }
        //         }
        //     } catch (...) {
        //         icon_index_ = IconIndex::Checkbox_X; // fallback on error
        //     }
        // } else {
        //     icon_index_ = IconIndex::Checkbox_X; // fallback if not provided
        // }

        // parse state allowing int or string aliases
        if (config["state"].valid()) 
        {
            try 
            {
                sol::object o = config["state"];
                if (o.get_type() == sol::type::number) 
                {
                    buttonState_ = static_cast<ButtonState>(o.as<int>());
                } 
                else if (o.get_type() == sol::type::string) 
                {
                    std::string s = o.as<std::string>();
                    for (auto &c : s) c = static_cast<char>(std::tolower(c));
                    auto opt = button_state_from_name(s);
                    if (opt) 
                    {
                        buttonState_ = *opt;
                    } 
                    else if (s == "0") 
                    {
                        buttonState_ = ButtonState::Inactive;
                    } 
                    else if (s == "1") 
                    {
                        buttonState_ = ButtonState::Active;
                    } 
                    else if (s == "2") 
                    {
                        buttonState_ = ButtonState::Mixed;
                    }
                }
            } catch(...) { /* ignore malformed values */ }
        }
        // Use helper instead of virtual call
        icon_index_ = iconIndexFromState(buttonState_); // set icon index based on initial state

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

            if (config["border_color"].valid() && config["border_color"].get_type() == sol::type::table) {
                sol::table bc = config["border_color"];
                border_color_.r = bc["r"].get_or((int)border_color_.r);
                border_color_.g = bc["g"].get_or((int)border_color_.g);
                border_color_.b = bc["b"].get_or((int)border_color_.b);
                border_color_.a = bc["a"].get_or((int)border_color_.a);
            } else if (config["borderColor"].valid() && config["borderColor"].get_type() == sol::type::table) {
                sol::table bc = config["borderColor"];
                border_color_.r = bc["r"].get_or((int)border_color_.r);
                border_color_.g = bc["g"].get_or((int)border_color_.g);
                border_color_.b = bc["b"].get_or((int)border_color_.b);
                border_color_.a = bc["a"].get_or((int)border_color_.a);
            }
        } catch (...) { /* ignore malformed color tables */ }
    } // END: TristateButton::TristateButton(const sol::table& config)


    // --------------------------------------------------------------------
    // 🌱 Lifecycle
    // --------------------------------------------------------------------

    bool TristateButton::onInit() 
    { 
        // create the IconButton object
        if (!iconButtonObject_)
        {
            IconButton::InitStruct init;
            init.name = getName() + "_iconbutton";
            init.type = "IconButton";
            init.icon_resource = icon_resource_;
            init.x = getX();
            init.y = getY();
            init.width = 8;
            init.height = 8;   
            init.isClickable = false;
            init.tabEnabled = false;
            init.color = getColor(); // use the color of the TriStateCheckbox to color the icon
            init.icon_index = icon_index_;
            iconButtonObject_ = getFactory().createDisplayObject("IconButton", init);
            addChild(iconButtonObject_);
        }

        // create the label object
        if (!labelObject_)
        {
            Label::InitStruct init;
            init.name = getName() + "_label";
            init.type = "Label";
            init.x = getX() + 12;     // adjusted below
            init.y = getY() + 0;      // adjusted below
            init.width = getWidth();  // adjusted below
            init.height = 12;         // adjusted below
            init.alignment = LabelAlign::MIDDLE_LEFT;
            init.anchorLeft = AnchorPoint::TOP_LEFT;
            init.anchorTop = AnchorPoint::TOP_LEFT; 
            init.anchorRight = AnchorPoint::TOP_LEFT;
            init.anchorBottom = AnchorPoint::TOP_LEFT;
            init.resourceName = font_resource_;
            init.border = use_border_;
            // init.color = label_color_;
            init.outlineColor = {0, 0, 0, 255};
            init.outlineThickness = 1;
            init.outline = true;
            init.borderThickness = 1;
            // init.dropshadow = true;
            init.foregroundColor = label_color_;
            init.borderColor = border_color_;
            init.color = label_color_;  // label color
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
            // INFO("TriStateCheckbox::onInit() - Label init metrics for '" + init.name + "': resource='" + init.resourceName + "' fontSize=" + std::to_string(init.fontSize) + " fontWidth=" + std::to_string(init.fontWidth) + " fontHeight=" + std::to_string(init.fontHeight));
            labelObject_ = getFactory().createDisplayObject("Label", init);
            addChild(labelObject_);
        }
        // center the icon vertically within the TriStateCheckbox height
        if (iconButtonObject_.isValid())
        {
            IconButton* iconBtn = iconButtonObject_.as<IconButton>();
            if (iconBtn)
            {
                int iconY = getY() + (getHeight() / 2 - icon_height_ / 2);
                iconBtn->setY(iconY);
                // Ensure the child IconButton reflects the current checked state
                // int state = static_cast<int>(state_);
                // int iconIndex = state + static_cast<int>(IconIndex::Checkbox_Empty);
                // iconBtn->setIconIndex(static_cast<IconIndex>(iconIndex));
                iconBtn->setIconIndex(static_cast<IconIndex>(icon_index_));
            }
        }
        IconButton* icn = iconButtonObject_.as<IconButton>();
        if (!icn)
        {
            ERROR("Error: TristateButton::onInit() - iconButtonObject_ is not an IconButton for '" + getName() + "'");
            return false;
        }
        SpriteSheet* ss = icn->getSpriteSheetPtr();
        if (ss == nullptr)
        {
            ERROR("Error: TristateButton::onInit() - iconButtonObject_ is not a SpriteSheet for '" + getName() + "'");
            return false;
        }
        icn->setWidth(ss->getSpriteWidth());
        icn->setHeight(ss->getSpriteHeight());
        icn->setX(getX() ); // - ss->getSpriteWidth()/2);
        int iconButtonY = getY() + (getHeight() / 2 - icn->getHeight() / 2) - 0;
        icn->setY(iconButtonY);
        if (labelObject_.isValid())
        {
            Label* label = labelObject_.as<Label>();
            if (label)
            {
                int labelX = getX() + icn->getWidth() + 4;
                label->setX(labelX);
                label->setWidth(getWidth() - icn->getWidth() - 4);
                int labelY = getY() + (getHeight() / 2 - label->getHeight() / 2) - 1;
                label->setY(labelY);
            }
        }


        return SUPER::onInit();

    } // END: bool TristateButton::onInit()


    void TristateButton::onQuit() 
    {
        SUPER::onQuit();
    } // END: void TristateButton::onQuit()


    void TristateButton::onUpdate(float) 
    {

    } // END: void TristateButton::onUpdate(float) 


    void TristateButton::onEvent(const Event& event) 
    {
        // only target phase
        if (event.getPhase() != Event::Phase::Target) { return; }
        if (event.getType() == EventType::MouseClick) 
        { 
            setState(static_cast<ButtonState>((static_cast<int>(getState()) + 1) % 3));
            // INFO(getType() +"::onEvent() - MouseClick on '" + getName() + "' new state: " + std::to_string(static_cast<int>(getState())));
        }

    } // END: void TristateButton::onEvent(const Event& event)


    void TristateButton::onRender() 
    {
        Label* label = labelObject_.as<Label>();
        if (!label) { ERROR("Error: TriStateCheckbox::onRender() - missing label object for: " + getName()); return; }

        // // Debug info: report presence of internal icon and label children
        // try {
        //     std::ostringstream dbg;
        //     dbg << "[DBG] TriStateCheckbox::onRender() - name='" << getName() << "' hasLabel=" << (label ? "yes" : "no");
        //     if (iconButtonObject_.isValid()) dbg << " iconChild='" << iconButtonObject_.getName() << "'";
        //     else dbg << " iconChild=none";
        //     LUA_INFO(dbg.str());
        // } catch(...) {}

        // use the border settings from the Label Object
        FontStyle& fontStyle = label->getDefaultStyle();
        SDL_Color borderColor = fontStyle.borderColor;
        if (fontStyle.border) 
        {
            SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(getRenderer(), borderColor.r, borderColor.g, borderColor.b, borderColor.a);
            SDL_FRect rect = { 
                static_cast<float>(getX()), 
                static_cast<float>(getY()), 
                static_cast<float>(getWidth()), 
                static_cast<float>(getHeight()) };
            SDL_RenderRect(getRenderer(), &rect);
        }

    } // END: void TristateButton::onRender()
        

    bool TristateButton::onUnitTest(int frame)
    {
        // Run base checks first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Register tests only once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Validate button state
            ut.add_test(objName, "TristateButton State Validation", [this](std::vector<std::string>& errors)
            {
                switch (getState())
                {
                    case ButtonState::Inactive:
                    case ButtonState::Active:
                    case ButtonState::Mixed:
                        break;
                    default:
                        errors.push_back("TristateButton '" + getName() +
                                        "' has invalid state: " +
                                        std::to_string(static_cast<int>(getState())));
                        break;
                }
                return true; // ✅ single-frame
            });

            // 🔹 2. Validate icon sprite (if present)
            ut.add_test(objName, "TristateButton Icon Sprite Validation", [this](std::vector<std::string>& errors)
            {
                SpriteSheet* ss = getIconSpriteSheet();
                if (ss)
                {
                    if (ss->getSpriteWidth() <= 0 || ss->getSpriteHeight() <= 0)
                    {
                        errors.push_back("TristateButton '" + getName() +
                                        "' has invalid icon sprite size: w=" +
                                        std::to_string(ss->getSpriteWidth()) +
                                        " h=" + std::to_string(ss->getSpriteHeight()));
                    }
                }
                return true;
            });

            registered = true;
        }

        // ✅ Return false so this test set remains active during test processing
        return false;
    } // END: TristateButton::onUnitTest()


    // --------------------------------------------------------------------
    // 🧩 Virtual State Accessors (From IButtonObject)
    // --------------------------------------------------------------------

    ButtonState TristateButton::getState() const 
    { 
        return buttonState_; 
    } // END: virtual ButtonState getState() const


    void TristateButton::setState(ButtonState state) 
    { 
        if (state == buttonState_) return; // no change
        // INFO(getType() +"::setState() - on '" + getName() + "' new state: " + std::to_string(static_cast<int>(getState())));

        onStateChanged(buttonState_, state); 
        // dispatch event
        queue_event(EventType::StateChanged, [this, state](Event& ev) {
            ev.setPayloadValue("old_state", static_cast<int>(buttonState_));
            ev.setPayloadValue("new_state", static_cast<int>(state));
            ev.setPayloadValue("buttonName", getName());
        });
        buttonState_ = state;
        // Keep cached icon index in sync with new state
        icon_index_ = iconIndexForState(state);
        // Update internal icon button if present. Prefer the internal child handle
        // created during onInit(); otherwise, try to resolve the conventional
        // child name '<thisname>_iconbutton' as a fallback.
        IconButton* ib = nullptr;
        if (iconButtonObject_.isValid()) {
            ib = iconButtonObject_.as<IconButton>();
        } else {
            // fallback: try to find the child by the conventional name
            std::string childName = getName() + "_iconbutton";
            DisplayHandle h = getFactory().getDisplayObject(childName);
            if (h.isValid()) ib = dynamic_cast<IconButton*>(h.get());
        }

        if (ib)
        {
            // int stateInt = static_cast<int>(state);
            // int iconIndex = stateInt + static_cast<int>(IconIndex::Checkbox_Empty);
            int iconIndex = static_cast<int>(iconIndexForState(state));
            ib->setIconIndex(static_cast<IconIndex>(iconIndex));
            setDirty(true);
        }
        else
        {
            // degrade gracefully: log debug instead of throwing an error so that
            // callers (including Factory/Init events) won't abort the program.
            DEBUG_LOG("TristateButton::setState() - internal IconButton not found for: " + getName());
        }
    } // END: virtual void setState(ButtonState state)


    // --------------------------------------------------------------------
    // 🧩 Public Accessors
    // --------------------------------------------------------------------

    DisplayHandle TristateButton::getLabelObject() const { return labelObject_; }
    std::string TristateButton::getText() const { return text_; }
    DisplayHandle TristateButton::getIconButtonObject() const { return iconButtonObject_; }


    IconButton* TristateButton::getIconButton() const
    {
        // Prefer the internal child handle created during onInit()
        if (iconButtonObject_.isValid()) {
            return iconButtonObject_.as<IconButton>();
        }
        // Fallback: try the conventional child name '<thisname>_iconbutton'
        std::string childName = getName() + "_iconbutton";
        DisplayHandle h = getFactory().getDisplayObject(childName);
        if (h.isValid()) {
            return dynamic_cast<IconButton*>(h.get());
        }
        return nullptr;
    } // END: IconButton* getIconButton() const


    SpriteSheet* TristateButton::getIconSpriteSheet() const
    {
        // If the internal IconButton exists, prefer its loaded SpriteSheet
        if (iconButtonObject_.isValid())
        {
            if (auto ib = iconButtonObject_.as<IconButton>())
            {
                if (auto ss = ib->getSpriteSheetPtr())
                    return ss;
            }
        }

        // Fallbacks based on resource name/filename
        if (icon_resource_.empty())
        {
            DEBUG_LOG("TristateButton::getIconSpriteSheet() - icon_resource_ is empty.");
            return nullptr;
        }

        Factory& factory = getFactory();
        // Try direct asset name
        AssetHandle ss_handle = factory.getAssetObject(icon_resource_);
        if (!ss_handle.isValid())
        {
            // Try common suffix naming
            ss_handle = factory.getAssetObject(icon_resource_ + std::string("_SpriteSheet"));
        }
        if (!ss_handle.isValid())
        {
            // Try locating a SpriteSheet that references the same filename
            ss_handle = factory.findAssetByFilename(icon_resource_, SpriteSheet::TypeName);
        }

        if (!ss_handle.isValid())
        {
            DEBUG_LOG("TristateButton::getIconSpriteSheet() - Could not resolve SpriteSheet for resource: " + icon_resource_);
            return nullptr;
        }

        return ss_handle.as<SpriteSheet>();
    } // END: SpriteSheet* TristateButton::getIconSpriteSheet() const;


    Label* TristateButton::getLabel() const
    {
        // Prefer the internal label child created during onInit()
        if (labelObject_.isValid())
        {
            if (auto lbl = labelObject_.as<Label>())
                return lbl;
        }
        // Fallback: conventional child name '<thisname>_label'
        std::string childName = getName() + "_label";
        DisplayHandle h = getFactory().getDisplayObject(childName);
        if (h.isValid())
        {
            return h.as<Label>();
        }
        return nullptr;
    } // END: Label* TristateButton::getLabel() const;

    std::string TristateButton::getFontResource() const { return font_resource_; }
    std::string TristateButton::getIconResource() const { return icon_resource_; }
    int TristateButton::getFontSize() const { return font_size_; }
    int TristateButton::getFontWidth() const { return font_width_; }
    int TristateButton::getFontHeight() const { return font_height_; }
    bool TristateButton::getUseBorder() const { return use_border_; }
    SDL_Color TristateButton::getLabelColor() const { return label_color_; }
    SDL_Color TristateButton::getBorderColor() const { return border_color_; }
    int TristateButton::getIconWidth() const { return icon_width_; }
    int TristateButton::getIconHeight() const { return icon_height_; }
    IconIndex TristateButton::getIconIndex() const { return iconIndexForState(buttonState_); }


    // --------------------------------------------------------------------
    // 🧩 Public Mutators
    // --------------------------------------------------------------------

    void TristateButton::setText(const std::string& newText) 
    {
        if (text_ != newText)   setDirty(true);
        text_ = newText;
        // If the internal label exists, update it immediately
        if (labelObject_.isValid())
        {
            auto lbl = labelObject_.as<Label>();
            if (lbl)
            {
                // defensive: only call if Label implements setText
                try {
                    lbl->setText(text_);
                } catch (...) { /* ignore if not available */ }
            }
        }
    } // END: void TristateButton::setText(const std::string& newText)


    // --------------------------------------------------------------------
    // ⚙️ Internal Virtuals
    // --------------------------------------------------------------------

    void TristateButton::onStateChanged(ButtonState, ButtonState) 
    {
        // ...
    } // END: void TristateButton::onStateChanged(ButtonState oldState, ButtonState newState)

    IconIndex TristateButton::iconIndexForState(ButtonState state) const
    {
        switch (state) {
            case ButtonState::Inactive:   return IconIndex::Checkbox_Empty;
            case ButtonState::Active:     return IconIndex::Checkbox_Checked;
            case ButtonState::Mixed:      return IconIndex::Checkbox_X;
            default:                      return IconIndex::Checkbox_Empty;
        }
    } // END: IconIndex TristateButton::iconIndexForState(ButtonState state) const


    // --------------------------------------------------------------------
    // 📜 Lua Registration
    // --------------------------------------------------------------------

    void TristateButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua) 
    {
        // Include IButtonObject bindings first
        IButtonObject::registerLuaBindings(lua);

        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "TriStateCheckbox";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // Ensure usertype + both legacy and canonical per-type tables.
        auto [typeTbl, regTbl] = SDOM::IDataObject::ensure_type_tables<TristateButton, SDOM::IDisplayObject>(lua, typeName);

        // Helper to bind onto both the legacy globals[typeName] table and the
        // canonical SDOM_Bindings[typeName] registry (idempotent on both).
        auto bind_type = [&](const char* name, auto&& fn) {
            sol::object cur1 = typeTbl.raw_get_or(name, sol::lua_nil);
            if (!cur1.valid() || cur1 == sol::lua_nil) {
                typeTbl.set_function(name, std::forward<decltype(fn)>(fn));
            }
            sol::object cur2 = regTbl.raw_get_or(name, sol::lua_nil);
            if (!cur2.valid() || cur2 == sol::lua_nil) {
                regTbl.set_function(name, std::forward<decltype(fn)>(fn));
            }
        };

        // -----------------------------------------------------------------
        // Accessors (multi-type aware to avoid overriding other classes)
        // -----------------------------------------------------------------
        bind_type("getLabelObject", [](DisplayHandle& self) -> DisplayHandle {
            if (auto* b = self.as<Button>()) return b->getLabelObject();
            if (auto* tb = self.as<TristateButton>()) return tb->getLabelObject();
            return {};
        });

        bind_type("getText", [](DisplayHandle& self) -> std::string {
            if (auto* b = self.as<Button>()) return b->getText();
            if (auto* tb = self.as<TristateButton>()) return tb->getText();
            return {};
        });

        bind_type("getIconButtonObject", [](DisplayHandle& self) -> DisplayHandle {
            if (auto* tb = self.as<TristateButton>()) return tb->getIconButtonObject();
            return {};
        });

        bind_type("getFontResource", [](DisplayHandle& self) -> std::string {
            if (auto* b = self.as<Button>()) return b->getFontResource();
            if (auto* tb = self.as<TristateButton>()) return tb->getFontResource();
            return {};
        });
        bind_type("getIconResource", [](DisplayHandle& self) -> std::string {
            if (auto* tb = self.as<TristateButton>()) return tb->getIconResource();
            return {};
        });
        bind_type("getFontSize", [](DisplayHandle& self) -> int {
            if (auto* tb = self.as<TristateButton>()) return tb->getFontSize();
            return 0;
        });
        bind_type("getFontWidth", [](DisplayHandle& self) -> int {
            if (auto* tb = self.as<TristateButton>()) return tb->getFontWidth();
            return 0;
        });
        bind_type("getFontHeight", [](DisplayHandle& self) -> int {
            if (auto* tb = self.as<TristateButton>()) return tb->getFontHeight();
            return 0;
        });
        bind_type("getUseBorder", [](DisplayHandle& self) -> bool {
            if (auto* tb = self.as<TristateButton>()) return tb->getUseBorder();
            return false;
        });
                        
        // ------------------------------------------------------------------------
        // 📜 Lua Bindings: Uint8-safe Color Accessors for TristateButton
        // ------------------------------------------------------------------------
        auto make_color_table = [&](sol::state_view lua, const SDL_Color& c) -> sol::table {
            sol::table t = lua.create_table();
            auto as_int = [](Uint8 v) -> int { return static_cast<int>(v & 0xFF); };
            t["r"] = as_int(c.r);
            t["g"] = as_int(c.g);
            t["b"] = as_int(c.b);
            t["a"] = as_int(c.a);
            t[1] = as_int(c.r);
            t[2] = as_int(c.g);
            t[3] = as_int(c.b);
            t[4] = as_int(c.a);
            return t;
        };

        // Clamp helper (inline lambda)
        auto clamp8 = [](int v) -> Uint8 {
            if (v < 0) return 0;
            if (v > 255) return 255;
            return static_cast<Uint8>(v);
        };

        // --- getLabelColor ---
        bind_type("getLabelColor",
            [lua, clamp8, make_color_table](DisplayHandle& self) mutable -> sol::table {
                SDL_Color c{255, 0, 255, 255}; // magenta fallback
                if (auto* tb = self.as<TristateButton>())        c = tb->getLabelColor();

                // Clamp & cast
                c.r = clamp8(static_cast<int>(c.r));
                c.g = clamp8(static_cast<int>(c.g));
                c.b = clamp8(static_cast<int>(c.b));
                c.a = clamp8(static_cast<int>(c.a));

                return make_color_table(lua, c);
            });

        // --- getBorderColor ---
        bind_type("getBorderColor",
            [lua, clamp8, make_color_table](DisplayHandle& self) mutable -> sol::table {
                SDL_Color c{0, 0, 0, 255};
                if (auto* tb = self.as<TristateButton>())        c = tb->getBorderColor();

                // Clamp & cast
                c.r = clamp8(static_cast<int>(c.r));
                c.g = clamp8(static_cast<int>(c.g));
                c.b = clamp8(static_cast<int>(c.b));
                c.a = clamp8(static_cast<int>(c.a));

                return make_color_table(lua, c);
            });



        bind_type("getIconWidth", [](DisplayHandle& self) -> int {
            if (auto* tb = self.as<TristateButton>()) return tb->getIconWidth();
            return 0;
        });
        bind_type("getIconHeight", [](DisplayHandle& self) -> int {
            if (auto* tb = self.as<TristateButton>()) return tb->getIconHeight();
            return 0;
        });
        bind_type("getIconIndex", [](DisplayHandle& self) -> int {
            if (auto* ab = self.as<ArrowButton>()) return static_cast<int>(ab->getIconIndex());
            if (auto* tb = self.as<TristateButton>()) return static_cast<int>(tb->getIconIndex());
            if (auto* ib = self.as<IconButton>()) return static_cast<int>(ib->getIconIndex());
            return -1;
        });

        // -----------------------------------------------------------------
        // Mutators (multi-type)
        // -----------------------------------------------------------------
        bind_type("setText", [](DisplayHandle& self, const std::string& newText) {
            if (auto* b = self.as<Button>()) b->setText(newText);
            else if (auto* tb = self.as<TristateButton>()) tb->setText(newText);
        });

    } // END: void TristateButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    


    void TristateButton::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "TristateButton");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    

} // END: namespace SDOM
