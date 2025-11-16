// SDOM_Frame.cpp

#include <SDOM/SDOM.hpp>

#include <SDOM/SDOM_Frame.hpp>



namespace SDOM
{

    Frame::Frame(const InitStruct& init) : IPanelObject(init)
    {
        // std::cout << "Box constructed with InitStruct: " << getName() 
        //           << " at address: " << this << std::endl;

        if (init.type != TypeName) {
            ERROR("Error: Frame constructed with incorrect type: " + init.type);
        }
    }



    // --- Virtual Methods --- //
    bool Frame::onInit()
    {
        return SUPER::onInit();
    } // END: bool Frame::onInit()


    void Frame::onQuit()
    {
        SUPER::onQuit();
    } // END: void Frame::onQuit()


    void Frame::onRender()
    {
        SUPER::onRender();
    } // END: void Frame::onRender()


    void Frame::onUpdate(float fElapsedTime)
    {
        (void) fElapsedTime; // suppress unused parameter warning

    } // END: void Frame::onUpdate(float fElapsedTime)


    void Frame::onEvent(const Event& event)
    {
        (void)event;    // suppress unused parameter warning

    } // END: void Frame::onEvent(const Event& event)
    

    bool Frame::onUnitTest(int frame)
    {
        // Run base class tests first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Only register once
        static bool registered = false;
        if (!registered)
        {
            // 🔹 Frame should have a positive width and height
            ut.add_test(objName, "Frame Dimensions Validity", [this](std::vector<std::string>& errors)
            {
                if (getWidth() <= 0 || getHeight() <= 0)
                {
                    errors.push_back(
                        "Frame '" + getName() + "' has invalid size: w=" +
                        std::to_string(getWidth()) + " h=" + std::to_string(getHeight()));
                }
                return true; // ✅ test completes in a single frame
            });

            registered = true;
        }

        // ✅ Return false to remain active for multi-frame consistency
        return false;
    } // END: Frame::onUnitTest()



    
    void Frame::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "Frame");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    


} // END: namespace SDOM
