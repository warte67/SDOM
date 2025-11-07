// SDOM_IFontObject.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>

namespace SDOM
{
    IFontObject::IFontObject(const InitStruct& init) : IAssetObject(init)
    {
        // requested type default -> "bitmap"
        std::string requested = init.type.empty() ? std::string("bitmap") : init.type;
        std::transform(requested.begin(), requested.end(), requested.begin(), [](unsigned char c){ return std::tolower(c); });

        auto it = StringToFontType.find(requested);
        if (it != StringToFontType.end())
        {
            fontType_ = it->second; // Set enum based on string
        }
        else
        {
            DEBUG_LOG("IFontObject constructed with unknown font type: " + requested + " — defaulting to 'bitmap'");
            fontType_ = FontType::Bitmap;
        }
        type_ = FontTypeToString.at(fontType_);
        fontSize_ = (init.fontSize_ > 0) ? init.fontSize_ : 8; // sane default



        
    }


    IFontObject::IFontObject(const sol::table& config) : IAssetObject(config)
    {
        // name (required)
        if (!config["name"].valid()) {
            ERROR("IFontObject constructed without required field 'name' in config");
        }
        name_ = config["name"].get<std::string>();

        // filename (required)
        if (!config["filename"].valid()) {
            ERROR("IFontObject constructed without required field 'filename' in config");
        }
        filename_ = config["filename"].get<std::string>();

        // type (required) -> normalize and map to FontType
        std::string requested = config["type"].valid() ? config["type"].get<std::string>() : std::string();
        if (requested.empty()) {
            DEBUG_LOG("IFontObject constructed without required field 'type' in config");
            requested = "bitmap";
        }
        std::transform(requested.begin(), requested.end(), requested.begin(), [](unsigned char c){ return std::tolower(c); });

        auto it = StringToFontType.find(requested);
        if (it != StringToFontType.end()) {
            fontType_ = it->second;
        } else {
            DEBUG_LOG("IFontObject constructed with unknown font type: " + requested);
            fontType_ = FontType::Bitmap;
        }
        type_ = FontTypeToString.at(fontType_);

        // optional size
        fontSize_ = config["size"].valid() ? config["size"].get<int>() : 8;
        if (fontSize_ <= 0) fontSize_ = 8;
    }


    IFontObject::~IFontObject()
    {
        // Destructor implementation if needed
        // ...
    }   

    int IFontObject::getWordWidth(const std::string& word) const
    {
        // Default implementation (can be overridden)
        int ret = 0;
        for (char c : word)
        {
            ret += getGlyphWidth(static_cast<Uint32>(c));
        }
        return ret;
    }

    int IFontObject::getWordHeight(const std::string& word) const
    {
        // Default implementation (can be overridden)
        if (word.empty()) return 0; // Handle empty word case
        return std::max(0, getGlyphHeight(static_cast<Uint32>(word[0])));
    }

    // --- Helper implementation ---
    void IFontObject::applyBitmapFontDefaults(Factory& factory,
                                              const std::string& fontResourceName,
                                              int &outFontSize,
                                              int &outFontWidth,
                                              int &outFontHeight)
    {
        if (fontResourceName.empty()) return;

        // If the out values are already set to sensible positives, do not
        // overwrite them. We'll only fill values that are non-positive (<=0)
        // to preserve explicit user-provided metrics.
        bool needSize = (outFontSize <= 0);
        bool needW = (outFontWidth <= 0);
        bool needH = (outFontHeight <= 0);
        if (!needSize && !needW && !needH) return; // nothing to do

        AssetHandle asset = factory.getAssetObject(fontResourceName);
        if (!asset.isValid()) return;

        // Try dynamic cast to BitmapFont
        IFontObject* ifont = asset.as<IFontObject>();
        if (!ifont) return;

        // Only bitmap fonts expose sprite metrics we can use
        BitmapFont* bmp = dynamic_cast<BitmapFont*>(ifont);
        if (!bmp) return;

        int bmpW = bmp->getBitmapFontWidth();
        int bmpH = bmp->getBitmapFontHeight();
        // BitmapFont may also store a nominal font size; prefer that for fontSize
        FontStyle fs = bmp->getFontStyle();

        if (needW && bmpW > 0) outFontWidth = bmpW;
        if (needH && bmpH > 0) outFontHeight = bmpH;
        if (needSize) {
            if (fs.fontSize > 0) outFontSize = fs.fontSize;
            else if (outFontHeight > 0) outFontSize = outFontHeight;
            else if (bmpH > 0) outFontSize = bmpH;
        }
    }


    // --- Lua Regisstration --- //

    void IFontObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IFontObject:" + getName();
            std::cout << CLR::MAGENTA << "Registered " << CLR::LT_MAGENTA << typeNameLocal 
                    << CLR::MAGENTA << " Lua bindings for type: " << CLR::LT_MAGENTA 
                    << typeName << CLR::RESET << std::endl;
        }

        // // Augment the single shared AssetHandle handle usertype (assets are exposed via AssetHandle handles in Lua)
        // Go-by for future bindings on AssetHandle:
        //   auto ut = SDOM::IDataObject::register_usertype_with_table<AssetHandle, SDOM::IDataObject>(lua, AssetHandle::LuaHandleName);
        //   sol::table handle = SDOM::IDataObject::ensure_sol_table(lua, AssetHandle::LuaHandleName);
    } // END _registerLuaBindings()

} // namespace SDOM
