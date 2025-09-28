// SDOM_Stage.hpp

#pragma once

#include <SDOM/SDOM_IDisplayObject.hpp>

namespace SDOM
{
    class Factory; // Forward declaration

    class Stage : public IDisplayObject
    {
        using SUPER = IDisplayObject; // Note the Super Class for Future Reference

        friend class Factory; // Allow Factory to create Stage Objects
        friend class Core;    // Allow Core to access Factory internals if needed

    public:
        static constexpr const char* TypeName = "Stage";
        // --- InitStruct (inherits from IDisplayObject::InitStruct)
        struct InitStruct : public IDisplayObject::InitStruct
        {
            InitStruct() : IDisplayObject::InitStruct() 
            { 
                name = TypeName; 
                type = TypeName;
                color = {0, 0, 0, 255}; 
            }
        };

    protected:   // protected constructor so only the Factory can create the object
        Stage(const InitStruct& init);  
        Stage(const sol::table& config);

    public:

        // --- Static Creators for this Type --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new Stage(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& stageInit = static_cast<const Stage::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Stage(stageInit));
        }

        // --- Virtual Destructor --- // 
        virtual ~Stage() = default;

        // --- Virtual Methods from IDataObject --- //
        virtual bool onInit() override;             
        virtual void onQuit() override;      

        // --- Virtual Methods from IDisplayObject --- //       
        virtual void onUpdate(float fElapsedTime); 
        virtual void onEvent(const Event& event); 
        virtual void onRender(); 
        virtual bool onUnitTest(); 

    public:
        inline static int mouseX = 0;   // current mouse horizontal coordinate within this stage
        inline static int mouseY = 0;   // current mouse vertical coordinate within this stage 

        // --- LUA Registration --- //
    protected:
        virtual void _registerLua_Usertype(sol::state_view lua)      { SUPER::_registerLua_Usertype(lua); }  
        virtual void _registerLua_Properties(sol::state_view lua)    { SUPER::_registerLua_Properties(lua); }
        virtual void _registerLua_Commands(sol::state_view lua)      { SUPER::_registerLua_Commands(lua); }
        virtual void _registerLua_Meta(sol::state_view lua)          { SUPER::_registerLua_Meta(lua); }
        virtual void _registerLua_Children(sol::state_view lua)      { SUPER::_registerLua_Children(lua); }
        virtual void _registerLua_All(sol::state_view lua) override       
        {
            SUPER::_registerLua_All(lua);
            _registerLua_Usertype(lua);
            _registerLua_Properties(lua);
            _registerLua_Commands(lua);
            _registerLua_Meta(lua);
            _registerLua_Children(lua);

            std::cout << "Stage: Registered Lua bindings." << std::endl;
        }

    };

} // namespace SDOM