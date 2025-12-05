/***  SDOM_DataRegistry.cpp  ****************************
 *    ___ ___   ___  __  __   ___             _                  
 *   / __|   \ / _ \|  \/  | | __|_ _____ _ _| |_   __ _ __ _ __ 
 *   \__ \ |) | (_) | |\/| | | _|\ V / -_) ' \  _|_/ _| '_ \ '_ \
 *   |___/___/ \___/|_|  |_|_|___|\_/\___|_||_\__(_)__| .__/ .__/
 *                        |___|                       |_|  |_|     
 *
 *  Central reflection registry for SDOM.
 *  Stores TypeInfo entries (objects, structs, enums, globals, aliases)
 *  and drives binding generators (C API, Lua, etc.).
 *
 *  Also owns the generic runtime dispatcher (SDOM::CAPI) used by
 *  generated C API wrappers (CallArg / CallResult).
 *
 *  Released under the ZLIB License.
 ************************************************************/

#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_IBindGenerator.hpp>

#include <utility>      // std::move
#include <stdexcept>    // std::runtime_error
#include <iostream>     // std::cout

namespace SDOM {

    // ---------------------------------------------------------
    // DataRegistry: lifecycle / singleton
    // ---------------------------------------------------------

    DataRegistry::DataRegistry() = default;
    DataRegistry::~DataRegistry() = default;

    DataRegistry& DataRegistry::instance()
    {
        static DataRegistry g_instance;
        return g_instance;
    }

    // ---------------------------------------------------------
    // Registration API
    // ---------------------------------------------------------

    bool DataRegistry::registerType(const TypeInfo& info)
    {
        std::scoped_lock lk(mutex_);

        auto it = entries_.find(info.name);
        if (it != entries_.end()) {
            // already registered with that name
            return false;
        }

        TypeInfo copy = info;
        if (copy.module_brief.empty() && !copy.doc.empty()) {
            copy.module_brief = copy.doc;
            copy.module_brief_explicit = false;
        }

        entries_.emplace(copy.name, std::move(copy));
        return true;
    }

    void DataRegistry::registerFunction(const std::string& typeName,
                                        const FunctionInfo& meta)
    {
        std::scoped_lock lk(mutex_);
        entries_[typeName].functions.push_back(meta);
    }

    void DataRegistry::registerProperty(const std::string& typeName,
                                        const PropertyInfo& meta)
    {
        std::scoped_lock lk(mutex_);
        entries_[typeName].properties.push_back(meta);
    }

    // ---------------------------------------------------------
    // Lookup / Queries
    // ---------------------------------------------------------

    const TypeInfo* DataRegistry::lookupType(const std::string& name) const
    {
        std::scoped_lock lk(mutex_);
        auto it = entries_.find(name);
        if (it == entries_.end())
            return nullptr;
        return &it->second;
    }

    TypeInfo& DataRegistry::getMutable(const std::string& name)
    {
        std::scoped_lock lk(mutex_);
        auto it = entries_.find(name);
        if (it == entries_.end()) {
            throw std::runtime_error("DataRegistry::getMutable missing entry: " + name);
        }
        return it->second;
    }

    std::vector<std::string> DataRegistry::listTypes() const
    {
        std::scoped_lock lk(mutex_);
        std::vector<std::string> out;
        out.reserve(entries_.size());
        for (const auto& kv : entries_) {
            out.push_back(kv.first);
        }
        return out;
    }

    // ---------------------------------------------------------
    // Binding Generation
    // ---------------------------------------------------------

    bool DataRegistry::generateBindings()
    {
        struct GeneratorTask {
            IBindGenerator* generator = nullptr;
            std::string header_dir;
            std::string source_dir;
            bool verbose = false;
        };

        std::vector<GeneratorTask> tasks;

        {
            std::scoped_lock lk(mutex_);
            tasks.reserve(generators_.size());
            for (auto& cfg : generators_)
            {
                tasks.push_back(GeneratorTask{
                    cfg.generator.get(),
                    cfg.header_output_dir,
                    cfg.source_output_dir,
                    cfg.verbose
                });
            }
        }

        for (auto& task : tasks)
        {
            if (!task.generator)
                continue;

            task.generator->setOutputDirs(
                task.header_dir,
                task.source_dir
            );

            if (task.verbose)
                std::cout << "[DataRegistry] Running generator: "
                          << task.generator->name() << "\n";

            auto merged = getMergedTypes();
            task.generator->generate(merged, *this);
        }

        return true;
    }

    void DataRegistry::addGenerator(std::unique_ptr<IBindGenerator> gen,
                    std::string headerDir, std::string sourceDir, bool overwrite, bool verbose)
    {
        std::scoped_lock lk(mutex_);
        generators_.push_back( GeneratorConfig {
            .generator = std::move(gen),
            .header_output_dir = std::move(headerDir),
            .source_output_dir = std::move(sourceDir),
            .overwrite = overwrite,
            .verbose = verbose
        });
    }
    
    std::unordered_map<std::string, TypeInfo> DataRegistry::getMergedTypes() const
    {
        std::scoped_lock lk(mutex_);
        return entries_;   // simple copy of the unified map
    }

    void DataRegistry::setModuleBrief(const std::string& typeName,
                                      std::string brief,
                                      bool explicitBrief)
    {
        if (brief.empty()) {
            return;
        }

        std::scoped_lock lk(mutex_);
        auto it = entries_.find(typeName);
        if (it == entries_.end()) {
            return;
        }

        it->second.module_brief = std::move(brief);
        it->second.module_brief_explicit = explicitBrief;
    }


    } // namespace SDOM

    // ===========================================================================
    // SDOM::CAPI dispatcher: global registry of GenericCallable
    // ===========================================================================

    namespace SDOM { namespace CAPI {

    namespace {
        std::unordered_map<std::string, GenericCallable>& callableMap()
        {
            static std::unordered_map<std::string, GenericCallable> map;
            return map;
        }

        std::mutex& callableMutex()
        {
            static std::mutex mtx;
            return mtx;
        }
    }

    bool registerCallable(const std::string& name, GenericCallable fn)
    {
        if (!fn) return false;
        std::scoped_lock lk(callableMutex());
        callableMap()[name] = std::move(fn);
        return true;
    }

    std::optional<GenericCallable> lookupCallable(const std::string& name)
    {
        std::scoped_lock lk(callableMutex());
        auto& map = callableMap();
        auto it = map.find(name);
        if (it == map.end())
            return std::nullopt;
        return it->second;
    }

    CallResult invokeCallable(const std::string& name, const std::vector<CallArg>& args)
    {
        auto opt = lookupCallable(name);
        if (!opt.has_value()) {
            // Unknown callable: return Void (caller decides what to do).
            return CallResult::Void();
        }

        const auto& fn = *opt;
        try {
            return fn(args);
        } catch (...) {
            // For now we swallow exceptions and expose a null CallResult.
            // If you want, we can later add an "error" channel here.
            return CallResult::Void();
        }
    }

}} // namespace SDOM::CAPI
