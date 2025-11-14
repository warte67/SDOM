#include <string>

namespace SDOM {

// Minimal stub used only when building the dataregistry_generator
// to avoid linking the full runtime. It intentionally does nothing.
void showWarning(const std::string& message, const std::string& file, int line)
{
    (void)message;
    (void)file;
    (void)line;
}

} // namespace SDOM
