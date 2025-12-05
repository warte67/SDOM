#pragma once

#include <string>

namespace SDOM {

class IDisplayObject;

namespace IDisplayObjectAPI {

// Registers the IDataObject reflection metadata and callable bindings for
// IDisplayObject-derived modules. Additional helpers will be added here as
// the public surface expands.
void registerBindings(IDisplayObject& object, const std::string& typeName);

} // namespace IDisplayObjectAPI

} // namespace SDOM
