// Box.hpp

#pragma once

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
// #include <SDOM/SDOM_Event.hpp>


class Box final : public SDOM::IDisplayObject
{
    using SUPER = SDOM::IDisplayObject; 

    friend class SDOM::Factory; // Allow Factory to create IDisplayObjects

    public:

        static constexpr const char* TypeName = "Box";
        struct InitStruct : public IDisplayObject::InitStruct
        {
            InitStruct() : IDisplayObject::InitStruct() 
            { 
                name = TypeName; 
                type = TypeName; 
                color = {0, 0, 0, 255}; 
            }
        };

    protected:
        Box(const SDOM::IDisplayObject::InitStruct& init);
        Box(const sol::table& config);

    public:
        virtual ~Box();

        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new Box(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& boxInit = static_cast<const Box::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Box(boxInit));
        }

        // Override DisplayObject methods
        virtual bool onInit() override; 
        virtual void onQuit() override;
        virtual void onEvent(const SDOM::Event& event) override;
        virtual void onUpdate(float fElapsedTime) override;
        virtual void onRender() override;
        virtual bool onUnitTest() override;

    private:
        float gray = 0.0f;  // used for keyfocus indication

    protected:
        // --- LUA Registration --- //
        virtual void _registerLua_Usertype(sol::state_view lua);
        
};


