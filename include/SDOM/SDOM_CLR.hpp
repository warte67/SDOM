/********************************************************************************
 * SDOM_CLR.hpp
 * 
 *      _____ _      _____        _                 
 *     / ____| |    |  __ \      | |                
 *    | |    | |    | |__) |     | |__  _ __  _ __  
 *    | |    | |    |  _  /      | '_ \| '_ \| '_ \ 
 *    | |____| |____| | \ \   _  | | | | |_) | |_) |
 *     \_____|______|_|  \_\ (_) |_| |_| .__/| .__/ 
 *                                     | |   | |    
 *                                     |_|   |_|   
 * 
 * Static container for ANSI escape sequences and rendering helpers.
 * ISO-compliant: uses "\x1B[" instead of "\e".
 ********************************************************************************/

#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <sol/sol.hpp>
#include <cstdio>

// Simple 8/16/32-bit architecture aliases
#ifndef Byte
    #define Byte Uint8
#endif
#ifndef Word
    #define Word Uint16
#endif
#ifndef DWord
    #define DWord Uint32
#endif

// Cross-platform console includes
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #include <Windows.h>
#elif defined(__linux__)
    #include <sys/ioctl.h>
#endif

/**
 * @brief Static container for ANSI escape sequences and CLI helpers.
 *
 * Cannot be instantiated. All members are static.
 * Color, formatting, cursor movement, box drawing, etc.
 */
class CLR
{
private:
    CLR();
    ~CLR();
    CLR(const CLR&) = delete;
    CLR(CLR&&) = delete;
    CLR& operator=(const CLR&) = delete;
    CLR& operator=(CLR&&) = delete;

    // ISO-compliant ESC prefix
    static constexpr const char* ESC = "\x1B[";

public:

    // ========================================================================
    // 8-bit and 24-bit color helpers
    // ========================================================================

    inline static std::string fg(Byte color)
    { return std::string(ESC) + "38;5;" + std::to_string(color) + "m"; }

    inline static std::string bg(Byte color)
    { return std::string(ESC) + "48;5;" + std::to_string(color) + "m"; }

    inline static std::string fg_rgb(Byte r, Byte g, Byte b)
    {
        return std::string(ESC) + "38;2;" +
               std::to_string(r) + ";" +
               std::to_string(g) + ";" +
               std::to_string(b) + "m";
    }

    inline static std::string bg_rgb(Byte r, Byte g, Byte b)
    {
        return std::string(ESC) + "48;2;" +
               std::to_string(r) + ";" +
               std::to_string(g) + ";" +
               std::to_string(b) + "m";
    }

    // Convert 8-bit pattern {R:3,G:3,B:2} to RGB ANSI
    inline static std::string fg_color(Byte clr) {
        Uint8 r3 = (clr >> 5) & 0x7;
        Uint8 g3 = (clr >> 2) & 0x7;
        Uint8 b2 = clr & 0x3;

        Uint8 r8 = static_cast<Uint8>((r3 * 255 + 3) / 7);
        Uint8 g8 = static_cast<Uint8>((g3 * 255 + 3) / 7);
        Uint8 b8 = static_cast<Uint8>((b2 * 255 + 1) / 3);

        return fg_rgb(r8, g8, b8);
    }

    inline static std::string bg_color(Byte clr) {
        Uint8 r3 = (clr >> 5) & 0x7;
        Uint8 g3 = (clr >> 2) & 0x7;
        Uint8 b2 = clr & 0x3;

        Uint8 r8 = static_cast<Uint8>((r3 * 255 + 3) / 7);
        Uint8 g8 = static_cast<Uint8>((g3 * 255 + 3) / 7);
        Uint8 b8 = static_cast<Uint8>((b2 * 255 + 1) / 3);

        return bg_rgb(r8, g8, b8);
    }

    // ========================================================================
    // Cursor movement / positioning
    // ========================================================================

    inline static std::string set_cursor_pos(Byte row = 1, Byte col = 1)
    { return std::string(ESC) + std::to_string(row) + ";" + std::to_string(col) + "H"; }

    inline static std::string save_cursor()
    { return std::string(ESC) + "s"; }

    inline static std::string restore_cursor()
    { return std::string(ESC) + "u"; }

    inline static void write_at(Byte row, Byte col, const std::string& s) {
        std::fwrite(save_cursor().c_str(), 1, save_cursor().size(), stdout);
        std::fwrite(set_cursor_pos(row, col).c_str(), 1, set_cursor_pos(row, col).size(), stdout);
        std::fwrite(s.c_str(), 1, s.size(), stdout);
        std::fwrite(restore_cursor().c_str(), 1, restore_cursor().size(), stdout);
        std::fflush(stdout);
    }

    // SDL rendering hook
    static bool draw_debug_text(const std::string& text, int x, int y,
                                int ptsize = 12,
                                Uint8 r = 255, Uint8 g = 255,
                                Uint8 b = 255, Uint8 a = 255);

    static bool get_cursor_pos(int& row, int& col);

    inline static std::string erase_in_display(Byte n = 0)
    { return std::string(ESC) + std::to_string(n) + "J"; }

    inline static std::string erase_in_line(Byte n = 0)
    { return std::string(ESC) + std::to_string(n) + "K"; }


    // ========================================================================
    // Formatting Constants
    // ========================================================================

    inline static const std::string RESET      = std::string(ESC) + "0m";
    inline static const std::string NORMAL     = std::string(ESC) + "0m";
    inline static const std::string RETURN     = std::string(ESC) + "0m\n";
    inline static const std::string BOLD       = std::string(ESC) + "1m";
    inline static const std::string DIM        = std::string(ESC) + "2m";
    inline static const std::string ITALIC     = std::string(ESC) + "3m";
    inline static const std::string UNDERLINE  = std::string(ESC) + "4m";
    inline static const std::string BLINKING   = std::string(ESC) + "5m";
    inline static const std::string REVERSED   = std::string(ESC) + "7m";
    inline static const std::string CONCEALED  = std::string(ESC) + "8m";
    inline static const std::string STRIKE     = std::string(ESC) + "9m";
    inline static const std::string ERASE_LINE = std::string(ESC) + "2K\r";


    // ========================================================================
    // Foreground Colors
    // ========================================================================

    inline static const std::string BLACK      = std::string(ESC) + "0;30m";
    inline static const std::string RED        = std::string(ESC) + "0;31m";
    inline static const std::string GREEN      = std::string(ESC) + "0;32m";
    inline static const std::string BROWN      = std::string(ESC) + "0;33m";
    inline static const std::string BLUE       = std::string(ESC) + "0;34m";
    inline static const std::string PURPLE     = std::string(ESC) + "0;35m";
    inline static const std::string MAGENTA    = std::string(ESC) + "0;35m";
    inline static const std::string CYAN       = std::string(ESC) + "0;36m";
    inline static const std::string GREY       = std::string(ESC) + "0;37m";

    inline static const std::string DARK       = std::string(ESC) + "1;30m";
    inline static const std::string ORANGE     = std::string(ESC) + "1;31m";
    inline static const std::string LT_GRN     = std::string(ESC) + "1;32m";
    inline static const std::string YELLOW     = std::string(ESC) + "1;33m";
    inline static const std::string LT_BLUE    = std::string(ESC) + "1;34m";
    inline static const std::string PINK       = std::string(ESC) + "1;35m";
    inline static const std::string LT_CYAN    = std::string(ESC) + "1;36m";
    inline static const std::string WHITE      = std::string(ESC) + "1;37m";

    inline static const std::string LT_MAGENTA = fg_rgb(255, 105, 180);
    inline static const std::string LT_ORANGE  = fg_rgb(255, 165, 79);
    inline static const std::string LT_YELLOW  = fg_rgb(255, 255, 102);


    // ========================================================================
    // Background Colors
    // ========================================================================

    inline static const std::string BG_BLACK      = std::string(ESC) + "0;40m";
    inline static const std::string BG_RED        = std::string(ESC) + "0;41m";
    inline static const std::string BG_GREEN      = std::string(ESC) + "0;42m";
    inline static const std::string BG_BROWN      = std::string(ESC) + "0;43m";
    inline static const std::string BG_BLUE       = std::string(ESC) + "0;44m";
    inline static const std::string BG_PURPLE     = std::string(ESC) + "0;45m";
    inline static const std::string BG_CYAN       = std::string(ESC) + "0;46m";
    inline static const std::string BG_GREY       = std::string(ESC) + "0;47m";
    inline static const std::string BG_DEFAULT    = std::string(ESC) + "0;49m";

    inline static const std::string BG_DARK       = std::string(ESC) + "0;100m";
    inline static const std::string BG_LT_RED     = std::string(ESC) + "0;101m";
    inline static const std::string BG_LT_GREEN   = std::string(ESC) + "0;102m";
    inline static const std::string BG_YELLOW     = std::string(ESC) + "0;103m";
    inline static const std::string BG_LT_BLUE    = std::string(ESC) + "0;104m";
    inline static const std::string BG_PINK       = std::string(ESC) + "0;105m";
    inline static const std::string BG_LT_CYAN    = std::string(ESC) + "0;106m";
    inline static const std::string BG_WHITE      = std::string(ESC) + "0;107m";


    // ========================================================================
    // Unicode UI Symbols
    // ========================================================================

    inline static const std::string ARROW_UP           = "↑";
    inline static const std::string ARROW_DOWN         = "↓";
    inline static const std::string ARROW_LEFT         = "←";
    inline static const std::string ARROW_RIGHT        = "→";
    inline static const std::string ARROW_UP_DOUBLE    = "⇑";
    inline static const std::string ARROW_DOWN_DOUBLE  = "⇓";
    inline static const std::string ARROW_LEFT_DOUBLE  = "⇐";
    inline static const std::string ARROW_RIGHT_DOUBLE = "⇒";
    inline static const std::string ARROW_THIN_RIGHT   = "▸";
    inline static const std::string ARROW_COLLAPSED    = "▶";
    inline static const std::string ARROW_EXPANDED     = "▼";

    inline static const std::string CHECKBOX_EMPTY     = "☐";
    inline static const std::string CHECKBOX_CHECKED   = "☑";
    inline static const std::string CHECKBOX_CROSSED   = "☒";

    inline static const std::string RADIO_OFF          = "○";
    inline static const std::string RADIO_ON           = "●";

    inline static const std::string TRACK_H            = "─";
    inline static const std::string TRACK_V            = "│";
    inline static const std::string KNOB_BLOCK         = "■";
    inline static const std::string KNOB_ROUND         = "●";
    inline static const std::string ARROW_UP_SMALL     = "▲";
    inline static const std::string ARROW_DOWN_SMALL   = "▼";
    inline static const std::string ARROW_LEFT_SMALL   = "◄";
    inline static const std::string ARROW_RIGHT_SMALL  = "►";

    inline static const std::string SHADE_LIGHT        = "░";
    inline static const std::string SHADE_MEDIUM       = "▒";
    inline static const std::string SHADE_DARK         = "▓";
    inline static const std::string SHADE_SOLID        = "█";

    inline static const std::string BOX_TL             = "┌";
    inline static const std::string BOX_TR             = "┐";
    inline static const std::string BOX_BL             = "└";
    inline static const std::string BOX_BR             = "┘";
    inline static const std::string BOX_H              = "─";
    inline static const std::string BOX_V              = "│";
    inline static const std::string BOX_T              = "┬";
    inline static const std::string BOX_B              = "┴";
    inline static const std::string BOX_L              = "├";
    inline static const std::string BOX_R              = "┤";
    inline static const std::string BOX_C              = "┼";


    // ========================================================================
    // Terminal Size
    // ========================================================================

    inline static void get_terminal_size(int& width, int& height) {
#if defined(_WIN32)
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        width  = static_cast<int>(csbi.srWindow.Right - csbi.srWindow.Left + 1);
        height = static_cast<int>(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
#elif defined(__linux__) || defined(__APPLE__)
        struct winsize w {};
        ioctl(fileno(stdout), TIOCGWINSZ, &w);
        width  = static_cast<int>(w.ws_col);
        height = static_cast<int>(w.ws_row);
#endif
    }


    // ========================================================================
    // Formatting helpers
    // ========================================================================

    inline static std::string indent_push()
    { return std::string(indent_level++ * 2, ' '); }

    inline static std::string indent_pop()
    { return std::string((indent_level-- > 0 ? indent_level : 0) * 2, ' '); }

    inline static std::string indent()
    { return std::string(indent_level * 2, ' '); }

    inline static std::string hex(Uint32 n, Uint8 d)
    {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    }

    inline static std::string pad(std::string text, Uint8 d)
    {
        while (text.length() < d)
            text.push_back(' ');
        return text;
    }

    // -----------------------------------------------------------------------
    // Lua exposure
    // -----------------------------------------------------------------------
    static void exposeToLua(sol::state& lua);

private:
    inline static int indent_level = 0;
};

