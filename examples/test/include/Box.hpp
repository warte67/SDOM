// Box.hpp

#pragma once

// #include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <atomic>
// #include <SDOM/SDOM_Event.hpp>


class Box final : public SDOM::IDisplayObject
{
    using SUPER = SDOM::IDisplayObject; 

    friend class SDOM::Factory; // Allow Factory to create IDisplayObjects

    public:

        static constexpr const char* TypeName = "Box";
        struct InitStruct : public IDisplayObject::InitStruct
        {
            InitStruct()
                : IDisplayObject::InitStruct()
            {
                //
                // From IDisplayObject
                //
                name = TypeName;
                type = TypeName;

                // Default color for Box (your original value)
                color = {255, 0, 255, 255};  // magenta box
            }

            // ---------------------------------------------------------------------
            // JSON Loader (Inheritance-Safe)
            // ---------------------------------------------------------------------
            static void from_json(const nlohmann::json& j, InitStruct& out)
            {
                IDisplayObject::InitStruct::from_json(j, out);

                if (j.contains("color"))
                    out.color = SDOM::json_to_color(j["color"]);
            }
        };



    protected:
        Box(const SDOM::IDisplayObject::InitStruct& init);

    public:
        virtual ~Box();

        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject>
        CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit)
        {
            const auto& init = static_cast<const Box::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new Box(init));
        }

        static std::unique_ptr<IDisplayObject>
        CreateFromJson(const nlohmann::json& j)
        {
            Box::InitStruct init;
            Box::InitStruct::from_json(j, init);
            return std::unique_ptr<IDisplayObject>(new Box(init));
        }


        // Override DisplayHandle methods
        virtual bool onInit() override; 
        virtual void onQuit() override;
        virtual void onEvent(const SDOM::Event& event) override;
        virtual void onUpdate(float fElapsedTime) override;
        virtual void onRender() override;
        virtual bool onUnitTest(int frame) override;

    private:
        // Test hook: count clicks received by any Box instance (for unit tests)
        static std::atomic<int> test_click_count_;

    public:
        static void resetTestClickCount() { test_click_count_.store(0); }
        static int getTestClickCount() { return test_click_count_.load(); }

    protected:

        // Lua Registration
        virtual void registerBindingsImpl(const std::string& typeName) override;        
};


