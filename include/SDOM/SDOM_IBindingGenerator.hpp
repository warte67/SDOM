#pragma once
#ifndef __SDOM_IBINDINGGENERATOR_HPP__
#define __SDOM_IBINDINGGENERATOR_HPP__

#include <string>

namespace SDOM {

struct DataRegistrySnapshot;

// Backwards compatibility: some files may still include the concrete
// DataRegistry header; forward declare DataRegistry for those cases.
class DataRegistry;

/**
 * IBindingGenerator
 *
 * Minimal interface for binding generators that can be registered with
 * DataRegistry. Implementations should generate files into `outputDir`
 * based on metadata stored in the supplied registry.
 */
class IBindingGenerator {
public:
    virtual ~IBindingGenerator() = default;

    // Called to generate output. Generators receive an immutable metadata
    // snapshot to avoid re-entrancy and locking issues. Return true on success.
    virtual bool generate(const DataRegistrySnapshot& snapshot, const std::string& outputDir) = 0;
};

} // namespace SDOM

#endif // __SDOM_IBINDINGGENERATOR_HPP__


// #pragma once

// class IBindingGenerator {
// public:
//     virtual ~IBindingGenerator() = default;

//     // ------------------------------------------------------------
//     // Per-language header / interface generator
//     // Each backend (C, Lua, Python, etc.) implements this.
//     // ------------------------------------------------------------
//     virtual bool generateInterface(
//         const DataRegistrySnapshot& snapshot,
//         const std::string& outDir) = 0;

//     // ------------------------------------------------------------
//     // Per-language wrapped-function emitter.
//     // Called by generateImplementation().
//     // ------------------------------------------------------------
//     virtual bool emitWrappedFunction(
//         const FunctionInfo& fn,
//         std::ostream& out) = 0;

//     // ------------------------------------------------------------
//     // MASTER GENERATOR (FINAL)
//     // Non-overridable
//     //
//     // 1. generateInterface()
//     // 2. generateImplementation()
//     // ------------------------------------------------------------
//     bool generate(
//         const DataRegistrySnapshot& snapshot,
//         const std::string& outDir
//     ) final
//     {
//         // 1. Interface/header generation
//         if (!generateInterface(snapshot, outDir))
//             return false;

//         // 2. Shared implementation generation
//         return generateImplementation(snapshot, outDir);
//     }

//     // ------------------------------------------------------------
//     // Accessors for filenames and object type
//     // ------------------------------------------------------------
//     const std::string& getObjectType() const { return objectType; }
//     const std::string& getFileHeader() const { return header_filename; }
//     const std::string& getFileImpl() const { return impl_filename; }

// protected:
//     // The type group this generator handles (ex: "Event", "SpriteSheet")
//     std::string objectType;

//     // Filenames this generator writes
//     std::string header_filename;
//     std::string impl_filename;

//     // ------------------------------------------------------------
//     // Shared implementation generator (NON-VIRTUAL)
//     // This loops DataRegistry types matching objectType and
//     // calls emitWrappedFunction() for each FunctionInfo.
//     // ------------------------------------------------------------
//     bool generateImplementation(
//         const DataRegistrySnapshot& snapshot,
//         const std::string& outDir)
//     {
//         // Compute output path
//         std::string implPath = computeImplementationFilename(outDir);
//         std::ofstream ofs(implPath);
//         if (!ofs)
//             return false;

//         writeImplPreamble(ofs);

//         // Loop all types in the snapshot
//         for (const auto& t : snapshot.types) {
//             if (t.category != objectType)
//                 continue;

//             for (const auto& fn : t.functions) {
//                 if (!emitWrappedFunction(fn, ofs))
//                     return false;
//             }
//         }

//         writeImplPostamble(ofs);
//         return true;
//     }

//     // ------------------------------------------------------------
//     // Shared helpers
//     // ------------------------------------------------------------
//     void writeImplPreamble(std::ostream& out)
//     {
//         out << "/* Auto-generated implementation for "
//             << objectType << " */\n\n";
//     }

//     void writeImplPostamble(std::ostream& out)
//     {
//         out << "\n/* End of generated file */\n";
//     }

//     std::string computeImplementationFilename(const std::string& outDir)
//     {
//         return outDir + "/" + impl_filename;
//     }
// };
