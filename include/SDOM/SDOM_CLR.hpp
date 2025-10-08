/*** SDOM_CLR.hpp ***********
 *      _____ _      _____        _                 
 *     / ____| |    |  __ \      | |                
 *    | |    | |    | |__) |     | |__  _ __  _ __  
 *    | |    | |    |  _  /      | '_ \| '_ \| '_ \ 
 *    | |____| |____| | \ \   _  | | | | |_) | |_) |
 *     \_____|______|_|  \_\ (_) |_| |_| .__/| .__/ 
 *                                     | |   | |    
 *                                     |_|   |_|   
 * 
 * Defines a set of ANSI escape sequences to colorize and format text in the terminal.
 * 
 * Static container object. Cannot be instantiated.
 * May require periodic fflush(stdout).
 * See: https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
 * 
 ***********/

/**
 * @file SDOM_CLR.hpp
 * @brief Static container for ANSI escape sequences for terminal text formatting and colorization.
 * @details
 *
 * Static container object. Cannot be instantiated.
 * Defines a set of ANSI escape sequences to colorize and format text in the terminal.
 * May require periodic fflush(stdout).
 * See: https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
 *
 * @author Jay Faries (warte67)
 * @copyright GPL v3.0 License
 * @date 2025-09-19
 */
#pragma once

#include <string>
#include <SDL3/SDL.h>
// Expose CLR helpers to embedded Lua states
#include <sol/sol.hpp>
#include <cstdio>

// simple types for 8-bit archetecture 
#ifndef Byte
    #define Byte Uint8
#endif
#ifndef Word
    #define Word Uint16
#endif
#ifndef DWord
    #define DWord Uint32
#endif

// cross-platform stuff for Linux, macos, and windows
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #include <Windows.h>
#elif defined(__linux__)
    #include <sys/ioctl.h>
#endif // Windows/Linux

/**
 * @brief Static container for ANSI escape sequences and formatting helpers.
 * @details
 * Provides color and formatting utilities for terminal output. Cannot be instantiated.
 * All members are static. See individual groups for details.
 */
class CLR
{
private:
    CLR();                                  // private constructor
    ~CLR();                                 // private destructor
    CLR(const CLR&) = delete;				// delete the copy constructor
    CLR(CLR&&) = delete;					// delete the move constructor
    CLR& operator=(const CLR&) = delete;	// delete the copy assignment operator
    CLR& operator=(CLR&&) = delete;			// delete the move assignment operator


public:

    /**
     * @name Utility Member Functions
     * @brief Functions for generating ANSI escape sequences for terminal formatting.
     * @{
     */

    /**
     * @brief Returns ANSI escape sequence to set 8-bit foreground color.
     * @param color 8-bit color value (0-255).
     * @return ANSI escape sequence as a string.
     */
    inline static const std::string fg(Byte color) 
    { return "\e[38;5;" + std::to_string(color) + "m"; } 

    /**
     * @brief Returns ANSI escape sequence to set 8-bit background color.
     * @param color 8-bit color value (0-255).
     * @return ANSI escape sequence as a string.
     */
    inline static const std::string bg(Byte color) 
    { return "\e[48;5;" + std::to_string(color) + "m"; } 

    /**
     * @brief Returns ANSI escape sequence to set 24-bit (RGB) foreground color.
     * @param r Red component (0-255).
     * @param g Green component (0-255).
     * @param b Blue component (0-255).
     * @return ANSI escape sequence as a string.
     */
    inline static const std::string fg_rgb(Byte r, Byte g, Byte b) 
    { return "\e[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m"; }

    /**
     * @brief Map a single 8-bit index into an RGB color using the bit-pattern {r:3,g:3,b:2}
     * and return the corresponding ANSI 24-bit foreground escape sequence.
     * @param clr 8-bit index where bits 7..5 = r (3 bits), 4..2 = g (3 bits), 1..0 = b (2 bits)
     * @return ANSI escape sequence for 24-bit foreground color.
     */
    inline static const std::string fg_color(Byte clr) {
        // extract bitfields
        Uint8 r3 = (clr >> 5) & 0x7; // 3 bits
        Uint8 g3 = (clr >> 2) & 0x7; // 3 bits
        Uint8 b2 = clr & 0x3;        // 2 bits

        // expand to 0..255 (rounding)
        Uint8 r8 = (Uint8)((r3 * 255 + 3) / 7);
        Uint8 g8 = (Uint8)((g3 * 255 + 3) / 7);
        Uint8 b8 = (Uint8)((b2 * 255 + 1) / 3);

        return fg_rgb(r8, g8, b8);
    }

    /**
     * @brief Map a single 8-bit index into an RGB color using the bit-pattern {r:3,g:3,b:2}
     * and return the corresponding ANSI 24-bit background escape sequence.
     * @param clr 8-bit index where bits 7..5 = r (3 bits), 4..2 = g (3 bits), 1..0 = b (2 bits)
     * @return ANSI escape sequence for 24-bit background color.
     */
    inline static const std::string bg_color(Byte clr) {
        // extract bitfields
        Uint8 r3 = (clr >> 5) & 0x7; // 3 bits
        Uint8 g3 = (clr >> 2) & 0x7; // 3 bits
        Uint8 b2 = clr & 0x3;        // 2 bits

        // expand to 0..255 (rounding)
        Uint8 r8 = (Uint8)((r3 * 255 + 3) / 7);
        Uint8 g8 = (Uint8)((g3 * 255 + 3) / 7);
        Uint8 b8 = (Uint8)((b2 * 255 + 1) / 3);

        return bg_rgb(r8, g8, b8);
    }

    /**
     * @brief Returns ANSI escape sequence to set 24-bit (RGB) background color.
     * @param r Red component (0-255).
     * @param g Green component (0-255).
     * @param b Blue component (0-255).
     * @return ANSI escape sequence as a string.
     */
    inline static const std::string bg_rgb(Byte r, Byte g, Byte b) 
    { return "\e[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m"; }

    /**
     * @brief Returns ANSI escape sequence to move the cursor to a specific position.
     * @param row Row number (1-based).
     * @param col Column number (1-based).
     * @return ANSI escape sequence as a string.
     */
    inline static const std::string set_cursor_pos(Byte row=1, Byte col=1) 
    { return "\e[" + std::to_string(row) + ";" + std::to_string(col) + "H"; }

    /**
     * @brief Returns ANSI escape sequence to save the current cursor position.
     * @note Many terminals support ESC[s to save and ESC[u to restore.
     */
    inline static const std::string save_cursor() { return "\e[s"; }

    /**
     * @brief Returns ANSI escape sequence to restore a previously saved cursor position.
     */
    inline static const std::string restore_cursor() { return "\e[u"; }

    /**
     * @brief Fast helper to write a string at a given terminal position.
     * @param row 1-based row
     * @param col 1-based column
     * @param s The string to write
     * @note This writes directly to stdout and flushes; use sparingly.
     */
    inline static void write_at(Byte row, Byte col, const std::string& s) {
        std::fwrite(save_cursor().c_str(), 1, save_cursor().size(), stdout);
        std::fwrite(set_cursor_pos(row, col).c_str(), 1, set_cursor_pos(row, col).size(), stdout);
        std::fwrite(s.c_str(), 1, s.size(), stdout);
        std::fwrite(restore_cursor().c_str(), 1, restore_cursor().size(), stdout);
        std::fflush(stdout);
    }

    // Render debug text into the current SDL renderer at (x,y).
    // Uses SDL_RenderDebugText when available. Returns true on success.
    static bool draw_debug_text(const std::string& text, int x, int y, int ptsize = 12,
                                Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255);

    /**
     * @brief Query the terminal for the current cursor position (row, col).
     * @param row Out parameter for 1-based row index.
     * @param col Out parameter for 1-based column index.
     * @return true if cursor position was successfully queried; false otherwise.
     * @note On POSIX this uses \x1b[6n (CPR) against /dev/tty and requires the terminal
     * to support the Cursor Position Report sequence. On Windows it queries the console API.
     */
    static bool get_cursor_pos(int& row, int& col);

    /**
     * @brief Returns ANSI escape sequence to clear part of the screen.
     * @details
     * - If n is 0 (or missing), clears from cursor to end of screen.
     * - If n is 1, clears from cursor to beginning of the screen.
     * - If n is 2, clears entire screen.
     * - If n is 3, deletes all lines saved in the scrollback buffer.
     * @param n Mode selector (0-3).
     * @return ANSI escape sequence as a string.
     */
    inline static const std::string erase_in_display(Byte n=0)
    { return "\e[" + std::to_string(n) + "J"; }

    /**
     * @brief Returns ANSI escape sequence to erase part of the current line.
     * @details
     * - If n is 0 (or missing), clears from cursor to end of the line.
     * - If n is 1, clears from cursor to beginning of the line.
     * - If n is 2, clears entire line.
     * @param n Mode selector (0-2).
     * @return ANSI escape sequence as a string.
     */
    inline static const std::string erase_in_line(Byte n=0)
    { return "\e[" + std::to_string(n) + "K"; }
    /** @} */



    /**
     * @name Type Set Constants
     * @brief ANSI escape sequences for text formatting attributes.
     * @{
     */
    inline static const std::string RESET	    = "\e[0m";      ///< ANSI escape sequence to reset formatting
    inline static const std::string NORMAL 		= "\e[0m";      ///< ANSI escape sequence to reset all attributes
    inline static const std::string RETURN   	= "\e[0m\n";    ///< ANSI escape sequence to reset all attributes and add a newline
    inline static const std::string BOLD		= "\e[1m";      ///< ANSI escape sequence to set bold text
    inline static const std::string DIM			= "\e[2m";      ///< ANSI escape sequence to set dim text
    inline static const std::string ITALIC		= "\e[3m";      ///< ANSI escape sequence to set italic text
    inline static const std::string UNDERLINE 	= "\e[4m";      ///< ANSI escape sequence to set underline text
    inline static const std::string BLINKING 	= "\e[5m";      ///< ANSI escape sequence to set blinking text
    inline static const std::string REVERSED 	= "\e[7m";      ///< ANSI escape sequence to set reversed text
    inline static const std::string CONCEALED 	= "\e[8m";      ///< ANSI escape sequence to set concealed text
    inline static const std::string STRIKE	  	= "\e[9m";      ///< ANSI escape sequence to set strikethrough text
    inline static const std::string ERASE_LINE  = "\e[2K\r";    ///< ANSI escape sequence to erase the entire current line and return cursor to start of line
    /** @} */



    /**
     * @name Foreground Color Constants
     * @brief ANSI escape sequences for foreground text colors.
     * @{
     */
    inline static const std::string BLACK		= "\e[0;30m";   ///< ANSI escape sequence for black foreground
    inline static const std::string RED			= "\e[0;31m";   ///< ANSI escape sequence for red foreground
    inline static const std::string GREEN		= "\e[0;32m";   ///< ANSI escape sequence for green foreground
    inline static const std::string BROWN		= "\e[0;33m";   ///< ANSI escape sequence for brown foreground
    inline static const std::string BLUE		= "\e[0;34m";   ///< ANSI escape sequence for blue foreground
    inline static const std::string PURPLE		= "\e[0;35m";   ///< ANSI escape sequence for purple foreground
    inline static const std::string MAGENTA		= "\e[0;35m";   ///< ANSI escape sequence for magenta foreground
    inline static const std::string CYAN		= "\e[0;36m";   ///< ANSI escape sequence for cyan foreground
    inline static const std::string GREY		= "\e[0;37m";   ///< ANSI escape sequence for grey foreground
    inline static const std::string DARK		= "\e[1;30m";   ///< ANSI escape sequence for dark grey foreground
    inline static const std::string ORANGE		= "\e[1;31m";   ///< ANSI escape sequence for orange foreground
    inline static const std::string LT_GRN		= "\e[1;32m";   ///< ANSI escape sequence for light green foreground
    inline static const std::string YELLOW		= "\e[1;33m";   ///< ANSI escape sequence for yellow foreground
    inline static const std::string LT_BLUE		= "\e[1;34m";   ///< ANSI escape sequence for light blue foreground
    inline static const std::string PINK		= "\e[1;35m";   ///< ANSI escape sequence for pink foreground
    inline static const std::string LT_CYAN		= "\e[1;36m";   ///< ANSI escape sequence for light cyan foreground
    inline static const std::string WHITE		= "\e[1;37m";   ///< ANSI escape sequence for white foreground
    inline static const std::string LT_MAGENTA  = fg_rgb(255, 105, 180); ///< ANSI escape sequence for light magenta (hot pink) foreground
    inline static const std::string LT_ORANGE   = fg_rgb(255, 165,  79); ///< ANSI escape sequence for light orange foreground

   /** @} */



    /**
     * @name Background Color Constants
     * @brief ANSI escape sequences for background text colors.
     * @{
     */
    inline static const std::string BG_BLACK	= "\e[0;40m";   ///< ANSI escape sequence for black background
    inline static const std::string BG_RED		= "\e[0;41m";   ///< ANSI escape sequence for red background
    inline static const std::string BG_GREEN	= "\e[0;42m";   ///< ANSI escape sequence for green background
    inline static const std::string BG_BROWN	= "\e[0;43m";   ///< ANSI escape sequence for brown background
    inline static const std::string BG_BLUE	= "\e[0;44m";   ///< ANSI escape sequence for blue background
    inline static const std::string BG_PURPLE	= "\e[0;45m";   ///< ANSI escape sequence for purple background
    inline static const std::string BG_CYAN	= "\e[0;46m";   ///< ANSI escape sequence for cyan background
    inline static const std::string BG_GREY	= "\e[0;47m";   ///< ANSI escape sequence for grey background
    inline static const std::string BG_DEFAULT	= "\e[0;49m";   ///< ANSI escape sequence for default background
    inline static const std::string BG_DARK	= "\e[0;100m";  ///< ANSI escape sequence for dark grey background
    inline static const std::string BG_LT_RED	= "\e[0;101m";  ///< ANSI escape sequence for light red background
    inline static const std::string BG_LT_GREEN	= "\e[0;102m";  ///< ANSI escape sequence for light green background
    inline static const std::string BG_YELLOW	= "\e[0;103m";  ///< ANSI escape sequence for yellow background
    inline static const std::string BG_LT_BLUE	= "\e[0;104m";  ///< ANSI escape sequence for light blue background
    inline static const std::string BG_PINK	= "\e[0;105m";  ///< ANSI escape sequence for pink background
    inline static const std::string BG_LT_CYAN	= "\e[0;106m";  ///< ANSI escape sequence for light cyan background
    inline static const std::string BG_WHITE	= "\e[0;107m";   ///< ANSI escape sequence for white background
    /** @} */


    /**
     * @brief Gets the current terminal size (width and height).
     * @details
     * This function is not called within an output stream. It should be called directly.
     * Example usage:
     * @code
     * int width = 0, height = 0;
     * CLR::get_terminal_size(width, height);
     * std::cout << "width=" << width << ", height=" << height << std::endl;
     * @endcode
     * @note Only works on supported platforms (Windows, Linux, macOS).
     * @param width Reference to store the terminal width.
     * @param height Reference to store the terminal height.
     */
    inline static void get_terminal_size(int& width, int& height) {
        #if defined(_WIN32)
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            width = (int)(csbi.srWindow.Right-csbi.srWindow.Left+1);
            height = (int)(csbi.srWindow.Bottom-csbi.srWindow.Top+1);
        #elif defined(__linux__) || defined(__APPLE__)
            struct winsize w;
            ioctl(fileno(stdout), TIOCGWINSZ, &w);
            width = (int)(w.ws_col);
            height = (int)(w.ws_row);
        #endif // Windows/Linux
    }   


    /**
     * @name Formatting Helper Functions
     * @brief Utility functions for indentation, hex formatting, and padding.
     * @{
     */

    /**
     * @brief Increases indentation level and returns corresponding spaces.
     * @return String of spaces for the new indentation level.
     */
    inline static std::string indent_push() { 
        return std::string(indent_level++ * 2, ' '); 
    }

    /**
     * @brief Decreases indentation level and returns corresponding spaces.
     * @return String of spaces for the new indentation level.
     */
    inline static std::string indent_pop() { 
        return std::string((indent_level-- > 0 ? indent_level : 0) * 2, ' '); 
    }

    /**
     * @brief Returns spaces for the current indentation level.
     * @return String of spaces for the current indentation level.
     */
    inline static std::string indent() { return std::string(indent_level * 2, ' '); }

    /**
     * @brief Converts an integer to a zero-padded hexadecimal string.
     * @param n Number to convert.
     * @param d Number of hex digits.
     * @return Hexadecimal string.
     */
    inline static std::string hex(Uint32 n, Uint8 d)
    {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    };

    /**
     * @brief Pads a string with spaces to a specified length.
     * @param text Input string.
     * @param d Desired length.
     * @return Padded string.
     */
    inline static std::string pad(std::string text, Uint8 d)
    {
        std::string ret = text;
        while (ret.length()<d) {
            ret += " ";
        }
        return ret;
    };
    /** @} */

    /**
     * @brief Expose CLR constants and helper functions to a Lua state.
     * @param lua The lua state to populate with a `CLR` table.
     * @note Implementation lives in src/SDOM_CLR.cpp to avoid duplicate inline
     * definitions across translation units.
     */
    static void exposeToLua(sol::state& lua);

private:
    inline static int indent_level = 0;
        
};