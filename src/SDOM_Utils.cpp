// SDOM_Utils.cpp

#include <SDOM/SDOM_Utils.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_CLR.hpp>

namespace SDOM
{

    void quit() { Core::getInstance().onQuit(); }
    void shutdown() { Core::getInstance().quit(); } // alias for quit()
    Core& getCore() { return Core::getInstance(); }
    Factory& getFactory() { return getCore().getFactory(); }
    Stage* getStage() { return getCore().getStage(); }
    DomHandle getStageHandle() { return getFactory().getStageHandle(); }
    SDL_Renderer* getRenderer() { return Core::getInstance().getRenderer(); }
    SDL_Window* getWindow() { return Core::getInstance().getWindow(); }
    SDL_Texture* getTexture() { return Core::getInstance().getTexture(); }  



    // helper function to extract SDL_Color from Lua table
    SDL_Color parseColor(const sol::object& colorObj) {
        SDL_Color color = {255, 255, 255, 255};
        if (colorObj.is<std::string>()) {
            std::string str = colorObj.as<std::string>();
            int r = 255, g = 255, b = 255, a = 255;
            sscanf(str.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a);
            color = { (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a };
        } else if (colorObj.is<sol::table>()) {
            sol::table tbl = colorObj.as<sol::table>();
            if (tbl.size() >= 4) {
                color.r = tbl[1].get_or(255);
                color.g = tbl[2].get_or(255);
                color.b = tbl[3].get_or(255);
                color.a = tbl[4].get_or(255);
            } else {
                color.r = tbl["r"].get_or(255);
                color.g = tbl["g"].get_or(255);
                color.b = tbl["b"].get_or(255);
                color.a = tbl["a"].get_or(255);
            }
        }
        return color;
    }

    // helper function to normalize anchor point strings
    std::string normalizeAnchorString(const std::string& s) {
        static const std::string operators = "-|&+,"; // Allowed operator characters
        static const std::vector<std::string> validWords = {
            "top", "bottom", "middle", "center", "left", "right"
        };

        std::string cleaned;
        for (char c : s) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                cleaned += std::tolower(static_cast<unsigned char>(c));
            }
        }

        size_t opPos = cleaned.find_first_of(operators);
        if (opPos == std::string::npos) {
            return cleaned; // single word anchor
        }
        if (cleaned.find_first_of(operators, opPos + 1) != std::string::npos) {
            WARNING("normalizeAnchorString: Multiple operators found in '" + s + "'");
            return "";
        }

        std::string left = cleaned.substr(0, opPos);
        std::string right = cleaned.substr(opPos + 1);

        auto isValid = [&](const std::string& w) {
            return std::find(validWords.begin(), validWords.end(), w) != validWords.end();
        };
        if (!isValid(left) || !isValid(right)) {
            WARNING("normalizeAnchorString: Invalid anchor keywords in '" + s + "' (" + left + ", " + right + ")");
            return "";
        }

        // Allow any combination if either side is "middle" or "center"
        if (left == "middle" || left == "center" || right == "middle" || right == "center") {
            return left + "_" + right;
        }

        // Disallow strictly horizontal or strictly vertical combinations
        static const std::vector<std::string> vertical = { "top", "bottom" };
        static const std::vector<std::string> horizontal = { "left", "right" };

        bool leftIsVertical = std::find(vertical.begin(), vertical.end(), left) != vertical.end();
        bool rightIsVertical = std::find(vertical.begin(), vertical.end(), right) != vertical.end();
        bool leftIsHorizontal = std::find(horizontal.begin(), horizontal.end(), left) != horizontal.end();
        bool rightIsHorizontal = std::find(horizontal.begin(), horizontal.end(), right) != horizontal.end();

        if ((leftIsVertical && rightIsVertical) || (leftIsHorizontal && rightIsHorizontal)) {
            WARNING("normalizeAnchorString: Invalid anchor combination in '" + s + "' (" + left + ", " + right + ")");
            return "";
        }

        return left + "_" + right;
    } // normalizeAnchorString

    bool validateAnchorPointString(const std::string& anchorString)
    {
        static const std::string operators = "-|&+,"; // Allowed operator characters
        static const std::vector<std::string> validWords = {
            "top", "bottom", "middle", "center", "left", "right"
        };

        std::string cleaned;
        for (char c : anchorString) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                cleaned += std::tolower(static_cast<unsigned char>(c));
            }
        }

        size_t opPos = cleaned.find_first_of(operators);
        if (opPos == std::string::npos) {
            // Single word anchor
            return std::find(validWords.begin(), validWords.end(), cleaned) != validWords.end();
        }
        if (cleaned.find_first_of(operators, opPos + 1) != std::string::npos) {
            return false; // Multiple operators
        }

        std::string left = cleaned.substr(0, opPos);
        std::string right = cleaned.substr(opPos + 1);

        auto isValid = [&](const std::string& w) {
            return std::find(validWords.begin(), validWords.end(), w) != validWords.end();
        };
        if (!isValid(left) || !isValid(right)) {
            return false;
        }

        // Allow any combination if either side is "middle" or "center"
        if (left == "middle" || left == "center" || right == "middle" || right == "center") {
            return true;
        }

        static const std::vector<std::string> vertical = { "top", "bottom" };
        static const std::vector<std::string> horizontal = { "left", "right" };

        bool leftIsVertical = std::find(vertical.begin(), vertical.end(), left) != vertical.end();
        bool rightIsVertical = std::find(vertical.begin(), vertical.end(), right) != vertical.end();
        bool leftIsHorizontal = std::find(horizontal.begin(), horizontal.end(), left) != horizontal.end();
        bool rightIsHorizontal = std::find(horizontal.begin(), horizontal.end(), right) != horizontal.end();

        // Disallow strictly horizontal or strictly vertical combinations
        if ((leftIsVertical && rightIsVertical) || (leftIsHorizontal && rightIsHorizontal)) {
            return false;
        }

        return true;
    } // validateAnchorPointString

    bool validateAnchorAssignments(const sol::table& config)
    {
        const std::vector<std::string> anchorKeys = {
            "anchorTop", "anchorLeft", "anchorBottom", "anchorRight"
        };

        bool allValid = true;
        for (const auto& key : anchorKeys) {
            if (config[key].valid() && config[key].is<std::string>()) {
                std::string anchorStr = config[key].get<std::string>();
                if (!validateAnchorPointString(anchorStr)) {
                    WARNING("Invalid anchor string for " + key + ": '" + anchorStr + "'");
                    allValid = false;
                }
            }
        }
        return allValid;
    }


    void printMessageBox(const std::string& title, const std::string& message, 
                            const std::string& file, int line, 
                            const std::string& colorTitle, 
                            const std::string& colorMsg,
                            const std::string& colorBorder)
    {
        int terminalWidth = 0, terminalHeight = 0;
        CLR::get_terminal_size(terminalWidth, terminalHeight);

        std::string line1 = title + ": " + message;
        std::string line2 = file.empty() ? "" : "File: " + file;
        std::string line3 = (line >= 0) ? "Line: " + std::to_string(line) : "";

        size_t maxWidth = std::max({line1.size(), line2.size(), line3.size()});

        std::cout << std::endl;
        if ((size_t)terminalWidth > maxWidth) {
            std::string border(maxWidth + 2, '-');
            std::cout << colorBorder << "+" << border << "+\n"
                    << colorBorder << "| " << colorTitle << title << ": " << colorMsg << message << std::string(maxWidth - line1.size(), ' ') << colorBorder << " |\n";
            if (!line2.empty())
                std::cout << colorBorder << "| " << colorTitle << "File: " << CLR::YELLOW << file << std::string(maxWidth - line2.size(), ' ') << colorBorder << " |\n";
            if (!line3.empty())
                std::cout << colorBorder << "| " << colorTitle << "Line: " << CLR::YELLOW << line << std::string(maxWidth - line3.size(), ' ') << colorBorder << " |\n";
            std::cout << colorBorder << "+" << border << "+" << CLR::RESET << std::endl;
        } else {
            std::cout << colorTitle << title << ": " << colorMsg << message << CLR::RESET << std::endl;
            if (!line2.empty())
                std::cout << colorTitle << "File: " << CLR::YELLOW << file << CLR::RESET << std::endl;
            if (!line3.empty())
                std::cout << colorTitle << "Line: " << CLR::YELLOW << line << CLR::RESET << std::endl;
        }
    }

    void showWarning(const std::string& message, const std::string& file, int line)
    {
        printMessageBox("Warning", message, file, line, CLR::GREY, CLR::WHITE, CLR::DARK);
    }

} // namespace SDOM