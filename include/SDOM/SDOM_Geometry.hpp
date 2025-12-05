/***  SDOM_Geometry.hpp  ****************************
 *  Shared geometry primitives (anchor enums, bounds structs, etc.) used by
 *  display objects, layout helpers, and binding metadata.
 *
 *  This header is intentionally lightweight so both engine and generated C
 *  interfaces can include it without dragging in the entirety of SDOM.hpp.
 *
 *  Released under the ZLIB License.
 *  Original Author: Jay Faries (warte67)
 *
 ****************************************************/

#pragma once

#include <cstdint>
#include <cmath>
#include <string>
#include <unordered_map>

namespace SDOM {

/**
 * @enum AnchorPoint
 * @brief Reference points for anchoring child edges to a parent.
 * @details
 * Specifies where each edge of a child display object should be anchored
 * relative to its parent, enabling flexible positioning and layout.
 */
enum class AnchorPoint : std::uint8_t
{
    DEFAULT         = 0,
    TOP_LEFT        = 0,
    TOP_CENTER      = 1,
    TOP_RIGHT       = 2,
    MIDDLE_LEFT     = 3,
    MIDDLE_CENTER   = 4,
    MIDDLE_RIGHT    = 5,
    BOTTOM_LEFT     = 6,
    BOTTOM_CENTER   = 7,
    BOTTOM_RIGHT    = 8,

    LEFT            = MIDDLE_LEFT,
    CENTER          = MIDDLE_CENTER,
    RIGHT           = MIDDLE_RIGHT,
    TOP             = TOP_CENTER,
    MIDDLE          = MIDDLE_CENTER,
    BOTTOM          = BOTTOM_CENTER,
    LEFT_TOP        = TOP_LEFT,
    CENTER_TOP      = TOP_CENTER,
    RIGHT_TOP       = TOP_RIGHT,
    LEFT_MIDDLE     = MIDDLE_LEFT,
    CENTER_MIDDLE   = MIDDLE_CENTER,
    RIGHT_MIDDLE    = MIDDLE_RIGHT,
    LEFT_BOTTOM     = BOTTOM_LEFT,
    CENTER_BOTTOM   = BOTTOM_CENTER,
    RIGHT_BOTTOM    = BOTTOM_RIGHT,
};

inline static const std::unordered_map<AnchorPoint, std::string> anchorPointToString_ = {
    { AnchorPoint::TOP_LEFT,     "top_left" },
    { AnchorPoint::TOP_CENTER,   "top_center" },
    { AnchorPoint::TOP_RIGHT,    "top_right" },
    { AnchorPoint::MIDDLE_LEFT,  "middle_left" },
    { AnchorPoint::MIDDLE_CENTER,"middle_center" },
    { AnchorPoint::MIDDLE_RIGHT, "middle_right" },
    { AnchorPoint::BOTTOM_LEFT,  "bottom_left" },
    { AnchorPoint::BOTTOM_CENTER,"bottom_center" },
    { AnchorPoint::BOTTOM_RIGHT, "bottom_right" }
};

inline static const std::unordered_map<std::string, AnchorPoint> stringToAnchorPoint_ = {
    { "default",        AnchorPoint::DEFAULT },
    { "left",           AnchorPoint::LEFT },
    { "center",         AnchorPoint::CENTER },
    { "right",          AnchorPoint::RIGHT },
    { "top",            AnchorPoint::TOP },
    { "top_left",       AnchorPoint::TOP_LEFT },
    { "top_center",     AnchorPoint::TOP_CENTER },
    { "top_right",      AnchorPoint::TOP_RIGHT },
    { "middle",         AnchorPoint::MIDDLE },
    { "middle_left",    AnchorPoint::MIDDLE_LEFT },
    { "middle_center",  AnchorPoint::MIDDLE_CENTER },
    { "middle_right",   AnchorPoint::MIDDLE_RIGHT },
    { "bottom",         AnchorPoint::BOTTOM },
    { "bottom_left",    AnchorPoint::BOTTOM_LEFT },
    { "bottom_center",  AnchorPoint::BOTTOM_CENTER },
    { "bottom_right",   AnchorPoint::BOTTOM_RIGHT },
    { "center_middle",  AnchorPoint::MIDDLE_CENTER },
    { "right_middle",   AnchorPoint::MIDDLE_RIGHT },
    { "left_middle",    AnchorPoint::MIDDLE_LEFT },
    { "center_top",     AnchorPoint::TOP_CENTER },
    { "right_top",      AnchorPoint::TOP_RIGHT },
    { "left_top",       AnchorPoint::TOP_LEFT },
    { "center_bottom",  AnchorPoint::BOTTOM_CENTER },
    { "right_bottom",   AnchorPoint::BOTTOM_RIGHT },
    { "left_bottom",    AnchorPoint::BOTTOM_LEFT },
};

/**
 * @struct Bounds
 * @brief Axis-aligned rectangle edges.
 */
struct Bounds
{
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;

    float width() const { return std::abs(right - left); }
    float height() const { return std::abs(bottom - top); }

    bool operator==(const Bounds& other) const {
        constexpr float epsilon = 0.0001f;
        return std::abs(left - other.left)   < epsilon &&
               std::abs(top - other.top)     < epsilon &&
               std::abs(right - other.right) < epsilon &&
               std::abs(bottom - other.bottom) < epsilon;
    }

    bool operator!=(const Bounds& other) const {
        return !(*this == other);
    }
};

} // namespace SDOM
