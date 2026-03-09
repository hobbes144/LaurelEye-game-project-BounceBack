#include "LaurelEyeEngine/io/Paths.h"

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#elif defined(__linux__)
#include <limits.h>
#include <unistd.h>
#endif

#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye::IO {
    namespace fs = std::filesystem;

    RuntimePaths paths{}; // <-- THE one definition

    static fs::path GetExecutablePath() {
#if defined(_WIN32)
        std::wstring buf;
        buf.resize(32768);

        DWORD len = GetModuleFileNameW(nullptr, buf.data(), (DWORD)buf.size());
        if ( len == 0 || len >= buf.size() ) {
            // You can replace with your logging/assert macro
            LE_ERROR("io", "Failed to get executable path");
        }
        buf.resize(len);
        return fs::path(buf);

#elif defined(__linux__)
        char buf[PATH_MAX];
        ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if ( len <= 0 ) {
            LE_ERROR("io", "Failed to get executable path");
        }
        buf[len] = '\0';
        return fs::path(buf);

#else
#error Unsupported platform
#endif
    }

    RuntimePaths InitRuntimePaths() {
        RuntimePaths p{};
        p.exePath = GetExecutablePath();
        p.exeDir = p.exePath.parent_path();
        p.assetsDir = p.exeDir / "assets";
        return p;
    }

    RuntimePaths& Paths() {
        static RuntimePaths p = InitRuntimePaths();
        return p;
    }

    fs::path RuntimePaths::asset(std::string_view rel) const {
        LE_DEBUG_ASSERT("io", fs::exists(assetsDir), "Assets directory does not exist: " + assetsDir.string());
        return assetsDir / fs::path(rel);
    }

    fs::path RuntimePaths::script(std::string_view rel) const {
        LE_DEBUG_ASSERT("io", fs::exists(assetsDir / "scripts"), "Scripts directory does not exist: " + (assetsDir / "scripts").string());
        return assetsDir / "scripts" / fs::path(rel);
    }

    fs::path RuntimePaths::resolve(std::string_view relativePath) const {
        LE_DEBUG_ASSERT("io", fs::exists(exePath), "Executable directory does not exist: " + exePath.string());
        return exeDir / relativePath;
    }

} // namespace LaurelEye::IO

