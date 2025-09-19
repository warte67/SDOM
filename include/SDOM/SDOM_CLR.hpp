/**
 * @file SDOM_CLR.hpp
 * @brief Static container for ANSI escape sequences for terminal text formatting and colorization.
 * @details
 *      _____ _      _____        _                 
 *     / ____| |    |  __ \      | |                
 *    | |    | |    | |__) |     | |__  _ __  _ __  
 *    | |    | |    |  _  /      | '_ \| '_ \| '_ \ 
 *    | |____| |____| | \ \   _  | | | | |_) | |_) |
 *     \_____|______|_|  \_\ (_) |_| |_| .__/| .__/ 
 *                                     | |   | |    
 *                                     |_|   |_|    
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

    //////////////////////////////////
    // UTILITY MEMBER FUNCTIONS:
    //////////////////////////////////

    // Returns ANSI escape to change 8-bit foreground color
    inline static const std::string fg(Byte color) 
    { return "\e[38;5;" + std::to_string(color) + "m"; } 	

    // Returns ANSI escape to change 8-bit background color
    inline static const std::string bg(Byte color) 
    { return "\e[48;5;" + std::to_string(color) + "m"; } 	

    // Returns ANSI escape to change 12-bit foreground RGB color
	inline static const std::string fg_rgb(Byte r, Byte g, Byte b) 
    { return "\e[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m"; }
	
    // Returns ANSI escape to change 12-bit background RGB color
	inline static const std::string bg_rgb(Byte r, Byte g, Byte b) 
    { return "\e[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m"; }

    // Moves the cursor to row n, column m. The values are 1-based.
	inline static const std::string csr_pos(Byte row=1, Byte col=1) 
    { return "\e[" + std::to_string(row) + ";" + std::to_string(col) + "H"; }

    // Clears part of the screen. 
    //      If n is 0 (or missing), clear from cursor to end of screen. 
    //      If n is 1, clear from cursor to beginning of the screen. 
    //      If n is 2, clear entire screen.
    //      If n is 3, delete all lines saved in the scrollback buffer
    inline static const std::string erase_in_display(Byte n=0)
    { return "\e[" + std::to_string(n) + "J"; }

    // Erases part of the line. Cursor position does not change. 
    //      If n is 0 (or missing), clear from cursor to the end of the line. 
    //      If n is 1, clear from cursor to beginning of the line. 
    //      If n is 2, clear entire line. 
    inline static const std::string erase_in_line(Byte n=0)
    { return "\e[" + std::to_string(n) + "K"; }


    //////////////////////////////////
    // TYPE SET CONSTANTS:
    //////////////////////////////////

   	inline static const std::string RESET	    = "\e[0m";
 	inline static const std::string NORMAL 		= "\e[0m";
	inline static const std::string RETURN  	= "\e[0m\n";
	inline static const std::string BOLD		= "\e[1m";
	inline static const std::string DIM			= "\e[2m";
	inline static const std::string ITALIC		= "\e[3m";
	inline static const std::string UNDERLINE 	= "\e[4m";
	inline static const std::string BLINKING 	= "\e[5m";
	inline static const std::string REVERSED 	= "\e[7m";
	inline static const std::string CONCEALED 	= "\e[8m";
	inline static const std::string STRIKE	 	= "\e[9m";
    inline static const std::string ERASE_LINE  = "\e[2K\r";


    //////////////////////////////////
	// FOREGROUND COLOR CONSTANTS
    //////////////////////////////////

	inline static const std::string BLACK		= "\e[0;30m";		
	inline static const std::string RED			= "\e[0;31m";		
	inline static const std::string GREEN		= "\e[0;32m";		
	inline static const std::string BROWN		= "\e[0;33m";		
	inline static const std::string BLUE		= "\e[0;34m";		
	inline static const std::string PURPLE		= "\e[0;35m";		
	inline static const std::string CYAN		= "\e[0;36m";		
	inline static const std::string GREY		= "\e[0;37m";		
	inline static const std::string DARK		= "\e[1;30m";		
	inline static const std::string ORANGE		= "\e[1;31m";		
	inline static const std::string LT_GRN		= "\e[1;32m";		
	inline static const std::string YELLOW		= "\e[1;33m";		
	inline static const std::string LT_BLUE		= "\e[1;34m";		
	inline static const std::string PINK		= "\e[1;35m";		
	inline static const std::string LT_CYAN		= "\e[1;36m";		
	inline static const std::string WHITE		= "\e[1;37m";		


    //////////////////////////////////
	// BACKGROUND COLOR CONSTANTS
    //////////////////////////////////

	inline static const std::string BG_BLACK	= "\e[0;40m";		
	inline static const std::string BG_RED		= "\e[0;41m";		
	inline static const std::string BG_GREEN	= "\e[0;42m";		
	inline static const std::string BG_BROWN	= "\e[0;43m";		
	inline static const std::string BG_BLUE		= "\e[0;44m";		
	inline static const std::string BG_PURPLE	= "\e[0;45m";		
	inline static const std::string BG_CYAN		= "\e[0;46m";		
	inline static const std::string BG_GREY		= "\e[0;47m";		
	inline static const std::string BG_DEFAULT	= "\e[0;49m";		
	inline static const std::string BG_DARK		= "\e[0;100m";		
	inline static const std::string BG_LT_RED	= "\e[0;101m";		
	inline static const std::string BG_LT_GREEN	= "\e[0;102m";		
	inline static const std::string BG_YELLOW	= "\e[0;103m";		
	inline static const std::string BG_LT_BLUE	= "\e[0;104m";		
	inline static const std::string BG_PINK		= "\e[0;105m";		
	inline static const std::string BG_LT_CYAN	= "\e[0;106m";		
	inline static const std::string BG_WHITE	= "\e[0;107m";	


    // Unlike the other methods in this class, this one is not called
    // within an output stream. It is to be called directly.
    //      Usage:
    //          int width = 0, height = 0;
    //          clr::get_terminal_size(width, height);
    //          std::cout << "width=" << width << ", height=" << height << std::endl;
    //
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

private:
    inline static int indent_level = 0;

public:
    inline static std::string indent_push() { 
        return std::string(indent_level++ * 2, ' '); 
    }    
    inline static std::string indent_pop() { 
        return std::string((indent_level-- > 0 ? indent_level : 0) * 2, ' '); 
    }
    inline static std::string indent() { return std::string(indent_level * 2, ' '); }

    inline static std::string hex(Uint32 n, Uint8 d)
    {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    };
    inline static std::string pad(std::string text, Uint8 d)
    {
        std::string ret = text;
        while (ret.length()<d) {
            ret += " ";
        }
        return ret;
    };
};