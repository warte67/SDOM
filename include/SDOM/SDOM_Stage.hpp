#pragma once

#include <SDOM/SDOM_IDisplayObject.hpp>

namespace SDOM
{

    class Factory; // Forward declaration

    class Stage : public IDisplayObject
    {
        // --- Friend Classes --- //
        friend class Factory; // Allow Factory to create Stage Objects
        friend class Core;    // Allow Core to access Factory internals if needed

        using SUPER = IDisplayObject;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "Stage";

        // --- Initialization Struct --- //
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
        // --- Constructors --- //
        Stage(const InitStruct& init);  
        Stage(const sol::table& config);
        // Defaults-aware Lua constructor so derived defaults can be forwarded
        Stage(const sol::table& config, const InitStruct& defaults);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new Stage(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& stageInit = static_cast<const Stage::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Stage(stageInit));
        }

        // --- Destructor --- // 
        ~Stage() override = default;

        // --- Lifecycle Methods (IDataObject) --- //
        bool onInit() override;             
        void onQuit() override;      

        // --- Core Display Methods (IDisplayObject) --- //       
        void onUpdate(float fElapsedTime) override; 
        void onEvent(const Event& event) override; 
        void onRender() override; 
        bool onUnitTest(int frame) override; 

        // --- Stage-Specific State --- //
        static int getMouseX() { return mouseX; }
        static void setMouseX(int x) { mouseX = x; }
        static int getMouseY() { return mouseY; }
        static void setMouseY(int y) { mouseY = y; }
        inline static int mouseX = 0;   // current mouse horizontal coordinate within this stage
        inline static int mouseY = 0;   // current mouse vertical coordinate within this stage 

        // --- Lua Wrappers --- //
        int getMouseX_lua() const { return getMouseX(); }
        int getMouseY_lua() const { return getMouseY(); }
        void setMouseX_lua(int x) { setMouseX(x); }
        void setMouseY_lua(int y) { setMouseY(y); }

    protected:
        // --- Lua Registration --- //        
        sol::usertype<Stage> objHandleType_;  
        

        // ---------------------------------------------------------------------
        // 🔗 Legacy Lua Registration
        // ---------------------------------------------------------------------
        void _registerLuaBindings(const std::string& typeName, sol::state_view lua) override;


        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         
    };

} // namespace SDOM