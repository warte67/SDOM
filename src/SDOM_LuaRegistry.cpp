#include <SDOM/SDOM_LuaRegistry.hpp>

namespace SDOM {

LuaBindingRegistry& getLuaBindingRegistry() {
    static LuaBindingRegistry inst;
    return inst;
}

} // namespace SDOM
