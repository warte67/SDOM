#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_CBindingGenerator.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sol/sol.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <thread>
#include <chrono>

using namespace SDOM;

int run_with_timeout(const std::vector<std::string>& argv, int timeout_seconds, const std::string& stderr_path) {
    pid_t pid = fork();
    if (pid == -1) return -1;
    if (pid == 0) {
        int fd = open(stderr_path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
        if (fd >= 0) {
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        std::vector<char*> cargs;
        for (const auto &s : argv)
            cargs.push_back(const_cast<char*>(s.c_str()));
        cargs.push_back(nullptr);
        execvp(cargs[0], cargs.data());
        _exit(127);
    }
    int status = 0;
    using clock = std::chrono::steady_clock;
    auto start = clock::now();
    while (true) {
        pid_t r = waitpid(pid, &status, WNOHANG);
        if (r == pid) {
            if (WIFEXITED(status)) return WEXITSTATUS(status);
            if (WIFSIGNALED(status)) return 128 + WTERMSIG(status);
            return -1;
        }
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(clock::now() - start).count();
        if (elapsed >= timeout_seconds) {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            return 128 + SIGKILL;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main(int argc, char** argv) {
    DataRegistry reg;
    // Populate registry with any known EventType instances so the generator
    // can emit a full SDOM_CAPI_Events.h. EventType static instances are
    // available via EventType::getAll().
    try {
        auto all = SDOM::EventType::getAll();
        for (SDOM::EventType* et : all) {
            if (!et) continue;
            SDOM::TypeInfo ti;
            ti.name = std::string("EventType::") + et->getName();
            ti.cpp_type_id = et->getName();
            try { ti.doc = et->getDoc(); } catch(...) { ti.doc.clear(); }
            // forward per-event category so generators can group entries
            try { ti.category = et->getCategory(); } catch(...) { ti.category.clear(); }

            SDOM::PropertyInfo p;
            p.name = "captures";
            p.cpp_type = "bool";
            p.read_only = false;
            p.doc = "Whether this event type participates in the capture phase.";
            ti.properties.push_back(p);

            p.name = "bubbles";
            p.cpp_type = "bool";
            p.read_only = false;
            p.doc = "Whether this event type bubbles.";
            ti.properties.push_back(p);

            p.name = "target_only";
            p.cpp_type = "bool";
            p.read_only = false;
            p.doc = "If true the event is target-only (no capture/bubble).";
            ti.properties.push_back(p);

            p.name = "global";
            p.cpp_type = "bool";
            p.read_only = false;
            p.doc = "Whether this event type is considered global.";
            ti.properties.push_back(p);

            reg.registerDataType(ti);
        }
    } catch(...) {}

    // TestGenerator
    struct TestGenerator : public IBindingGenerator {
        std::string marker;
        TestGenerator(const std::string &m): marker(m) {}
        bool generate(const DataRegistrySnapshot& snapshot, const std::string& outDir) override {
            try {
                std::filesystem::create_directories(outDir);
                std::filesystem::path p(outDir);
                p /= marker;
                std::ofstream fh(p);
                fh << "generated" << std::endl;
                return true;
            } catch (...) { return false; }
        }
    };

    reg.addGenerator(std::make_unique<TestGenerator>(".dataregistry_marker"));
    reg.addGenerator(std::make_unique<CBindingGenerator>());

    std::string outDir = "build/test_out_dataregistry_standalone";
    // allow overriding output dir via argv[1]
    if (argc > 1) {
        outDir = argv[1];
    }
    try { std::filesystem::remove_all(outDir); } catch(...) {}

    std::cout << "[run_dataregistry_test] using outDir: " << outDir << std::endl;
    bool ok = reg.generateBindings(outDir);
    std::cout << "generateBindings returned: " << ok << std::endl;

    auto hdr = std::filesystem::path(outDir) / "sdom_capi_objects.h";
    auto gen_hdr = std::filesystem::path(outDir) / "sdom_capi_objects_generated.h";
    auto marker = std::filesystem::path(outDir) / ".dataregistry_marker";
    auto cmarker = std::filesystem::path(outDir) / ".c_binding_generator_marker";

    std::cout << "header exists: " << std::filesystem::exists(hdr) << "\n";
    std::cout << "gen header exists: " << std::filesystem::exists(gen_hdr) << "\n";
    std::cout << "marker exists: " << std::filesystem::exists(marker) << "\n";
    std::cout << "cmarker exists: " << std::filesystem::exists(cmarker) << "\n";

    if (std::filesystem::exists(hdr)) {
        std::cout << "About to compile generated header with gcc..." << std::endl;
        std::filesystem::path csrc = outDir;
        csrc /= "test_compile.c";
        std::ofstream sc(csrc);
        sc << "#include \"sdom_capi_objects.h\"\nint main(void) { return 0; }\n";
        sc.close();

        std::vector<std::string> argv = {"gcc", "-std=c11", "-I", outDir, "-c", csrc.string(), "-o", outDir + std::string("/test_compile.o")};
        int rc = run_with_timeout(argv, 10, outDir + std::string("/compile_err.txt"));
        std::cout << "gcc rc=" << rc << std::endl;
        if (rc != 0) {
            std::ifstream ef(outDir + std::string("/compile_err.txt"));
            std::ostringstream ss; ss << ef.rdbuf();
            std::cout << "compile stderr:\n" << ss.str() << std::endl;
        }
    }

    return 0;
}
