// SDOM_TriStateCheckbox.hpp
#pragma once

#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>

namespace SDOM
{
    enum class TriState : int
    {
        Unchecked = 0,
        Checked = 1,
        Indeterminate = 2
    };    
    inline static const std::unordered_map<int, std::string> tristate_to_string = {
        { static_cast<int>(TriState::Unchecked), "unchecked" },
        { static_cast<int>(TriState::Checked), "checked" },
        { static_cast<int>(TriState::Indeterminate), "indeterminate" },
    };
    inline static const std::unordered_map<std::string, int> tristate_string_to_index = []{
        std::unordered_map<std::string,int> m;
        for (auto &p : tristate_to_string) m.emplace(p.second, p.first);
        return m;
    }();

    inline static std::optional<std::string> tristate_name_from_index(TriState idx)
    {
        auto it = tristate_to_string.find(static_cast<int>(idx));
        if (it == tristate_to_string.end()) return std::nullopt;
        return it->second;
    };

    inline static std::optional<TriState> tristate_index_from_name(const std::string &name)
    {
        auto it = tristate_string_to_index.find(name);
        if (it == tristate_string_to_index.end()) return std::nullopt;
        return static_cast<TriState>(it->second);
    };


    // --- TriStateCheckbox Class --- //

    class TriStateCheckbox : public IPanelObject    // , IButtonObject
    {
        using SUPER = SDOM::IPanelObject; 

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "TriStateCheckbox";

        // --- Initialization Struct --- //
        struct InitStruct : IPanelObject::InitStruct
        {
            InitStruct() : IPanelObject::InitStruct() 
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // TriStateCheckbox color
                tabEnabled = false;     // TriStateCheckboxes are not tab-enabled by default
                isClickable = true;     // TriStateCheckboxes are clickable by default

                icon_resource = "internal_icon_8x8"; // Default to internal 8x8 sprite sheet
                icon_width = 8;         // default icon width is 8
                icon_height = 8;        // default icon height is 8
                font_resource = "internal_font_8x8"; // Default to internal 8x8 font 
                font_width = 8;         // default font width is 8
                font_height = 8;        // default font height is 8
            }
            // IconIndex icon_index = IconIndex::Checkbox_Empty; 
            std::string text = "TriStateCheckbox"; // default TriStateCheckbox text
            int font_size = 8;          // default font size is 8
            SDL_Color label_color = {255, 255, 255, 255}; // default label color is white
            SDL_Color border_color = {0, 0, 0, 128};               // transparent
            TriState state = TriState::Indeterminate;     // default to Indeterminate
            bool border = false;        // default to no border
        }; // END: InitStruct

    protected:
        // --- Constructors --- //
        TriStateCheckbox(const InitStruct& init);  
        TriStateCheckbox(const sol::table& config);

    public:

        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new TriStateCheckbox(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& TriStateCheckboxInit = static_cast<const TriStateCheckbox::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new TriStateCheckbox(TriStateCheckboxInit));
        }

        TriStateCheckbox() = default;
        virtual ~TriStateCheckbox() = default;

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onRender() override;   // Called to render the display object
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onEvent(const Event& event) override;  // Called when an event occurs

        DisplayHandle getLabelObject() const { return labelObject_; }
        std::string getText() const { return text_; }   
        void setText(const std::string& newText);
        DisplayHandle getIconButtonObject() const { return iconButtonObject_; }

        TriState getState() const;
        void setState(TriState state);


    protected:
        DisplayHandle labelObject_; // internal label object for group text
        std::string text_;     // initialized label text
        std::string font_resource_ = "internal_font_8x8"; // default font resource name
        std::string icon_resource_ = "internal_icon_8x8"; // default icon resource name
        int font_size_ = 8;        // default font size
        int font_width_ = 8;       // default font width
        int font_height_ = 8;      // default font height
        bool use_border_ = false;
        TriState state_  = TriState::Indeterminate; // current state
        SDL_Color label_color_ = {255, 255, 255, 255}; // default label color is white
        SDL_Color border_color_ = {0, 0, 0, 128};               // transparent
        DisplayHandle iconButtonObject_; // internal icon button object for TriStateCheckbox icon
        IconIndex icon_index_ = IconIndex::Checkbox_X;
        int icon_width_ = 8;        // default icon width
        int icon_height_ = 8;       // default icon height
        
        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);

    }; // END: class TriStateCheckbox : public IPanelObject

} // END namespace SDOM