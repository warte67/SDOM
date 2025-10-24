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
        Hamburger = 54,         // | 54    | Hamburger            
        Arrow_Left_Raised,      // | 55    | Left Arrow Raised    
        Arrow_Right_Raised,     // | 56    | Right Arrow Raised   
        Arrow_Up_Raised,        // | 57    | Up Arrow Raised      
        Arrow_Down_Raised,      // | 58    | Down Arrow Raised    
        Arrow_Left_Depressed,   // | 59    | Left Arrow Depressed     
        Arrow_Right_Depressed,  // | 60    | Right Arrow Depressed    
        Arrow_Up_Depressed,     // | 61    | Up Arrow Depressed   
        Arrow_Down_Depressed,   // | 62    | Down Arrow Depressed     
        Knob_Horizontal,        // | 63    | Horizontal Knob      
        Knob_Vertical,          // | 64    | Vertical Knob        
        Slider_Tick,            // | 65    | Slider Tick          
        HSlider_Rail,           // | 66    | HSlider Rail         
        VSlider_Rail,           // | 67    | VSlider Rail         
        Checkbox_Empty,         // | 68    | Empty CheckBox       
        Checkbox_Checked,       // | 69    | Checked Checkbox         
        Checkbox_X,             // | 70    | X'd Checkbox         
        Radiobox_Unselected,    // | 71    | Unselected Radio         
        Radiobox_Selected,      // | 72    | Selected Radio           
        HProgress_Left,         // | 73    | Left HProgress               
        HProgress_Rail,         // | 74    | Empty HProgress          
        HProgress_Thumb,        // | 75    | Thumb HProgress          
        HProgress_Right,        // | 76    | Right HProgress          
        VProgress_Top,          // | 77    | Top VProgress            
        VProgress_Rail,         // | 78    | Empty VProgress              
        VProgress_Thumb,        // | 79    | Thumb VProgress              
        VProgress_Bottom        // | 80    | Bottom VProgress             
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
        { static_cast<int>(IconIndex::HProgress_Rail), "hprogress_rail" },
        { static_cast<int>(IconIndex::HProgress_Thumb), "hprogress_thumb" },
        { static_cast<int>(IconIndex::HProgress_Right), "hprogress_right" },
        { static_cast<int>(IconIndex::VProgress_Top), "vprogress_top" },
        { static_cast<int>(IconIndex::VProgress_Rail), "vprogress_rail" },
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

    // NOTE:
    //      Each Unicode symbol must map to exactly one ExtendedCharacterIndex.
    //      So we need to pick which meaning "owns" the glyph, and the other 
    //      semantic names must become aliases, not separate enum entries. 
    enum class ExtendedCharacterIndex : uint8_t
    {
        // Arrows
          ARROW_UP            // "↑"
        , ARROW_DOWN          // "↓"
        , ARROW_LEFT          // "←"
        , ARROW_RIGHT         // "→"
        , ARROW_UP_DOUBLE     // "⇑"
        , ARROW_DOWN_DOUBLE   // "⇓"
        , ARROW_LEFT_DOUBLE   // "⇐"
        , ARROW_RIGHT_DOUBLE  // "⇒"
        , ARROW_THIN_RIGHT    // "▸"
        , ARROW_COLLAPSED     // "▶"
        , ARROW_EXPANDED      // "▼"    // Collision with ARROW_DOWN_SMALL

        // Checkbox / Toggle States
        , CHECKBOX_EMPTY      // "☐"
        , CHECKBOX_CHECKED    // "☑"
        , CHECKBOX_CROSSED    // "☒"
        , CHECKBOX_MIXED      // "◪"
        , CHECKBOX_FLAT       // "▬"
        , CHECKBOX_BLOCK      // "■"    


        // Radio Buttons
        , RADIO_OFF           // "○"
        , RADIO_ON            // "●"    // Collision with KNOB_ROUND
        , RADIO_DISABLED      // "◎"

        // Progress Shading
        , SHADE_LIGHT         // "░"
        , SHADE_MEDIUM        // "▒"
        , SHADE_DARK          // "▓"
        , SHADE_SOLID         // "█"

        // Box Drawing
        , BOX_TL              // "┌"
        , BOX_TR              // "┐"
        , BOX_BL              // "└"
        , BOX_BR              // "┘"
        , BOX_H               // "─"    // Collision with TRACK_H
        , BOX_V               // "│"    // Collision with TRACK_V
        , BOX_T               // "┬"
        , BOX_B               // "┴"
        , BOX_L               // "├"
        , BOX_R               // "┤"
        , BOX_C               // "┼"

        // reserve up to 255 for future icons        
    };    
    static const std::unordered_map<char32_t, ExtendedCharacterIndex> UnicodeToIconIndex =
    {
        // Arrows
        { U'↑', ExtendedCharacterIndex::ARROW_UP           },
        { U'↓', ExtendedCharacterIndex::ARROW_DOWN         },
        { U'←', ExtendedCharacterIndex::ARROW_LEFT         },
        { U'→', ExtendedCharacterIndex::ARROW_RIGHT        },
        { U'⇑', ExtendedCharacterIndex::ARROW_UP_DOUBLE    },
        { U'⇓', ExtendedCharacterIndex::ARROW_DOWN_DOUBLE  },
        { U'⇐', ExtendedCharacterIndex::ARROW_LEFT_DOUBLE  },
        { U'⇒', ExtendedCharacterIndex::ARROW_RIGHT_DOUBLE },
        { U'▸', ExtendedCharacterIndex::ARROW_THIN_RIGHT   },
        { U'▶', ExtendedCharacterIndex::ARROW_COLLAPSED    },
        { U'▼', ExtendedCharacterIndex::ARROW_EXPANDED     },

        // Checkbox / Toggle States
        { U'☐', ExtendedCharacterIndex::CHECKBOX_EMPTY     },
        { U'☑', ExtendedCharacterIndex::CHECKBOX_CHECKED   },
        { U'☒', ExtendedCharacterIndex::CHECKBOX_CROSSED   },
        { U'◪', ExtendedCharacterIndex::CHECKBOX_MIXED     },
        { U'▬', ExtendedCharacterIndex::CHECKBOX_FLAT      },

        // Radio Buttons
        { U'○', ExtendedCharacterIndex::RADIO_OFF         },
        { U'●', ExtendedCharacterIndex::RADIO_ON          },
        { U'◎', ExtendedCharacterIndex::RADIO_DISABLED    },

        // Progress Shading
        { U'░', ExtendedCharacterIndex::SHADE_LIGHT       },
        { U'▒', ExtendedCharacterIndex::SHADE_MEDIUM      },
        { U'▓', ExtendedCharacterIndex::SHADE_DARK        },
        { U'█', ExtendedCharacterIndex::SHADE_SOLID       },

        // Box Drawing
        { U'┌', ExtendedCharacterIndex::BOX_TL             },
        { U'┐', ExtendedCharacterIndex::BOX_TR             },
        { U'└', ExtendedCharacterIndex::BOX_BL             },
        { U'┘', ExtendedCharacterIndex::BOX_BR             },
        { U'─', ExtendedCharacterIndex::BOX_H              },
        { U'│', ExtendedCharacterIndex::BOX_V              },
        { U'┬', ExtendedCharacterIndex::BOX_T              },
        { U'┴', ExtendedCharacterIndex::BOX_B              },
        { U'├', ExtendedCharacterIndex::BOX_L              },
        { U'┤', ExtendedCharacterIndex::BOX_R              },
        { U'┼', ExtendedCharacterIndex::BOX_C              },

        // reserve up to 255 for future icons                
    };

    // uint8_t Font::resolveGlyph(char32_t ch)
    // {
    //     // First check ASCII/primary bitmap font
    //     if (ch < 256) { return { 0, uint8_t(ch) }; }
    //
    //     // Then check the icon sheet map
    //     auto it = UnicodeToIconIndex.find(ch);
    //     if (it != UnicodeToIconIndex.end())
    //         return { 1, uint8_t(it->second) };  // Plane 1 = icon_8x8
    //
    //     // Otherwise fallback (missing glyph)
    //     return { 0, (uint8_t)'?' };
    // }

} // END: namespace SDOM





/*

// Standard Arrows:
ARROW_UP        = "↑"
ARROW_DOWN      = "↓"
ARROW_LEFT      = "←"
ARROW_RIGHT     = "→"

// Double Arrows:
ARROW_UP_DOUBLE    = "⇑"
ARROW_DOWN_DOUBLE  = "⇓"
ARROW_LEFT_DOUBLE  = "⇐"
ARROW_RIGHT_DOUBLE = "⇒"

// Thin Arrows (used for UI disclosure trees, subtle pointer)
ARROW_THIN_UP      = "▴"
ARROW_THIN_DOWN    = "▾"
ARROW_THIN_LEFT    = "◂"
ARROW_THIN_RIGHT   = "▸"

// Thick Arrows (bold icon directional indicators)
ARROW_THICK_UP     = "▲"
ARROW_THICK_DOWN   = "▼"
ARROW_THICK_LEFT   = "◀"
ARROW_THICK_RIGHT  = "▶"



## Thin Line Segment Glyphs (Bitmask: N=1, E=2, S=4, W=8)

| Hex | Bits (N E S W) | Name            | Glyph | Unicode | Description                         |
|----:|:--------------:|----------------|:-----:|---------|--------------------------------------|
| 0x0 | 0000           | LINE_NONE      | ␠     | (space) | No line                              |
| 0x1 | 0001           | LINE_THIN_N    | ╵     | U+2575  | Line upward from center              |
| 0x2 | 0010           | LINE_THIN_E    | ╶     | U+2576  | Line right from center               |
| 0x3 | 0011           | LINE_THIN_NE   | └     | U+2514  | Up + Right                           |
| 0x4 | 0100           | LINE_THIN_S    | ╷     | U+2577  | Line downward from center            |
| 0x5 | 0101           | LINE_THIN_NS   | │     | U+2502  | Vertical (Up + Down)                 |
| 0x6 | 0110           | LINE_THIN_ES   | ┌     | U+250C  | Right + Down                         |
| 0x7 | 0111           | LINE_THIN_NES  | ┤     | U+2524  | Up + Down + Right (T-left)           |
| 0x8 | 1000           | LINE_THIN_W    | ╴     | U+2574  | Line left from center                |
| 0x9 | 1001           | LINE_THIN_NW   | ┘     | U+2518  | Up + Left                            |
| 0xA | 1010           | LINE_THIN_EW   | ─     | U+2500  | Horizontal (Left + Right)            |
| 0xB | 1011           | LINE_THIN_NEW  | ┴     | U+2534  | Up + Left + Right (T-down)           |
| 0xC | 1100           | LINE_THIN_SW   | ┐     | U+2510  | Down + Left                          |
| 0xD | 1101           | LINE_THIN_NSW  | ├     | U+251C  | Up + Down + Left (T-right)           |
| 0xE | 1110           | LINE_THIN_ESW  | ┬     | U+252C  | Down + Left + Right (T-up)           |
| 0xF | 1111           | LINE_THIN_NESW | ┼     | U+253C  | Up + Right + Down + Left (Cross)     |


## Quadrant Mapped Blocks

| Hex | Bits (TR BR BL TL) | Symbol |   Code  | Name                                       |
| --: | :----------------: | :----: | :-----: | ------------------------------------------ |
| 0x0 |        0000        |    ⠀   | (space) | Empty                                      |
| 0x1 |        0001        |    ▝   |  U+259D | Quadrant Upper Right                       |
| 0x2 |        0010        |    ▗   |  U+2597 | Quadrant Lower Right                       |
| 0x3 |        0011        |    ▐   |  U+2590 | Right Half Block                           |
| 0x4 |        0100        |    ▖   |  U+2596 | Quadrant Lower Left                        |
| 0x5 |        0101        |    ▞   |  U+259E | Diagonal (Upper Right + Lower Left)        |
| 0x6 |        0110        |    ▙   |  U+2599 | All but Upper Right (full lower half + UL) |
| 0x7 |        0111        |    ▟   |  U+259F | All but Upper Left                         |
| 0x8 |        1000        |    ▘   |  U+2598 | Quadrant Upper Left                        |
| 0x9 |        1001        |    ▚   |  U+259A | Diagonal (Upper Left + Lower Right)        |
| 0xA |        1010        |    ▛   |  U+259B | All but Lower Right                        |
| 0xB |        1011        |    ▜   |  U+259C | All but Lower Left                         |
| 0xC |        1100        |    ▙   |  U+2599 | Bottom Half + Left (non-diagonal)          |
| 0xD |        1101        |    ▛   |  U+259B | Variation (depends symmetry rules)         |
| 0xE |        1110        |    ▜   |  U+259C | See note below                             |
| 0xF |        1111        |    █   |  U+2588 | Full Block                                 |







*/