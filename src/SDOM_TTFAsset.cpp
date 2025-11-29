// SDOM_TTFAsset.cpp
#include <SDOM/SDOM.hpp>


#include <SDOM/SDOM_TTFAsset.hpp>
#include <unordered_set>
#include <SDOM/SDOM_PathRegistry.hpp>

namespace
{
    inline std::string resolveFontFilename(const std::string& filename)
    {
        if (filename.empty()) {
            return filename;
        }
        auto& registry = SDOM::PathRegistry::get();
        std::string resolved = registry.resolve(filename, SDOM::PathType::Fonts);
        if (!resolved.empty()) {
            return resolved;
        }
        return filename;
    }
}

namespace SDOM
{

    TTFAsset::TTFAsset(const InitStruct& init) : IAssetObject(init)
    {
        internalFontSize_ = init.internal_font_size;
        name_ = init.name;
        type_ = init.type;
        filename_ = init.filename;

    } // END:  TTFAsset::TTFAsset(const InitStruct& init)

    TTFAsset::~TTFAsset()
    {
        // std::cout << CLR::LT_ORANGE << "BitmapFont::" << CLR::YELLOW << "~BitmapFont()" 
        //           << CLR::LT_ORANGE << " called for: " << CLR::YELLOW << getName() << CLR::RESET << std::endl;
        // Destructor implementation if needed
        // ...
    } // END:  TTFAsset::~TTFAsset()

    bool TTFAsset::onInit() 
    {
        return true;
    } // END: TTFAsset::onInit()

    void TTFAsset::onQuit() 
    {

    } // END: TTFAsset::onQuit()

    void TTFAsset::onLoad() 
    {
        // If already loaded, nothing to do
        if (ttf_font_) return;

        // Load internal default ttf font resource
        if (filename_ == "internal_ttf")
        {
            SDL_IOStream* rw = SDL_IOFromMem(static_cast<void*>(const_cast<unsigned char*>(internal_ttf)), internal_ttf_len);
            if (!rw)
            {
                ERROR("Failed to create SDL_IOStream from internal_ttf[]");
                return;
            }

            ttf_font_ = TTF_OpenFontIO(rw, true, internalFontSize_);
            if (!ttf_font_)
            {
                ERROR("Failed to open TTF font: " + std::string(SDL_GetError()));
                return;
            }

            isLoaded_ = true;   // Mark as created
            isInternal_ = true; // is an internal resource
        }
        else
        // Load TTF font from file
        {
            const std::string resolvedFilename = resolveFontFilename(filename_);
            ttf_font_ = TTF_OpenFont(resolvedFilename.c_str(), internalFontSize_);
            if (!ttf_font_)
            {
                ERROR("Failed to open TTF font: " + resolvedFilename + " - " + std::string(SDL_GetError()));
                return;
            }
            isLoaded_ = true; // Mark as created
            isInternal_ = false; // not an internal resource
        }

    } // END: TTFAsset::onLoad()

    void TTFAsset::onUnload() 
    {
        if (!ttf_font_) {
            isLoaded_ = false;
            return;
        }
        // Guard against accidental double-closes across code paths by tracking
        // which font pointers we've already closed. This protects against
        // re-entrancy and shared-pointer mistakes.
        static std::unordered_set<void*> s_closed_fonts;
        void* raw = static_cast<void*>(ttf_font_);

        if (s_closed_fonts.find(raw) == s_closed_fonts.end())
        {
            s_closed_fonts.insert(raw);
            // If SDL_ttf is already shut down, do not call into it. Just drop the pointer.
            if (TTF_WasInit()) {
                TTF_Font* tmp = ttf_font_;
                ttf_font_ = nullptr; // clear early to prevent re-entrancy
                TTF_CloseFont(tmp);
            } else {
                ttf_font_ = nullptr;
            }
        } else {
            // Already closed elsewhere; just clear our pointer
            ttf_font_ = nullptr;
        }
        isLoaded_ = false;
    } // END: TTFAsset::onUnload()

        
    bool TTFAsset::onUnitTest(int frame)
    {
        (void)frame; // suppress unused parameter warning

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Register once — scaffolding for future TTF tests
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Basic placeholder test
            ut.add_test(objName, "TTFAsset Scaffold", [this](std::vector<std::string>& /*errors*/)
            {
                // No checks yet — placeholder for future font validation
                return true; // ✅ finished immediately
            });

            registered = true;
        }

        // ✅ Always return false so this asset participates in the test cycle
        return false;
    } // END: TTFAsset::onUnitTest()



    
    void TTFAsset::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "TTFAsset");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    


} // END namespace SDOM
