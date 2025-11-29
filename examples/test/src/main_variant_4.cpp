#include <iostream>
#include <string>

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>

#include "Box.hpp"
#include "UnitTests.hpp"
using namespace SDOM;

namespace
{
    struct Variant4Options
    {
        bool stopAfterTests = false;
        bool showHelp = false;
        std::string configPath = "json/config.json";
    };

    void print_variant4_help(const char* exe)
    {
        std::cout
            << "Usage: " << (exe ? exe : "prog") << " --variant 4 [options]\n"
            << "Options:\n"
            << "  --config <file>         Path to JSON config (default: json/config.json)\n"
            << "  --stop_after_tests      Run unit tests then exit\n"
            << "  --stop-after-tests      Alias for --stop_after_tests\n"
            << "  --help                  Show this help message\n"
            << std::endl;
    }

    Variant4Options parse_options(int argc, char** argv)
    {
        Variant4Options opts;
        for (int i = 1; i < argc; ++i)
        {
            if (!argv[i]) continue;
            const std::string arg = argv[i];
            if (arg == "--help" || arg == "-?" || arg == "/?")
            {
                opts.showHelp = true;
                break;
            }
            else if ((arg == "--config" || arg == "--json") && i + 1 < argc && argv[i + 1])
            {
                opts.configPath = argv[++i];
            }
            else if (arg == "--stop_after_tests" || arg == "--stop-after-tests")
            {
                opts.stopAfterTests = true;
            }
        }
        return opts;
    }

    void register_custom_types(Core& core)
    {
        core.getFactory().registerDisplayObjectType("Box", TypeCreators{
            Box::CreateFromInitStruct,
            Box::CreateFromJson
        });
    }
}

extern bool Main_UnitTests();

int SDOM_main_variant_4(int argc, char** argv)
{
    Variant4Options opts = parse_options(argc, argv);
    if (opts.showHelp)
    {
        print_variant4_help(argv[0]);
        return 0;
    }

    Core& core = getCore();
    core.setStopAfterUnitTests(opts.stopAfterTests);
    core.registerOnUnitTest(Main_UnitTests);

    register_custom_types(core);
    if (!core.loadProjectFromJsonFile(opts.configPath))
    {
        std::cerr << "[Variant 4] Failed to load config '" << opts.configPath << "'" << std::endl;
        return 1;
    }

    bool ok = core.run();
    return ok ? 0 : 1;
}
