#include <filesystem>

namespace LaurelEye::IO {
    namespace fs = std::filesystem;

    using Path = fs::path;

    struct RuntimePaths {
        fs::path exePath;   // full path to executable
        fs::path exeDir;    // directory containing executable
        fs::path assetsDir; // exeDir/assets

        fs::path asset(std::string_view rel) const;

        fs::path script(std::string_view rel) const;

        fs::path resolve(std::string_view relativePath) const;
    };

    /// @brief Return instance of RuntimePaths
    /// RuntimePaths uses a static variable so it should be the same globally.
    ///
    /// @return RuntimePaths object with the paths needed in runtime.
    RuntimePaths& Paths();

} // namespace LaurelEye::IO
