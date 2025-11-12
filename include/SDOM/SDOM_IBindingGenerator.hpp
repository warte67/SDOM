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
