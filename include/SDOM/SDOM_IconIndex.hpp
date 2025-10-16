// SDOM_IconIndex.hpp

#pragma once

#include <unordered_map>
#include <string>
#include <optional>

namespace SDOM
{
    enum class IconIndex : int
    {
        // Individual Indices
        Hamburger = 54,
        Arrow_Left_Raised,
        Arrow_Right_Raised,
        Arrow_Up_Raised,
        Arrow_Down_Raised,
        Arrow_Left_Depressed,
        Arrow_Right_Depressed,
        Arrow_Up_Depressed,
        Arrow_Down_Depressed,
        Knob_Horizontal,
        Knob_Vertical,
        Slider_Tick,
        HSlider_Rail,
        VSlider_Rail,
        Checkbox_Empty,
        Checkbox_Checked,
        Checkbox_X,
        Radiobox_Unselected,
        Radiobox_Selected,
        HProgress_Left,
        HProgress_Empty,
        HProgress_Thumb,
        HProgress_Right,
        VProgress_Top,
        VProgress_Empty,
        VProgress_Thumb,
        VProgress_Bottom
    };

    inline static const std::unordered_map<int, std::string> icon_index_to_string = {
        { static_cast<int>(IconIndex::Hamburger), "hamburger" },
        { static_cast<int>(IconIndex::Arrow_Left_Raised), "arrow_left_raised" },
        { static_cast<int>(IconIndex::Arrow_Right_Raised), "arrow_right_raised" },
        { static_cast<int>(IconIndex::Arrow_Up_Raised), "arrow_up_raised" },
        { static_cast<int>(IconIndex::Arrow_Down_Raised), "arrow_down_raised" },
        { static_cast<int>(IconIndex::Arrow_Left_Depressed), "arrow_left_depressed" },
        { static_cast<int>(IconIndex::Arrow_Right_Depressed), "arrow_right_depressed" },
        { static_cast<int>(IconIndex::Arrow_Up_Depressed), "arrow_up_depressed" },
        { static_cast<int>(IconIndex::Arrow_Down_Depressed), "arrow_down_depressed" },
        { static_cast<int>(IconIndex::Knob_Horizontal), "knob_horizontal" },
        { static_cast<int>(IconIndex::Knob_Vertical), "knob_vertical" },
        { static_cast<int>(IconIndex::Slider_Tick), "slider_tick" },
        { static_cast<int>(IconIndex::HSlider_Rail), "hslider_rail" },
        { static_cast<int>(IconIndex::VSlider_Rail), "vslider_rail" },
        { static_cast<int>(IconIndex::Checkbox_Empty), "empty_checkbox" },
        { static_cast<int>(IconIndex::Checkbox_Checked), "checked_checkbox" },
        { static_cast<int>(IconIndex::Checkbox_X), "x_checkbox" },
        { static_cast<int>(IconIndex::Radiobox_Unselected), "radiobox_unselected" },
        { static_cast<int>(IconIndex::Radiobox_Selected), "radiobox_selected" },
        { static_cast<int>(IconIndex::HProgress_Left), "hprogress_left" },
        { static_cast<int>(IconIndex::HProgress_Empty), "hprogress_empty" },
        { static_cast<int>(IconIndex::HProgress_Thumb), "hprogress_thumb" },
        { static_cast<int>(IconIndex::HProgress_Right), "hprogress_right" },
        { static_cast<int>(IconIndex::VProgress_Top), "vprogress_top" },
        { static_cast<int>(IconIndex::VProgress_Empty), "vprogress_empty" },
        { static_cast<int>(IconIndex::VProgress_Thumb), "vprogress_thumb" },
        { static_cast<int>(IconIndex::VProgress_Bottom), "vprogress_bottom" }
    };

    inline static const std::unordered_map<std::string, int> icon_string_to_index = []{
        std::unordered_map<std::string,int> m;
        for (auto &p : icon_index_to_string) {
            std::string key = p.second;
            std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
                return static_cast<char>(std::tolower(c));
            });
            m.emplace(std::move(key), p.first);
        }
        return m;
    }();

    inline static std::optional<std::string> icon_name_from_index(IconIndex idx)
    {
        auto it = icon_index_to_string.find(static_cast<int>(idx));
        if (it == icon_index_to_string.end()) return std::nullopt;
        std::string name = it->second;
        std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){
            return static_cast<char>(std::tolower(c));
        });
        return name;
    }

    inline static std::optional<IconIndex> icon_index_from_name(const std::string &name)
    {
        std::string key = name;
        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
            return static_cast<char>(std::tolower(c));
        });
        auto it = icon_string_to_index.find(key);
        if (it == icon_string_to_index.end()) return std::nullopt;
        return static_cast<IconIndex>(it->second);
    }
} // END: namespace SDOM