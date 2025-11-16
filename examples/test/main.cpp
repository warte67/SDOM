/**
 * @file main.cpp
 * @brief Unified entry point for all SDOM startup modes.
 */

#include <iostream>
#include <string>
#include <vector>
#include "main_variants.hpp"  // variant entry point declarations

// Default variant if user does not specify one
#ifndef MAIN_VARIANT
#define MAIN_VARIANT 2
#endif

// -----------------------------------------------------------------------------
//  Print Help
// -----------------------------------------------------------------------------
static void print_help(const char* exe)
{
    std::cout << "Usage: " << (exe ? exe : "sdom_app") << " [options]\n\n"
              << "SDOM Startup Options:\n"
              << "  --variant N           Select startup variant (1 = Lua, 2 = C++/JSON, 3 = C-API)\n"
              << "  --list-variants       Show all available variants\n\n"
              << "Common Options:\n"
              << "  --help                Show this message and exit\n"
              << "  --version             Print SDOM version information\n"
              << "  --stop_after_tests    Run unit tests then exit\n"
              << "  --lua_file <file>     Lua config (Variant 1)\n"
              << "  --config <file>       Alias for --lua_file\n"
              << "\n"
              << "Example:\n"
              << "  " << exe << " --variant 2\n"
              << std::endl;
}

// -----------------------------------------------------------------------------
//  Print Variants
// -----------------------------------------------------------------------------
static void print_variants()
{
    std::cout <<
        "Available SDOM Startup Variants:\n"
        "  1  Lua-driven mode (uses sol2 and lua/config.lua)\n"
        "  2  C++ front-end + JSON constructors (fast unit-test mode)\n"
        "  3  C API bootstrap (SDOM_Init / SDOM_Configure)\n"
        << std::endl;
}

// -----------------------------------------------------------------------------
//  main()
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    int variant = MAIN_VARIANT;      // default
    bool showHelp = false;
    bool showVersion = false;
    bool showVariants = false;

    // allow passing through arguments to variants
    std::vector<std::string> passthroughArgs;
    passthroughArgs.reserve(argc);

    // -------------------------------------------------------------------------
    // Parse command-line arguments
    // -------------------------------------------------------------------------
    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);

        if (arg == "--help" || arg == "-?" || arg == "/?")
        {
            showHelp = true;
        }
        else if (arg == "--version" || arg == "-v")
        {
            showVersion = true;
        }
        else if (arg == "--variant")
        {
            if (i + 1 < argc)
            {
                variant = std::stoi(argv[++i]);
            }
            else
            {
                std::cerr << "ERROR: Missing value after --variant\n";
                return 1;
            }
        }
        else if (arg == "--list-variants")
        {
            showVariants = true;
        }
        else
        {
            // push all other args through to the variant handler
            passthroughArgs.push_back(arg);
        }
    }

    if (showHelp)
    {
        print_help(argv[0]);
        return 0;
    }

    if (showVariants)
    {
        print_variants();
        return 0;
    }

    if (showVersion)
    {
        // Prefer to let Variant 1 (Lua) handle it, but variant-agnostic print is fine.
        std::cout << "SDOM (version info depends on variant)" << std::endl;
        // The real version printing is in Variant 1 (Lua) already.
        // You could add a Core version query here if needed.
        return 0;
    }

    // -------------------------------------------------------------------------
    // Convert passthroughArgs to a C-style array and dispatch variant
    // -------------------------------------------------------------------------
    std::vector<char*> argvVariant;
    argvVariant.reserve(passthroughArgs.size() + 1);
    argvVariant.push_back(argv[0]); // executable name

    for (auto& s : passthroughArgs)
        argvVariant.push_back(const_cast<char*>(s.c_str()));

    argvVariant.push_back(nullptr);
    int argcVariant = static_cast<int>(argvVariant.size()) - 1;

    // -------------------------------------------------------------------------
    // Dispatch to selected variant
    // -------------------------------------------------------------------------
    switch (variant)
    {
        case 1: return SDOM_main_variant_1(argcVariant, argvVariant.data());
        case 2: return SDOM_main_variant_2(argcVariant, argvVariant.data());
        case 3: return SDOM_main_variant_3(argcVariant, argvVariant.data());
        default:
            std::cerr << "ERROR: Unknown variant " << variant
                      << ". Use --list-variants for help.\n";
            return 1;
    }
}
