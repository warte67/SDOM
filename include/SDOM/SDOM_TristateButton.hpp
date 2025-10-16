// SDOM_TristateButton.hpp
#pragma once

#include <string>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_IconIndex.hpp>
#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_Label.hpp>

namespace SDOM
{

class TristateButton : public IDisplayObject, public IButtonObject
{
    using SUPER = SDOM::IDisplayObject; 

public:
    // --- Type Info --- //
    static constexpr const char* TypeName = "TristateButton";

    // --- Initialization Struct --- //
    struct InitStruct : IDisplayObject::InitStruct
    {
        InitStruct() : IDisplayObject::InitStruct() 
        { 
            // from IDisplayObject
            name = TypeName;
            type = TypeName;
            color = {96, 0, 96, 255};   // Icon Color
            tabEnabled = false;
            isClickable = true;
        }
        // TristateButton Initialization Properties
        std::string text = "TriStateCheckbox"; // default TriStateCheckbox text
        int font_size = 8;          // default font size is 8
        SDL_Color label_color = {0, 255, 255, 255}; // default label color is white
        SDL_Color border_color = {0, 0, 0, 128};               // transparent
        ButtonState state = ButtonState::Unchecked;
        std::string icon_resource = "internal_icon_8x8"; // Default to internal 8x8 sprite sheet
        // IconIndex icon_index = IconIndex::Checkbox_Empty;
        int icon_width = 8;
        int icon_height = 8;
        std::string font_resource = "internal_font_8x8"; 
        int font_width = 8;
        int font_height = 8;
        bool border = false;        // default to no border
    }; // END: InitStruct


protected:
    // --- Constructors --- //
    TristateButton(const InitStruct& init);  
    TristateButton(const sol::table& config);

public:
    // --- Static Factory Methods --- //
    static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
        return std::unique_ptr<IDisplayObject>(new TristateButton(config));
    }
    static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
        const auto& TristateButtonInit = static_cast<const TristateButton::InitStruct&>(baseInit);
        return std::unique_ptr<IDisplayObject>(new TristateButton(TristateButtonInit));
    }

    // --- Default Constructor and Virtual Destructor --- //
    TristateButton() = default;
    virtual ~TristateButton() {};

    // --- Lifecycle & Core Virtuals --- //
    virtual bool onInit() override;
    virtual void onQuit() override;
    virtual void onUpdate(float fElapsedTime) override;
    virtual void onEvent(const Event& event) override;
    virtual void onRender() override;
    virtual bool onUnitTest() override { return true; }

    // --- Virtual State Accessors (From IButtonObject) --- //
    virtual ButtonState getState() const;
    virtual void setState(ButtonState state);

    // --- Public Accessors --- //
    DisplayHandle getLabelObject() const { return labelObject_; }
    std::string getText() const { return text_; }   
    DisplayHandle getIconButtonObject() const { return iconButtonObject_; }
    IconButton* getIconButton() const;
    SpriteSheet* getIconSpriteSheet() const;
    Label* getLabel() const;
    std::string getFontResource() const { return font_resource_; }
    std::string getIconResource() const { return icon_resource_; }
    int getFontSize() const { return font_size_; }
    int getFontWidth() const { return font_width_; }
    int getFontHeight() const { return font_height_; }
    bool getUseBorder() const { return use_border_; }
    SDL_Color getLabelColor() const { return label_color_; }
    SDL_Color getBorderColor() const { return border_color_; }
    int getIconWidth() const { return icon_width_; }
    int getIconHeight() const { return icon_height_; }
    IconIndex getIconIndex() const { return icon_index_; }

    // --- Public Mutators --- //
    void setText(const std::string& newText);
    // ...  (not sure yet which setters we want to include; which properties are read-only)

protected:
    // --- Protected Virtual Methods (From IButtonObject) --- //
    virtual void onStateChanged(ButtonState oldState, ButtonState newState) override;
    virtual IconIndex iconIndexForState(ButtonState state) const override;

    // --- Data Members --- //
    DisplayHandle labelObject_; // internal label object for group text
    std::string text_;     // initialized label text
    std::string font_resource_ = "internal_font_8x8"; // default font resource name
    std::string icon_resource_ = "internal_icon_8x8"; // default icon resource name
    int font_size_ = 8;        // default font size
    int font_width_ = 8;       // default font width
    int font_height_ = 8;      // default font height
    bool use_border_ = false;
    // ButtonState state_  = ButtonState::Unchecked; // current state
    SDL_Color label_color_ = {255, 255, 255, 255}; // default label color is white
    SDL_Color border_color_ = {0, 0, 0, 128};               // transparent
    DisplayHandle iconButtonObject_; // internal icon button object for TriStateCheckbox icon
    IconIndex icon_index_ = IconIndex::Checkbox_Empty;
    int icon_width_ = 8;        // default icon width
    int icon_height_ = 8;       // default icon height    
    
    // --- Lua Registration --- //
    virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);
};

} // namespace SDOM