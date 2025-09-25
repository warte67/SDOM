// SDOM_Front.hpp

// Note:
// SDOM_Front.hpp is the umbrella header for SDOM, providing a single entry point
// that includes all SDOM system headers. It is intended for use in implementation
// (.cpp) files or by end-users who want access to the full SDOM API with one include.
// Never include SDOM_Front.hpp in interface (header) files, as this can cause circular
// dependencies and excessive recompilation. For interface headers, use SDOM_Types.hpp
// or include only the specific SDOM headers you need.


#ifndef __SDOM_FRONT_HPP__ 
#define __SDOM_FRONT_HPP__
    #ifndef SDOM_USE_INDIVIDUAL_HEADERS

        #define SOL_ALL_SAFETIES_ON 1
        #include <sol/sol.hpp>

        #include <SDOM/json.hpp>  // This one is going away

        #include <SDOM/SDOM.hpp>
        #include <SDOM/SDOM_CLR.hpp>
        #include <SDOM/SDOM_Core.hpp>
        #include <SDOM/SDOM_Event.hpp>
        #include <SDOM/SDOM_EventManager.hpp>
        #include <SDOM/SDOM_EventType.hpp>
        #include <SDOM/SDOM_EventTypeHash.hpp>
        #include <SDOM/SDOM_Factory.hpp>
        #include <SDOM/SDOM_Front.hpp>
        #include <SDOM/SDOM_Handle.hpp>
        #include <SDOM/SDOM_IDataObject.hpp>
        #include <SDOM/SDOM_IDisplayObject.hpp>
        #include <SDOM/SDOM_IResourceObject.hpp>
        #include <SDOM/SDOM_IUnitTest.hpp>
        #include <SDOM/SDOM_SDL_Utils.hpp>
        #include <SDOM/SDOM_Stage.hpp>
        #include <SDOM/SDOM_UnitTests.hpp>

        // ...

    #endif // SDOM_USE_INDIVIDUAL_HEADERS
#endif // __SDOM_FRONT_HPP__

