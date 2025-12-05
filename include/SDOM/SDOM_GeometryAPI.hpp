#pragma once

#include <SDOM/SDOM_Geometry.hpp>

namespace SDOM {

class DataRegistry;

namespace GeometryAPI {

// -----------------------------------------------------------------------------
// Anchor helpers
// -----------------------------------------------------------------------------
AnchorPoint anchorPointFromString(const char* name);
const char* anchorPointToString(AnchorPoint anchor);

// -----------------------------------------------------------------------------
// Bounds helpers
// -----------------------------------------------------------------------------
bool setBounds(Bounds* bounds, float left, float top, float right, float bottom);
float getBoundsWidth(const Bounds* bounds);
float getBoundsHeight(const Bounds* bounds);

// -----------------------------------------------------------------------------
// Data registry wiring
// -----------------------------------------------------------------------------
void registerTypes(DataRegistry& registry);

} // namespace GeometryAPI

} // namespace SDOM
