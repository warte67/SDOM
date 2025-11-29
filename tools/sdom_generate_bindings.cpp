#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_CAPI_BindGenerator.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>

namespace {

struct GeneratorConfig {
    std::filesystem::path headerDir;
    std::filesystem::path sourceDir;
    bool verbose = false;
};

GeneratorConfig parseArgs(int argc, char** argv)
{
    GeneratorConfig cfg;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if ((arg == "--header-dir" || arg == "-H") && i + 1 < argc) {
            cfg.headerDir = std::filesystem::path(argv[++i]);
        } else if ((arg == "--source-dir" || arg == "-S") && i + 1 < argc) {
            cfg.sourceDir = std::filesystem::path(argv[++i]);
        } else if (arg == "--verbose" || arg == "-v") {
            cfg.verbose = true;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: sdom_generate_bindings [options]\n"
                      << "  -H, --header-dir <dir>  Output directory for generated headers\n"
                      << "  -S, --source-dir <dir>  Output directory for generated sources\n"
                      << "  -v, --verbose           Enable verbose generator logging\n";
            std::exit(EXIT_SUCCESS);
        }
    }

    if (cfg.headerDir.empty() || cfg.sourceDir.empty()) {
        throw std::runtime_error("sdom_generate_bindings requires --header-dir and --source-dir arguments");
    }

    return cfg;
}

} // namespace

int main(int argc, char** argv)
{
    try {
        const auto cfg = parseArgs(argc, argv);

    #if defined(_WIN32)
        _putenv_s("SDOM_GENERATE_BINDINGS", "1");
    #else
        setenv("SDOM_GENERATE_BINDINGS", "1", 1);
    #endif

        // Ensure output directories exist before running the generator.
        std::filesystem::create_directories(cfg.headerDir);
        std::filesystem::create_directories(cfg.sourceDir);

        SDOM::Core& core = SDOM::Core::getInstance();
        SDOM::Factory& factory = core.getFactory();

        if (!factory.isInitialized()) {
            factory.startup();
        }

        SDOM::DataRegistry& registry = factory.getDataRegistry();

        SDOM::CAPI_BindGenerator generator;
        generator.setOutputDirs(cfg.headerDir.string(), cfg.sourceDir.string());

        if (cfg.verbose) {
            std::cout << "[sdom_generate_bindings] Writing outputs to\n"
                      << "  Headers: " << cfg.headerDir << '\n'
                      << "  Sources: " << cfg.sourceDir << std::endl;
        }

        const auto mergedTypes = registry.getMergedTypes();
        if (!generator.generate(mergedTypes, registry)) {
            std::cerr << "[sdom_generate_bindings] Generation failed" << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "[sdom_generate_bindings] Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
