// SDOM_IRangeControl_scaffold.cpp
#include <SDOM/SDOM.hpp>


#include <SDOM/SDOM_IRangeControl_scaffold.hpp>

namespace SDOM
{
    // --- Protected Constructors --- //
    IRangeControl_scaffold::IRangeControl_scaffold(const InitStruct& init) : IRangeControl(init)
    {
    } // END: IRangeControl_scaffold::IRangeControl_scaffold(const InitStruct& init)


    // --- Virtual Methods --- //

    bool IRangeControl_scaffold::onInit()
    {
        // Call the base class onInit first to load the SpriteSheet
        return SUPER::onInit(); 
        // additional initialization as needed
        // ...
    } // END: bool IRangeControl_scaffold::onInit()

    // Example device lifecycle usage in scaffold: defer to base behavior so
    // composed assets (SpriteSheet) are loaded and cached textures are dropped
    // safely around device rebuilds. Derived controls can extend if needed.
    bool IRangeControl_scaffold::onLoad()
    {
        return SUPER::onLoad();
    }

    void IRangeControl_scaffold::onUnload()
    {
        SUPER::onUnload();
    }

    void IRangeControl_scaffold::onQuit()
    {
        SUPER::onQuit();
    } // END: void IRangeControl_scaffold::onQuit()

    void IRangeControl_scaffold::onEvent(const Event& event)
    {
        SUPER::onEvent(event);
    } // END: void IRangeControl_scaffold::onEvent(const Event& event)

    void IRangeControl_scaffold::onUpdate(float fElapsedTime)
    {
        SUPER::onUpdate(fElapsedTime);
    } // END: void IRangeControl_scaffold::onUpdate(float fElapsedTime)

    void IRangeControl_scaffold::onRender()
    {
        SUPER::onRender();
    } // END: void IRangeControl_scaffold::onRender()

    bool IRangeControl_scaffold::onUnitTest(int frame)
    {
        // Run base class unit tests first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Register test lambdas only once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 1. Basic scaffold integrity check (placeholder)
            ut.add_test(objName, "IRangeControl_scaffold Basic Integrity", [this](std::vector<std::string>& errors)
            {
                // For now, we don’t have dedicated range validation logic,
                // but we can assert that the object is initialized correctly.
                if (getMin() > getMax())
                    errors.push_back("IRangeControl_scaffold '" + getName() + "' has min > max.");
                return true; // ✅ single-frame test
            });

            // 🔹 2. Placeholder for derived control interaction checks
            ut.add_test(objName, "IRangeControl_scaffold Derived Behavior", [this](std::vector<std::string>& errors)
            {
                // Future test placeholder for subclasses (e.g., sliders, scrollbars)
                (void)errors; // suppress unused warning
                return true; // ✅ placeholder test passes
            });

            registered = true;
        }

        // ✅ Return false so this unit test remains active for consistency
        return false;
    } // END: IRangeControl_scaffold::onUnitTest()


    // Cached-texture resilience on device changes
    // See header for the detailed contract. This scaffold provides a safe
    // default that clears cached renderer-owned resources and marks the
    // control dirty so derived classes render correctly after SDL rebuilds.
    void IRangeControl_scaffold::onWindowResize(int /*logicalWidth*/, int /*logicalHeight*/)
    {
        // If this scaffold or its derivatives cache textures, invalidate them here.
        // (The base IRangeControl provides members commonly used by range controls.)
        // Do not call SDL_DestroyTexture during renderer transitions; drop pointer.
        if (cachedTexture_) { cachedTexture_ = nullptr; }
        current_width_ = 0;
        current_height_ = 0;
        current_pixel_format_ = SDL_PIXELFORMAT_UNKNOWN;
        cached_renderer_ = nullptr;
        setDirty(true);
    }




    // --- Public Accessors --- //




    // --- Public Mutators --- //



    
    // --- Protected Virtual Methods --- //        

    void IRangeControl_scaffold::_onValueChanged(float oldValue, float newValue)
    {
        SUPER::_onValueChanged(oldValue, newValue);
        // dispatch event
        queue_event(EventType::ValueChanged, [this, oldValue, newValue](Event& ev) {
            ev.setPayloadValue("name", getName());
            ev.setPayloadValue("old_value", oldValue);
            ev.setPayloadValue("new_value", newValue);
        });
    } // END: void IRangeControl_scaffold::_onValueChanged(float oldValue, float newValue)



    // -----------------------------------------------------------------
    // 📜 Data Registry Integration
    // -----------------------------------------------------------------
    
    void IRangeControl_scaffold::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "IRangeControl_scaffold");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    

} // END: namespace SDOM
