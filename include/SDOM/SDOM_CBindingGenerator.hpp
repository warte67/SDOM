#pragma once
#ifndef __SDOM_C_BINDING_GENERATOR_HPP__
#define __SDOM_C_BINDING_GENERATOR_HPP__

#include <SDOM/SDOM_IBindingGenerator.hpp>
#include <string>

namespace SDOM {

class CBindingGenerator : public IBindingGenerator {
public:
    CBindingGenerator() = default;
    ~CBindingGenerator() override = default;

    // Generate a modest, conservative C header into outputDir. This is a
    // minimal concrete generator to demonstrate the generator plumbing.
    bool generate(const DataRegistrySnapshot& snapshot, const std::string& outputDir) override;
    // Generate event-specific helper outputs (event bindings header)
    bool generateEventBindings(const DataRegistrySnapshot& snapshot, const std::string& outputDir);
    bool emitCAPIEventsHeader(const DataRegistrySnapshot& snapshot, const std::string& outputDir);
};

} // namespace SDOM

#endif // __SDOM_C_BINDING_GENERATOR_HPP__
