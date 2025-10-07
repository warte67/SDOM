// SDOM_SpriteSheet.hpp
#pragma once

#include <SDOM/SDOM_IAssetObject.hpp>

namespace SDOM
{

    class SpriteSheet : public IAssetObject
    {
    public:
        SpriteSheet() = default;
        virtual ~SpriteSheet() = default;

        virtual bool onInit() override;
        virtual void onQuit() override;
        virtual void onLoad() override;
        virtual void onUnload() override;
        virtual bool onUnitTest() override;

        // Additional sprite sheet specific methods will be added here
        // ...

    }; // END class SpriteSheet

} // namespace SDOM