// IPanelObject.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <SDOM/SDOM_IPanelObject.hpp>

namespace SDOM
{
    IPanelObject::IPanelObject(const InitStruct& init) : IDisplayObject(init)
    {
        if (init.type != TypeName) {
            ERROR("IPanelObject: Stage constructed with incorrect type: " + init.type);
        }        

        icon_resource_name_ = init.icon_resource_name;
        font_resource_name_ = init.font_resource_name;
        icon_width_ = init.icon_width;
        icon_height_ = init.icon_height;
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        base_index_ = init.base_index;
    } // END: IPanelObject::IPanelObject(const InitStruct& init)


    IPanelObject::IPanelObject(const sol::table& config) : IDisplayObject(config)
    {
        std::string type = config["type"].valid() ? config["type"].get<std::string>() : TypeName;
        if (type != TypeName) {
            ERROR("IPanelObject: Stage constructed with incorrect type: " + type);
        }
        // --- Initialization Lambdas --- //
        auto get_str = [&](const char* k, const std::string& d = "") -> std::string 
        {
            return config[k].valid() ? config[k].get<std::string>() : d;
        };
        auto get_int = [&](const char* k, int d = 0) -> int 
        {
            return config[k].valid() ? config[k].get<int>() : d;
        };
        InitStruct init;
        init.icon_resource_name = get_str("icon_resource_name_", init.icon_resource_name);
        init.font_resource_name = get_str("font_resource_name_", init.font_resource_name);
        init.icon_width = get_int("icon_width_", init.icon_width);
        init.icon_height = get_int("icon_height_", init.icon_height);
        init.font_width = get_int("font_width_", init.font_width);
        init.font_height = get_int("font_height_", init.font_height);

        // Convert base_index from string if present
        std::string base_index_str = get_str("base_index_", "frame");
        auto it = stringToPanelBaseIndex_.find(base_index_str);
        init.base_index = (it != stringToPanelBaseIndex_.end()) ? it->second : PanelBaseIndex::Frame;

        // Assign to members
        icon_resource_name_ = init.icon_resource_name;
        font_resource_name_ = init.font_resource_name;
        icon_width_ = init.icon_width;
        icon_height_ = init.icon_height;
        font_width_ = init.font_width;
        font_height_ = init.font_height;
        base_index_ = init.base_index;
    } // END: IPanelObject::IPanelObject(const sol::table& config)


    // --- Virtual Methods --- //
    bool IPanelObject::onInit() 
    {
        // Load sprite sheet asset
        if (!spriteSheetAsset_) 
        {
            spriteSheetAsset_ = getFactory().getAssetObject(icon_resource_name_);
            if (!spriteSheetAsset_) 
            {
                ERROR("IPanelObject::onInit: Failed to load sprite sheet asset: " + icon_resource_name_);
                return false;
            }
        }

        // Load font asset
        if (fontAsset_ == nullptr && !font_resource_name_.empty())
        if (!font_resource_name_.empty())
        {
            fontAsset_ = getFactory().getAssetObject(font_resource_name_);
            if (!fontAsset_) 
            {
                ERROR("IPanelObject::onInit: Failed to load font asset: " + font_resource_name_);
                return false;
            }
        }

        return true;
    } // END: bool IPanelObject::onInit() 


    void IPanelObject::onQuit() 
    {
        // We dont want to reset the assets here.  Let the factory do it
        // ...
    } // END: void IPanelObject::onQuit() 

    

    // void IPanelObject::onUpdate(float fElapsedTime)=0;
    // void IPanelObject::onEvent(const Event& event)=0;



    void IPanelObject::onRender() 
    {
        renderPanel();
    } // END: void IPanelObject::onRender() 


    // --- Helper Methods --- // 
    void IPanelObject::renderPanel()
    {
        if (!spriteSheetAsset_) return;

        int x = getX(), y = getY(), w = getWidth(), h = getHeight();
        int cw = icon_width_, ch = icon_height_;

        int leftW   = std::min(cw, w / 2);
        int rightW  = std::min(cw, w - leftW);
        int topH    = std::min(ch, h / 2);
        int bottomH = std::min(ch, h - topH);

        int centerW = std::max(0, w - leftW - rightW);
        int centerH = std::max(0, h - topH - bottomH);

        float fx = static_cast<float>(x), fy = static_cast<float>(y);
        float fleftW = static_cast<float>(leftW), frightW = static_cast<float>(rightW);
        float ftopH = static_cast<float>(topH), fbottomH = static_cast<float>(bottomH);
        float fcenterW = static_cast<float>(centerW), fcenterH = static_cast<float>(centerH);
        float fcw = static_cast<float>(cw), fch = static_cast<float>(ch);

        int base = static_cast<int>(base_index_);
        auto idx = [base](PanelTileOffset pto) { return base + static_cast<int>(pto); };
        SDL_Color color = getColor();

        // SDL_Color color = getColor();
        // SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        using PTO = PanelTileOffset;

        SpriteSheet* sprite_sheet = spriteSheetAsset_->as<SpriteSheet>();
        if (!sprite_sheet)
        {
            ERROR("IPanelObject::renderPanel: sprite asset is not a SpriteSheet");
            return;
        }

        // Top row
        sprite_sheet->drawSprite(idx(PTO::TopLeft), {0, 0, fleftW, ftopH}, {fx, fy, fleftW, ftopH}, color);
        if (centerW > 0)
            sprite_sheet->drawSprite(idx(PTO::TopCenter), {0, 0, fcw, ftopH}, {fx + fleftW, fy, fcenterW, ftopH}, color);
        sprite_sheet->drawSprite(idx(PTO::TopRight), {fcw - frightW, 0, frightW, ftopH}, {fx + fleftW + fcenterW, fy, frightW, ftopH}, color);

        // Middle row
        if (centerH > 0)
            sprite_sheet->drawSprite(idx(PTO::LeftCenter), {0, 0, fleftW, fch}, {fx, fy + ftopH, fleftW, fcenterH}, color);
        if (centerW > 0 && centerH > 0)
            sprite_sheet->drawSprite(idx(PTO::Center), {0, 0, fcw, fch}, {fx + fleftW, fy + ftopH, fcenterW, fcenterH}, color);
        if (centerH > 0)
            sprite_sheet->drawSprite(idx(PTO::RightCenter), {fcw - frightW, 0, frightW, fch}, {fx + fleftW + fcenterW, fy + ftopH, frightW, fcenterH}, color);

        // Bottom row
        sprite_sheet->drawSprite(idx(PTO::BottomLeft), {0, fch - fbottomH, fleftW, fbottomH}, {fx, fy + ftopH + fcenterH, fleftW, fbottomH}, color);
        if (centerW > 0)
            sprite_sheet->drawSprite(idx(PTO::BottomCenter), {0, fch - fbottomH, fcw, fbottomH}, {fx + fleftW, fy + ftopH + fcenterH, fcenterW, fbottomH}, color);
        sprite_sheet->drawSprite(idx(PTO::BottomRight), {fcw - frightW, fch - fbottomH, frightW, fbottomH}, {fx + fleftW + fcenterW, fy + ftopH + fcenterH, frightW, fbottomH}, color);
    } // END: void IPanelObject::renderPanel()


    // --- Lua Registration --- //
    void IPanelObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IPanelObject";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN 
                    << typeName << CLR::RESET << std::endl;
        }

        // // Augment the single shared DisplayHandle handle usertype
        // sol::table handle = DisplayHandle::ensure_handle_table(lua);

        // auto absent = [&](const char* name) -> bool {
        //     sol::object cur = handle.raw_get_or(name, sol::lua_nil);
        //     return !cur.valid() || cur == sol::lua_nil;
        // };

        // --- these will need to be bound to the handle sol::table --- //
        // AssetHandle spriteSheet_;
        // AssetHandle fontObject_; 
        // std::string icon_resource_name_ = "internal_icon_8x8"; 
        // std::string font_resource_name_ = "internal_font_8x8"; 
        // int icon_width_ = 8; 
        // int icon_height_ = 8; 
        // int font_width_ = 8;
        // int font_height_ = 8;
        // PanelBaseIndex base_index_ = PanelBaseIndex::Frame;


    } // END: void IPanelObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END namespace SDOM