#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_CLR.hpp>
#if defined(__linux__) || defined(__APPLE__)
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#endif

// Expose CLR constants and helper functions into the embedded Lua state.
void CLR::exposeToLua(sol::state& lua)
{
    using std::string;
    using std::tuple;
    sol::table clr = lua.create_table();

    // color/style constants
    clr["RESET"] = CLR::RESET;
    clr["NORMAL"] = CLR::NORMAL;
    clr["BOLD"] = CLR::BOLD;
    clr["BLUE"] = CLR::BLUE;
    clr["LT_BLUE"] = CLR::LT_BLUE;
    clr["CYAN"] = CLR::CYAN;
    clr["LT_CYAN"] = CLR::LT_CYAN;
    clr["GREEN"] = CLR::GREEN;
    clr["LT_GRN"] = CLR::LT_GRN;
    clr["YELLOW"] = CLR::YELLOW;
    clr["RED"] = CLR::RED;
    clr["WHITE"] = CLR::WHITE;
    clr["ORANGE"] = CLR::ORANGE;
    clr["GREY"] = CLR::GREY;
    clr["DARK"] = CLR::DARK;
    clr["PINK"] = CLR::PINK;

    // additional formatting/control constants
    clr["RETURN"] = CLR::RETURN;
    clr["DIM"] = CLR::DIM;
    clr["ITALIC"] = CLR::ITALIC;
    clr["UNDERLINE"] = CLR::UNDERLINE;
    clr["BLINKING"] = CLR::BLINKING;
    clr["REVERSED"] = CLR::REVERSED;
    clr["CONCEALED"] = CLR::CONCEALED;
    clr["STRIKE"] = CLR::STRIKE;
    clr["ERASE_LINE"] = CLR::ERASE_LINE;

    // additional foreground color constants
    clr["BLACK"] = CLR::BLACK;
    clr["BROWN"] = CLR::BROWN;
    clr["PURPLE"] = CLR::PURPLE;
    clr["MAGENTA"] = CLR::MAGENTA;

    // background color constants
    clr["BG_BLACK"] = CLR::BG_BLACK;
    clr["BG_RED"] = CLR::BG_RED;
    clr["BG_GREEN"] = CLR::BG_GREEN;
    clr["BG_BROWN"] = CLR::BG_BROWN;
    clr["BG_BLUE"] = CLR::BG_BLUE;
    clr["BG_PURPLE"] = CLR::BG_PURPLE;
    clr["BG_CYAN"] = CLR::BG_CYAN;
    clr["BG_GREY"] = CLR::BG_GREY;
    clr["BG_DEFAULT"] = CLR::BG_DEFAULT;
    clr["BG_DARK"] = CLR::BG_DARK;
    clr["BG_LT_RED"] = CLR::BG_LT_RED;
    clr["BG_LT_GREEN"] = CLR::BG_LT_GREEN;
    clr["BG_YELLOW"] = CLR::BG_YELLOW;
    clr["BG_LT_BLUE"] = CLR::BG_LT_BLUE;
    clr["BG_PINK"] = CLR::BG_PINK;
    clr["BG_LT_CYAN"] = CLR::BG_LT_CYAN;
    clr["BG_WHITE"] = CLR::BG_WHITE;

    // clr["ARROW_UP"]            = ARROW_UP;
    // clr["ARROW_DOWN"]          = ARROW_DOWN;
    // clr["ARROW_LEFT"]          = ARROW_LEFT;
    // clr["ARROW_RIGHT"]         = ARROW_RIGHT;
    // clr["ARROW_UP_DOUBLE"]     = ARROW_UP_DOUBLE;
    // clr["ARROW_DOWN_DOUBLE"]   = ARROW_DOWN_DOUBLE;
    // clr["ARROW_LEFT_DOUBLE"]   = ARROW_LEFT_DOUBLE;
    // clr["ARROW_RIGHT_DOUBLE"]  = ARROW_RIGHT_DOUBLE;
    // clr["ARROW_THIN_RIGHT"]    = ARROW_THIN_RIGHT;
    // clr["ARROW_COLLAPSED"]     = ARROW_COLLAPSED;
    // clr["ARROW_EXPANDED"]      = ARROW_EXPANDED;

    // clr["CHECKBOX_EMPTY"]      = CHECKBOX_EMPTY;
    // clr["CHECKBOX_CHECKED"]    = CHECKBOX_CHECKED;
    // clr["CHECKBOX_CROSSED"]    = CHECKBOX_CROSSED;
    // clr["CHECKBOX_MIXED"]      = CHECKBOX_MIXED;
    // clr["CHECKBOX_FLAT"]       = CHECKBOX_FLAT;

    // clr["RADIO_OFF"]           = RADIO_OFF;
    // clr["RADIO_ON"]            = RADIO_ON;
    // clr["RADIO_DISABLED"]      = RADIO_DISABLED;

    // clr["TRACK_H"]             = TRACK_H;
    // clr["TRACK_V"]             = TRACK_V;
    // clr["KNOB_BLOCK"]          = KNOB_BLOCK;
    // clr["KNOB_ROUND"]          = KNOB_ROUND;

    // clr["SHADE_LIGHT"]         = SHADE_LIGHT;
    // clr["SHADE_MEDIUM"]        = SHADE_MEDIUM;
    // clr["SHADE_DARK"]          = SHADE_DARK;
    // clr["SHADE_SOLID"]         = SHADE_SOLID;

    // clr["BOX_TL"]              = BOX_TL;
    // clr["BOX_TR"]              = BOX_TR;
    // clr["BOX_BL"]              = BOX_BL;
    // clr["BOX_BR"]              = BOX_BR;
    // clr["BOX_H"]               = BOX_H;
    // clr["BOX_V"]               = BOX_V;
    // clr["BOX_T"]               = BOX_T;
    // clr["BOX_B"]               = BOX_B;
    // clr["BOX_L"]               = BOX_L;
    // clr["BOX_R"]               = BOX_R;
    // clr["BOX_C"]               = BOX_C;


    // helper functions (map C++ static methods into Lua)
    clr.set_function("indent", &CLR::indent);
    clr.set_function("indent_push", &CLR::indent_push);
    clr.set_function("indent_pop", &CLR::indent_pop);
    clr.set_function("hex", &CLR::hex);             // ✅ Tested in Lua
    clr.set_function("pad", &CLR::pad);             // ✅ Tested in Lua

    // color output helpers
    clr.set_function("fg", &CLR::fg);               // ✅ Tested in Lua
    clr.set_function("bg", &CLR::bg);               // ✅ Tested in Lua
    clr.set_function("rgb", &CLR::fg_rgb);          // ✅ Tested in Lua
    clr.set_function("fg_rgb", &CLR::fg_rgb);       // ✅ Tested in Lua
    clr.set_function("fg_color", &CLR::fg_color);   // ✅ Tested in Lua
    clr.set_function("bg_rgb", &CLR::bg_rgb);       // ✅ Tested in Lua
    clr.set_function("bg_color", &CLR::bg_color);   // ✅ Tested in Lua

    // cursor/erase helpers
    clr.set_function("set_cursor_pos", &CLR::set_cursor_pos);
    clr.set_function("erase_in_display", &CLR::erase_in_display);
    clr.set_function("erase_in_line", &CLR::erase_in_line);

    clr.set_function("get_terminal_size", []() -> sol::table {
        int w = 0, h = 0;
        CLR::get_terminal_size(w, h);
        sol::state_view lua = SDOM::Core::getInstance().getLua();
        sol::table t = lua.create_table();
        t["width"]  = w;
        t["w"]      = w;
        t["height"] = h;
        t["h"]      = h;
        return t;
    });

    clr.set_function("get_cursor_pos", []() -> sol::table {
        sol::state_view lua = SDOM::Core::getInstance().getLua();
        int row = -1, col = -1;
        if (!CLR::get_cursor_pos(row, col)) {
            // Fallback when TTY or cursor detection isn’t available
            return lua.create_table_with(
                "row", -1,
                "col", -1,
                "x", -1,
                "y", -1
            );
        }
        // Return as both (row,col) and (x,y) for convenience
        return lua.create_table_with(
            "row", row,
            "col", col,
            "x", col,
            "y", row
        );
    });    


    clr.set_function("is_tty", []() {
    #if defined(_WIN32)
        return _isatty(_fileno(stdout));
    #else
        return isatty(STDOUT_FILENO);
    #endif
    });    

    // Fast save/restore helpers and write_at for quick terminal updates
    clr.set_function("save_cursor", [](){ return CLR::save_cursor(); });
    clr.set_function("restore_cursor", [](){ return CLR::restore_cursor(); });
    clr.set_function("write_at", [](int row, int col, const std::string& s){ CLR::write_at(static_cast<unsigned char>(row), static_cast<unsigned char>(col), s); });

    // draw_debug_text: render debug text into the SDL renderer at (x,y)
    clr.set_function("draw_debug_text", [](const std::string& text, int x, int y, int ptsize, int r, int g, int b, int a) {
        return CLR::draw_debug_text(text, x, y, ptsize, static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a));
    });

    // publish into lua
    lua["CLR"] = clr;
} // END CLR::exposeToLua()

bool CLR::get_cursor_pos(int& row, int& col)
{
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return false;
    row = static_cast<int>(csbi.dwCursorPosition.Y) + 1;
    col = static_cast<int>(csbi.dwCursorPosition.X) + 1;
    return true;

#elif defined(__linux__) || defined(__APPLE__)

    // Prefer stdin if it’s a TTY, fallback to /dev/tty
    int fd = isatty(STDIN_FILENO) ? STDIN_FILENO : open("/dev/tty", O_RDWR | O_NOCTTY);
    if (fd < 0)
        return false;

    struct termios oldt{};
    if (tcgetattr(fd, &oldt) != 0) {
        if (fd != STDIN_FILENO) close(fd);
        return false;
    }

    struct termios newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(fd, TCSANOW, &newt);

    // Make sure any pending color output is flushed
    fflush(stdout);

    // Ask terminal for cursor position
    const char* req = "\x1b[6n";
    if (write(fd, req, 3) < 0) {
        tcsetattr(fd, TCSANOW, &oldt);
        if (fd != STDIN_FILENO) close(fd);
        return false;
    }

    // Read response: ESC [ row ; col R
    char buf[64] = {0};
    ssize_t len = 0;
    fd_set rfds;
    struct timeval tv{0, 200000}; // 200ms timeout

    while (true) {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        int sel = select(fd + 1, &rfds, nullptr, nullptr, &tv);
        if (sel <= 0) break;
        ssize_t r = read(fd, buf + len, sizeof(buf) - 1 - len);
        if (r < 0 && errno == EINTR) continue;
        if (r <= 0) break;
        len += r;
        if (buf[len - 1] == 'R' || len >= (ssize_t)sizeof(buf) - 1)
            break;
    }

    // Restore terminal and clean up
    tcsetattr(fd, TCSANOW, &oldt);
    if (fd != STDIN_FILENO) close(fd);

    // Parse the response
    int r = 0, c = 0;
    if (len > 0 && sscanf(buf, "\x1b[%d;%dR", &r, &c) == 2) {
        row = r;
        col = c;
        return true;
    }

    // If we get here, failed to parse or timed out
    row = col = -1;
    return false;

#else
    (void)row; (void)col;
    return false;
#endif
} // END CLR::get_cursor_pos()


bool CLR::draw_debug_text(const std::string& text, int x, int y, int /*ptsize*/,
                            Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    // Use Core's renderer if available. Prefer SDL3's built-in debug text renderer
    // when available via SDL_RenderDebugText(). This avoids pulling in SDL_ttf
    // and matching platform font paths. That comes later with the IResourceObjects.
    try {
        SDOM::Core& core = SDOM::Core::getInstance();
        SDL_Renderer* renderer = core.getRenderer();
        if (!renderer) return false;
        if (!SDL_SetRenderDrawColor(renderer, r, g, b, a)) return false;

        // SDL3 provides a small debug text rendering helper on many builds. Call it
        // directly to draw UTF-8 text at a float position. If the symbol is not
        // present in the linked SDL library this will fail at link time; the project
        // is configured to use SDL3 on target systems where this helper exists.
        if(!SDL_RenderDebugText(renderer, static_cast<float>(x), static_cast<float>(y), text.c_str())) {
            ERROR("SDL_RenderDebugText failed");
            return false;
        }
        return true;
    } catch (...) {
        return false;
    }
}
