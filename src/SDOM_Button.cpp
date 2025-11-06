// ============================================================================
// SDOM_Button.cpp
// ----------------------------------------------------------------------------
// Implementation file for SDOM::Button
//
// Purpose:
//     Implements the logic, rendering, and event handling for the Button class.
//     This file defines how Button objects initialize, render, and respond to
//     user interaction via mouse and keyboard input.
//
// Notes:
//     - Helper functions should remain in anonymous namespaces or be marked `static`.
//     - Doxygen comments should exist only in the header, not the implementation.
//     - Maintain strict include order: standard library, SDL, SDOM core, then local.
//     - All debug, logging, and test hooks should use SDOM-provided utilities.
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
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_Utils.hpp>
#include <SDOM/SDOM_Button.hpp>



namespace SDOM
{
    namespace {
        template <typename F>
        inline void dh_bind_both_impl(sol::table& handle,
                                      sol::optional<sol::usertype<DisplayHandle>>& maybeUT,
                                      const char* name,
                                      F&& fn)
        {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                handle.set_function(name, std::forward<F>(fn));
            }
            if (maybeUT) {
                try { (*maybeUT)[name] = fn; } catch(...) {}
            }
        }
    }
    Button::Button(const InitStruct& init) : IPanelObject(init)
    {
        // std::cout << "Box constructed with InitStruct: " << getName() 
        //           << " at address: " << this << std::endl;

        if (init.type != TypeName) {
            ERROR("Error: Button constructed with incorrect type: " + init.type);
        }
        // Button Specific
        text_ = init.text;
        base_index_ = init.base_index;
        font_size_ = init.font_size; 
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        font_resource_ = init.font_resource;
        label_color_ = init.label_color;
    }


    Button::Button(const sol::table& config) : IPanelObject(config, InitStruct())
    {
        // std::cout << "Box constructed with Lua config: " << getName() 
        //         << " at address: " << this << std::endl;            

        std::string type = config["type"].valid() ? config["type"].get<std::string>() : "";

        // INFO("Button::Button(const sol::table& config) -- name: " << getName() 
        //         << " type: " << type << " typeName: " << TypeName << std::endl 
        // ); // END INFO()

        if (type != TypeName) {
            ERROR("Error: Button constructed with incorrect type: " + type);
        }
        
        InitStruct init;

        // Apply Lua-provided text property to the Button
        if (config["text"].valid()) {
            try {
                text_ = config["text"].get<std::string>();
            } catch (...) {
                // defensive: ignore non-string values
            }
        }       
        // Apply Lua-provided font properties to the Button (accept multiple key variants)
        // Accept font properties; if absent mark as unspecified (-1) to allow
        // BitmapFont defaults to be applied later.
        font_size_ = config["font_size"].valid() ? config["font_size"].get<int>() : -1;
        font_width_ = config["font_width"].valid() ? config["font_width"].get<int>() : -1;
        font_height_ = config["font_height"].valid() ? config["font_height"].get<int>() : -1;

        // font resource name - accept either 'font_resource' or 'font_resource_name'
        if (config["font_resource"].valid()) {
            try { font_resource_name_ = config["font_resource"].get<std::string>(); }
            catch (...) { /* ignore */ }
        } else if (config["font_resource_name"].valid()) {
            try { font_resource_name_ = config["font_resource_name"].get<std::string>(); }
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
        base_index_ = init.base_index; // default to ButtonUp
        
        setTabEnabled(true); // buttons are tab-enabled by default
    }


    // --- Virtual Methods --- //
    bool Button::onInit()
    {
        // create the label object
        if (!labelObject_)
        {
            Label::InitStruct init;
            init.name = getName() + "_label";
            init.type = "Label";
            init.x = getX() + 4;
            init.y = getY() + 2;
            init.width = getWidth() - 8;
            init.height = getHeight() - 4;
            init.alignment = LabelAlign::CENTER;
            init.anchorLeft = AnchorPoint::TOP_LEFT;
            init.anchorTop = AnchorPoint::TOP_LEFT; 
            init.anchorRight = AnchorPoint::BOTTOM_RIGHT;
            init.anchorBottom = AnchorPoint::BOTTOM_RIGHT;
            init.resourceName = font_resource_name_;
            // init.color = label_color_;
            init.foregroundColor = label_color_;
            init.isClickable = false;
            init.tabEnabled = false;
            init.text = text_;
            init.fontSize = font_size_;
            init.fontWidth = font_width_;
            init.fontHeight = font_height_;
            // Apply BitmapFont defaults for missing metrics if the font resource
            // references a BitmapFont. This will preserve any explicit values
            // provided by the user while filling in sensible defaults.
            IFontObject::applyBitmapFontDefaults(getFactory(), init.resourceName,
                                                init.fontSize, init.fontWidth, init.fontHeight);
            labelObject_ = getFactory().create("Label", init);
            addChild(labelObject_);

            // Labels embedded in buttons have a fixed layout defined by the
            // button's box and anchors; disable auto-resize so the label does
            // not continuously recompute layout and rebuild its cache when the
            // parent toggles visual state (hover/press). This keeps the label
            // texture stable and avoids per-frame rebuilds.
            if (IDisplayObject* obj = labelObject_.get())
            {
                if (auto* lbl = dynamic_cast<Label*>(obj))
                {
                    lbl->setAutoResize(false);
                }
            }

            // INFO("Button::onInit() - Label init metrics for '" + init.name + "': resource='" + init.resourceName + "' fontSize=" + std::to_string(init.fontSize) + " fontWidth=" + std::to_string(init.fontWidth) + " fontHeight=" + std::to_string(init.fontHeight));
        }

        return SUPER::onInit();
    } // END: bool Button::onInit()


    void Button::onQuit()
    {
        SUPER::onQuit();
    } // END: void Button::onQuit()


    void Button::onRender()
    {
        SUPER::onRender();

    } // END: void Button::onRender()


    void Button::onUpdate(float fElapsedTime)
    {
    } // END: void Button::onUpdate(float fElapsedTime)


    void Button::onEvent(const Event& event)
    {
        // only target phase
        if (event.getPhase() != Event::Phase::Target) 
        {
            return;
        }

        if (event.getType() == EventType::MouseEnter)
        {
            base_index_ = PanelBaseIndex::ButtonUpSelected;
            // std::cout << "Button " << getName() << " is hovered." << std::endl;
            if (last_base_index_ != base_index_)
            {
                setMouseHovered(true);
                last_base_index_ = base_index_;
                setDirty();
            }
        }
        if (event.getType() == EventType::MouseLeave)
        {
            base_index_ = PanelBaseIndex::ButtonUp;
            // std::cout << "Button " << getName() << " is not hovered." << std::endl;
            if (last_base_index_ != base_index_)
            {
                setMouseHovered(false);
                last_base_index_ = base_index_;
                setDirty();
            }
        }
        if (event.getType() == EventType::MouseButtonDown)
        {
            base_index_ = PanelBaseIndex::ButtonDown;
            // std::cout << "Button " << getName() << " is pressed." << std::endl;
            if (last_base_index_ != base_index_)
            {
                setState(ButtonState::Pressed);
                last_base_index_ = base_index_;
                setDirty();
            }
        }
        if (event.getType() == EventType::MouseButtonUp)
        {
            base_index_ = PanelBaseIndex::ButtonDownSelected;
            // std::cout << "Button " << getName() << " is released." << std::endl;
            if (last_base_index_ != base_index_)
            {
                setState(ButtonState::Released);
                last_base_index_ = base_index_;
                setDirty();
            }
        }
    } // END: void Button::onEvent(const Event& event)

    bool Button::onUnitTest(int frame)
    {
        // Run base class tests first
        if (!SUPER::onUnitTest(frame))
            return false;

        const std::string objName = getName();

        // Only register once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 Scaffold placeholder for future Button-specific tests
            // Example: 
            // 
            // UnitTests& ut = UnitTests::getInstance();
            // ut.add_test(objName, "Button Click State", [this](std::vector<std::string>& errors)
            // {
            //     if (!isEnabled())
            //         errors.push_back("Button '" + getName() + "' should be enabled by default!");
            //     return true;
            // });

            registered = true;
        }

        // ✅ Return true to indicate that this test group has completed
        return true;
    } // END: Button::onUnitTest()


    void Button::setText(const std::string& newText) 
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
    } // END: void Button::setText(const std::string& newText)




    // --- Lua Registration --- //

    void Button::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Include IButtonObject bindings first
        IButtonObject::registerLuaBindings(lua);

        // Include inherited bindings first
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            const std::string typeNameLocal = "Button";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // ---------------------------------------------------------------------
        // 📜 Acquire or create DisplayHandle table
        // ---------------------------------------------------------------------
        sol::table handleTbl = SDOM::IDataObject::ensure_sol_table(lua, SDOM::DisplayHandle::LuaHandleName);

        // Attempt to retrieve the associated usertype, if present
        sol::optional<sol::usertype<DisplayHandle>> maybeUT;
        try { maybeUT = lua[DisplayHandle::LuaHandleName]; } catch(...) {}

        // ---------------------------------------------------------------------
        // 🧩 Button-specific Lua bindings
        // ---------------------------------------------------------------------

        // getText(): string
        dh_bind_both_impl(handleTbl, maybeUT, "getText", [](DisplayHandle& self) -> std::string {
            if (auto* b = self.as<Button>()) return b->getText();
            return {};
        });

        // setText(newText): void
        dh_bind_both_impl(handleTbl, maybeUT, "setText", [](DisplayHandle& self, const std::string& newText) {
            if (auto* b = self.as<Button>()) b->setText(newText);
        });

        // getLabelColor(): table { r,g,b,a }
        dh_bind_both_impl(handleTbl, maybeUT, "getLabelColor",
            [lua](DisplayHandle& self) mutable -> sol::table {
                sol::table t = lua.create_table();
                if (auto* b = self.as<Button>()) {
                    SDL_Color c = b->getLabelColor();
                    t["r"] = c.r; t["g"] = c.g; t["b"] = c.b; t["a"] = c.a;
                    t[1] = (int)c.r; t[2] = (int)c.g; t[3] = (int)c.b; t[4] = (int)c.a;
                }
                return t;
            });

        // setLabelColor(color): accepts table or string "r,g,b,a"
        dh_bind_both_impl(handleTbl, maybeUT, "setLabelColor", [](DisplayHandle& self, const sol::object& color) {
            if (auto* b = self.as<Button>()) {
                SDL_Color c = SDOM::parseColor(color);
                b->setLabelColor(c);
            }
        });

        // getFontResource(): string (read-only)
        dh_bind_both_impl(handleTbl, maybeUT, "getFontResource", [](DisplayHandle& self) -> std::string {
            if (auto* b = self.as<Button>()) return b->getFontResource();
            return {};
        });

        // getLabelObject(): DisplayHandle
        dh_bind_both_impl(handleTbl, maybeUT, "getLabelObject", [](DisplayHandle& self) -> DisplayHandle {
            if (auto* b = self.as<Button>()) return b->getLabelObject();
            return {};
        });

    } // END: void Button::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM
