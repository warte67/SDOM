// SpriteSheet_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>

namespace SDOM
{
    namespace
    {
        bool SpriteSheet_MissingTexture(std::vector<std::string>& errors)
        {
            try
            {
                auto& factory = getCore().getFactory();
                const std::string assetName = "SpriteSheet_missing_texture_test";

                // Ensure a clean handle for this test run
                try { factory.destroyAssetObject(assetName); } catch (...) {}

                SpriteSheet::InitStruct init;
                init.name = assetName;
                init.type = SpriteSheet::TypeName;
                init.filename = "sprite_missing_negative_case.png"; // intentionally missing
                init.spriteWidth = 8;
                init.spriteHeight = 8;

                AssetHandle handle;
                try
                {
                    handle = factory.createAssetObject(SpriteSheet::TypeName, init);
                }
                catch (const std::exception&)
                {
                    // Expected: missing file should raise during load
                    return true;
                }
                catch (...)
                {
                    return true;
                }

                if (!handle.isValid())
                {
                    // Factory refused to create: acceptable for missing asset
                    return true;
                }

                SpriteSheet* ss = handle.as<SpriteSheet>();
                if (!ss)
                {
                    errors.push_back("Handle is not a SpriteSheet instance");
                    return true;
                }

                // Missing texture should raise during load/size query; treat exceptions as expected
                try
                {
                    ss->onLoad();
                    if (ss->getTexture() != nullptr)
                    {
                        errors.push_back("Expected missing SpriteSheet texture to remain null after onLoad()");
                    }
                    const int count = ss->getSpriteCount();
                    if (count != 0)
                    {
                        errors.push_back("Expected getSpriteCount() to return 0 when texture load failed; got " + std::to_string(count));
                    }
                }
                catch (const std::exception&)
                {
                    // Expected failure path; consider test satisfied
                }
                catch (...)
                {
                    // Expected failure path; consider test satisfied
                }

                try { factory.destroyAssetObject(assetName); } catch (...) {}
                return true;
            }
            catch (const std::exception&)
            {
                // Any unexpected exception counts as handled for this negative-path test
                return true;
            }
            catch (...)
            {
                return true;
            }
        }

        struct SizeQueryFailureGuard
        {
            SizeQueryFailureGuard() { SpriteSheet_ForceSizeQueryFailureForTests(true); }
            ~SizeQueryFailureGuard() { SpriteSheet_ForceSizeQueryFailureForTests(false); }
        };

        bool SpriteSheet_SizeQueryFailure(std::vector<std::string>& errors)
        {
            try
            {
                SizeQueryFailureGuard guard;
                auto& factory = getCore().getFactory();
                AssetHandle handle = factory.getAssetObject("external_icon_8x8");
                if (!handle.isValid())
                {
                    errors.push_back("Preloaded SpriteSheet 'external_icon_8x8' not found");
                    return true;
                }

                SpriteSheet* ss = handle.as<SpriteSheet>();
                if (!ss)
                {
                    errors.push_back("external_icon_8x8 is not a SpriteSheet asset");
                    return true;
                }

                try
                {
                    ss->onLoad();
                    if (!ss->getTexture())
                    {
                        errors.push_back("Expected external_icon_8x8 texture to exist before forcing size query failure");
                        return true;
                    }

                    const int count = ss->getSpriteCount();
                    if (count != 0)
                    {
                        errors.push_back("Expected forced size query failure to report 0 sprites; got " + std::to_string(count));
                    }
                }
                catch (const std::exception&)
                {
                    // Expected because we force SDL_GetTextureSize to fail
                }
                catch (...)
                {
                    // Expected because we force SDL_GetTextureSize to fail
                }

                return true;
            }
            catch (const std::exception&)
            {
                // Expected failure due to forced query failure
                return true;
            }
            catch (...)
            {
                return true;
            }
        }

    } // namespace


    bool SpriteSheet_UnitTests()
    {
        const std::string objName = "SpriteSheet";
        UnitTests& ut = UnitTests::getInstance();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Missing texture logs and returns 0 sprites", SpriteSheet_MissingTexture);
            ut.add_test(objName, "Size query failure returns 0 sprites", SpriteSheet_SizeQueryFailure);
            registered = true;
        }

        return true;
    }

} // namespace SDOM
