// IRangeObject.hpp
#pragma once

#include <SDOM/SDOM_IDisplayObject.hpp>

namespace SDOM
{
    // --- Enums and Tables --- //
    enum class Orientation { Horizontal, Vertical };
    inline static const std::map<Orientation, std::string> orientation_to_string = {
        { Orientation::Horizontal, "horizontal" },
        { Orientation::Vertical, "vertical" }
    };
    inline static const std::map<std::string, Orientation> string_to_orientation = {
        { "horizontal", Orientation::Horizontal },
        { "vertical", Orientation::Vertical }
    };   



    // --- IRangeObject Interface --- //

    class IRangeObject : public IDisplayObject 
    {
        using SUPER = IDisplayObject;

    public:
        // --- Type Info --- //
        static constexpr const char* TypeName = "IRangeObject";


        // --- Initialization Struct --- //
        struct InitStruct : IDisplayObject::InitStruct
        {
            InitStruct() : IDisplayObject::InitStruct()
            { 
                // from IDisplayObject
                name = TypeName;
                type = TypeName;
                color = {96, 0, 96, 255};   // Icon Color
                tabEnabled = false;
                isClickable = true;
            }
            // Additional IRangeObject Parameters
            float min = 0.0f;      // Minimum value (0.0% - 100.0%)
            float max = 100.0f;    // Maximum value (0.0% - 100.0%)
            float value = 0.0f;    // Current value (0.0% - 100.0%)
            Orientation orientation = Orientation::Horizontal;

        }; // END: InitStruct
        
    protected:
        // --- Protected Constructors --- //
        IRangeObject(const InitStruct& init);  
        IRangeObject(const sol::table& config);

    public:
        // --- Static Factory Methods --- //
        static std::unique_ptr<IDisplayObject> CreateFromLua(const sol::table& config) {
            return std::unique_ptr<IDisplayObject>(new IRangeObject(config));
        }
        static std::unique_ptr<IDisplayObject> CreateFromInitStruct(const IDisplayObject::InitStruct& baseInit) {
            const auto& IRangeObjectInit = static_cast<const IRangeObject::InitStruct&>(baseInit);
            return std::unique_ptr<IDisplayObject>(new IRangeObject(IRangeObjectInit));
        }

        // --- Default Constructor and Destructor --- //
        IRangeObject() = default;
        virtual ~IRangeObject() = default;

        // --- Virtual Methods --- //
        virtual bool onInit() override;     // Called when the display object is initialized
        virtual void onQuit() override;     // Called when the display object is being destroyed
        virtual void onEvent(const Event& event) override;  // Called when an event occurs
        virtual void onUpdate(float fElapsedTime) override; // Called every frame to update the display object
        virtual void onRender() override;   // Called to render the display object
        virtual bool onUnitTest() override; // Unit test method


        // --- Public Accessors --- //
        float getMin() const;
        float getMax() const;
        float getValue() const;
        Orientation getOrientation() const;

        // --- Public Mutators --- //
        void setMin(float v) 
        {
            float clamped = std::max(0.0f, std::min(100.0f, v));
            if (clamped != min_) 
            {
                min_ = clamped;
                // Optionally clamp value_ if needed
                setValue(value_);
                setDirty(true);
            }
        } // END: void setMin(float v)

        void setMax(float v) 
        {
            float clamped = std::max(0.0f, std::min(100.0f, v));
            if (clamped != max_) 
            {
                max_ = clamped;
                // Optionally clamp value_ if needed
                setValue(value_);
                setDirty(true);
            }
        } // END: void setMax(float v)

        void setValue(float v) 
        {
            float clamped = std::max(min_, std::min(max_, v));
            if (clamped != value_) {
                float old = value_;
                value_ = clamped;
                _onValueChanged(old, value_);
                setDirty(true);
            }
        } // END: void setValue(float v)

    protected:
        // --- Protected Data Members --- //
        float min_ = 0.0f;      // Minimum value (0.0% - 100.0%)
        float max_ = 100.0f;    // Maximum value (0.0% - 100.0%)
        float value_ = 0.0f;    // Current value (0.0% - 100.0%)
        Orientation orientation_ = Orientation::Horizontal;

        // --- Protected Virtual Methods --- //
        void setOrientation(Orientation o);

        // --- Protected Virtual Methods --- //
        virtual void _onValueChanged(float oldValue, float newValue);

        // --- Lua Registration --- //
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua);        
    }; // END: class IRangeObject

} // END: namespace SDOM