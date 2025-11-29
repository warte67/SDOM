#pragma once

#include <cstdint>
#include <string>

#include <SDOM/CAPI/SDOM_CAPI_Core.h>

namespace SDOM {

class Core;

namespace CoreAPI {

// Runtime helpers
const char* getErrorCString();
std::string getErrorString();
bool setErrorMessage(const char* message);
bool init(uint64_t flags);
bool configure(const SDOM_CoreConfig* cfg);
bool getCoreConfig(SDOM_CoreConfig* out_cfg);
bool setStopAfterUnitTests(bool stop);
bool setIsRunning(bool running);
bool loadDomFromJsonFile(const char* filename);
bool run();
void quit();

// Bindings
void registerBindings(Core& core, const std::string& typeName);

} // namespace CoreAPI

} // namespace SDOM
