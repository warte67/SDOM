#pragma once

#include <SDOM/SDOM_IDisplayObject.hpp>
#include <external/nlohmann/json.hpp>

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
            InitStruct()
                : IDisplayObject::InitStruct()
            {
                name  = TypeName;
                type  = TypeName;
                color = { 0, 0, 0, 255 };
            }

            // --- JSON Loader ---
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                // 1) Populate all inherited fields first
                IDisplayObject::InitStruct::from_json(j, out);

                // 2) Load Stage-specific overrides
                if (j.contains("color"))
                    out.color = json_to_color(j["color"]);
            }
        };


    protected:
        // --- Constructors --- //

        // ✔ JSON-compatible constructor (InitStruct)
        Stage(const InitStruct& init);

        // ⚠ Legacy Lua constructors — DEPRECATED and to be removed soon
        Stage(const sol::table& config);
        Stage(const sol::table& config, const InitStruct& defaults);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject>
        CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const Stage::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Stage(init));
        }

        static std::unique_ptr<IDisplayObject>
        CreateFromJson(const nlohmann::json& j)
        {
            Stage::InitStruct init;
            Stage::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new Stage(init));
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

        inline static int mouseX = 0;
        inline static int mouseY = 0;

        // --- Lua Wrappers (will be removed) --- //
        int getMouseX_lua() const { return getMouseX(); }
        int getMouseY_lua() const { return getMouseY(); }
        void setMouseX_lua(int x) { setMouseX(x); }
        void setMouseY_lua(int y) { setMouseY(y); }

    protected:

        // ✔ DataRegistry → CAPI Bindings stay
        void registerBindingsImpl(const std::string& typeName) override;
    };

} // namespace SDOM
