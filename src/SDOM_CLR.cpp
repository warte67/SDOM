#include <SDOM/SDOM.hpp>
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

    // helper functions (map C++ static methods into Lua)
    clr.set_function("indent", &CLR::indent);
    clr.set_function("indent_push", &CLR::indent_push);
    clr.set_function("indent_pop", &CLR::indent_pop);
    clr.set_function("hex", &CLR::hex);
    clr.set_function("pad", &CLR::pad);

    // color output helpers
    clr.set_function("fg", &CLR::fg);
    clr.set_function("bg", &CLR::bg);
    clr.set_function("fg_rgb", &CLR::fg_rgb);
    clr.set_function("fg_color", &CLR::fg_color);
    clr.set_function("bg_rgb", &CLR::bg_rgb);
    clr.set_function("bg_color", &CLR::bg_color);

    // cursor/erase helpers
    clr.set_function("set_cursor_pos", &CLR::set_cursor_pos);
    clr.set_function("erase_in_display", &CLR::erase_in_display);
    clr.set_function("erase_in_line", &CLR::erase_in_line);

    // get_terminal_size returns two values (width, height)
    clr.set_function("get_terminal_size", []() -> std::tuple<int,int> {
        int w = 0, h = 0;
        CLR::get_terminal_size(w, h);
        return std::make_tuple(w, h);
    });

    // cursor position: returns (row, col) or (-1, -1) on failure
    clr.set_function("get_cursor_pos", []() -> std::tuple<int,int> {
        int r = -1, c = -1;
        if (CLR::get_cursor_pos(r, c)) return std::make_tuple(r, c);
        return std::make_tuple(-1, -1);
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
}

    bool CLR::get_cursor_pos(int& row, int& col)
    {
    #if defined(_WIN32)
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) return false;
        row = (int)csbi.dwCursorPosition.Y + 1;
        col = (int)csbi.dwCursorPosition.X + 1;
        return true;
    #elif defined(__linux__) || defined(__APPLE__)
        // Use /dev/tty so we talk to the actual terminal
        int fd = open("/dev/tty", O_RDWR | O_NOCTTY);
        if (fd < 0) return false;

        struct termios oldt, newt;
        if (tcgetattr(fd, &oldt) != 0) { close(fd); return false; }
        newt = oldt;
        // disable canonical mode and echo
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(fd, TCSANOW, &newt);

        // request cursor position report
        const char* req = "\x1b[6n";
        ssize_t w = write(fd, req, 3);
        (void)w;

        // read response: ESC [ row ; col R
        char buf[32];
        ssize_t len = 0;
        fd_set rfds;
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 200000; // 200ms timeout

        while (true) {
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);
            int sel = select(fd+1, &rfds, NULL, NULL, &tv);
            if (sel <= 0) break;
            ssize_t r = read(fd, buf + len, sizeof(buf) - 1 - len);
            if (r <= 0) break;
            len += r;
            if (buf[len-1] == 'R' || len >= (ssize_t)sizeof(buf)-1) break;
        }
        buf[len] = '\0';

        // restore terminal
        tcsetattr(fd, TCSANOW, &oldt);
        close(fd);

        // parse
        if (len <= 0) return false;
        int r = 0, c = 0;
        if (sscanf(buf, "\x1b[%d;%dR", &r, &c) == 2) {
            row = r;
            col = c;
            return true;
        }
        return false;
    #else
        (void)row; (void)col;
        return false;
    #endif
    }

    bool CLR::draw_debug_text(const std::string& text, int x, int y, int ptsize,
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
