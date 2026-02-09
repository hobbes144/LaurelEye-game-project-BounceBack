/// @file   engineLog.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    02-08-2026
/// @brief Debug Logging and Asserts

// Usage:
// LE_DEBUG_WARN_IF("Physics", dt > 0.05f, "Large dt: " << dt);
// LE_DEBUG_WARN("Scripting", "Reloaded " << scriptCount << " scripts");
//
// LE_DEBUG_ASSERT("Render", framebuffer != 0, "FBO not created; name=" << fboName);
// LE_DEBUG_ASSERT0("Scene", owner != nullptr);
//
// LE_DEBUG_WARN_IF_ONCE("Input", keyRepeatDetected, "Key=" << key);
// LE_DEBUG_ASSERT_ONCE("Physics", !std::isnan(pos.x), "pos=" << pos);


#pragma once
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <source_location>
#include <sstream>
#include <string_view>
#include <thread>

namespace LaurelEye::Log {

enum class Level : uint8_t { Info, Warn, Error, Assert };

// ---------- Configuration ----------
struct Config {
    bool print_time     = true;
    bool print_thread   = true;
    bool print_location = true; // file:line + function
};

inline Config& config() { static Config cfg{}; return cfg; }

// Optional: if you want to stamp logs with a frame index from your engine loop.
inline std::atomic<uint64_t>& frame_counter() { static std::atomic<uint64_t> f{0}; return f; }
inline void set_frame(uint64_t frame) { frame_counter().store(frame, std::memory_order_relaxed); }

// ---------- Sinks (today: stderr; later: file/console/etc.) ----------
inline std::mutex& sink_mutex() { static std::mutex m; return m; }

inline std::string_view level_sv(Level lvl) {
    switch (lvl) {
        case Level::Info:   return "info";
        case Level::Warn:   return "warn";
        case Level::Error:  return "error";
        case Level::Assert: return "assert";
    }
    return "log";
}

inline void append_time(std::ostream& os)
{
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto ms  = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::time_t t = system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    os << std::setfill('0')
       << std::setw(2) << tm.tm_hour << ':'
       << std::setw(2) << tm.tm_min  << ':'
       << std::setw(2) << tm.tm_sec  << '.'
       << std::setw(3) << ms.count();
}

inline uint64_t thread_id_u64()
{
    // Portable-ish: hash std::thread::id to an integer.
    return static_cast<uint64_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

// The one function you’ll later reroute to your real logger.
inline void write(Level lvl,
                  std::string_view subsystem,
                  std::string_view msg,
                  std::source_location loc = std::source_location::current())
{
    std::ostringstream out;

    if (config().print_time) {
        out << '['; append_time(out); out << "] ";
    }

    out << '[' << level_sv(lvl) << ']';

    if (!subsystem.empty())
        out << '[' << subsystem << ']';

    // Optional frame stamp (handy once you have a game loop)
    out << "[f=" << frame_counter().load(std::memory_order_relaxed) << ']';

    if (config().print_thread)
        out << "[t=" << thread_id_u64() << ']';

    out << ' ' << msg;

    if (config().print_location) {
        out << "  @ " << loc.file_name() << ':' << loc.line()
            << " (" << loc.function_name() << ')';
    }

    std::lock_guard<std::mutex> lock(sink_mutex());
    std::cerr << out.str() << '\n';
}

// Stream-style message builder
#define LE_LOG_MSG(stream_expr) \
    ([&]() -> std::string { std::ostringstream _oss; _oss << stream_expr; return _oss.str(); }())

// Debug break for asserts
inline void debug_break()
{
#if defined(_MSC_VER)
    __debugbreak();
#elif defined(__clang__) || defined(__GNUC__)
    __builtin_trap();
#else
    std::abort();
#endif
}

[[noreturn]] inline void fail_assert(std::string_view subsystem,
                                    std::string_view expr,
                                    std::string_view msg,
                                    std::source_location loc = std::source_location::current())
{
    std::ostringstream oss;
    oss << "Condition failed: (" << expr << ")";
    if (!msg.empty()) oss << " — " << msg;

    write(Level::Assert, subsystem, oss.str(), loc);
    debug_break();
    std::abort();
}

} // namespace LaurelEye::Log

// -------------------- Info ---------------------
#define LE_INFO(subsystem, msg_stream_expr) \
    do { ::LaurelEye::Log::write(::LaurelEye::Log::Level::Info, (subsystem), LE_LOG_MSG(msg_stream_expr), std::source_location::current()); } while (0)

#ifndef NDEBUG
    #define LE_DEBUG_INFO(subsystem, msg_stream_expr) LE_INFO(subsystem, msg_stream_expr)
#else
    #define LE_DEBUG_INFO(subsystem, msg_stream_expr) ((void)0)
#endif

// -------------------- Error --------------------
#define LE_ERROR(subsystem, msg_stream_expr) \
    do { ::LaurelEye::Log::write(::LaurelEye::Log::Level::Error, (subsystem), LE_LOG_MSG(msg_stream_expr), std::source_location::current()); } while (0)

#ifndef NDEBUG
    #define LE_DEBUG_ERROR(subsystem, msg_stream_expr) LE_ERROR(subsystem, msg_stream_expr)
#else
    #define LE_DEBUG_ERROR(subsystem, msg_stream_expr) ((void)0)
#endif

// -------------------- Warnings -----------------
#define LE_WARN(subsystem, msg_stream_expr) \
    do { ::LaurelEye::Log::write(::LaurelEye::Log::Level::Warn, (subsystem), LE_LOG_MSG(msg_stream_expr), std::source_location::current()); } while (0)

#define LE_WARN_IF(subsystem, cond, msg_stream_expr) \
    do { if ((cond)) ::LaurelEye::Log::write(::LaurelEye::Log::Level::Warn, (subsystem), LE_LOG_MSG("Condition true: (" #cond ") — " << msg_stream_expr), std::source_location::current()); } while (0)

#ifndef NDEBUG
    #define LE_DEBUG_WARN(subsystem, msg_stream_expr) LE_WARN(subsystem, msg_stream_expr)
    #define LE_DEBUG_WARN_IF(subsystem, cond, msg_stream_expr) LE_WARN_IF(subsystem, cond, msg_stream_expr)
#else
    #define LE_DEBUG_WARN(subsystem, msg_stream_expr) ((void)0)
    #define LE_DEBUG_WARN_IF(subsystem, cond, msg_stream_expr) ((void)0)
#endif

#define LE_WARN_IF_ONCE(subsystem, cond, msg_stream_expr) \
    do { \
        static std::atomic_bool _once{false}; \
        if ((cond) && !_once.exchange(true, std::memory_order_relaxed)) \
            ::LaurelEye::Log::write(::LaurelEye::Log::Level::Warn, (subsystem), LE_LOG_MSG("Condition true (once): (" #cond ") — " << msg_stream_expr), std::source_location::current()); \
    } while (0)

#ifndef NDEBUG
    #define LE_DEBUG_WARN_IF_ONCE(subsystem, cond, msg_stream_expr) LE_WARN_IF_ONCE(subsystem, cond, msg_stream_expr)
#else
    #define LE_DEBUG_WARN_IF_ONCE(subsystem, cond, msg_stream_expr) ((void)0)
#endif

// -------------------- Asserts -------------------
#define LE_ASSERT(subsystem, cond, msg_stream_expr) \
    do { if (!(cond)) ::LaurelEye::Log::fail_assert((subsystem), #cond, std::string_view{LE_LOG_MSG(msg_stream_expr)}, std::source_location::current()); } while (0)

#define LE_ASSERT0(subsystem, cond) \
    do { if (!(cond)) ::LaurelEye::Log::fail_assert((subsystem), #cond, std::string_view{}, std::source_location::current()); } while (0)

#ifndef NDEBUG
    #define LE_DEBUG_ASSERT(subsystem, cond, msg_stream_expr) LE_ASSERT(subsystem, cond, msg_stream_expr)
    #define LE_DEBUG_ASSERT0(subsystem, cond) LE_ASSERT0(subsystem, cond)
#else
    #define LE_DEBUG_ASSERT(subsystem, cond, msg_stream_expr) ((void)0)
    #define LE_DEBUG_ASSERT0(subsystem, cond) ((void)0)
#endif

#define LE_ASSERT_ONCE(subsystem, cond, msg_stream_expr) \
    do { \
        static std::atomic_bool _once{false}; \
        if (!(cond) && !_once.exchange(true, std::memory_order_relaxed)) \
            ::LaurelEye::Log::fail_assert((subsystem), #cond, std::string_view{LE_LOG_MSG(msg_stream_expr)}, std::source_location::current()); \
    } while (0)

#ifndef NDEBUG
    #define LE_DEBUG_ASSERT_ONCE(subsystem, cond, msg_stream_expr) LE_ASSERT_ONCE(subsystem, cond, msg_stream_expr)
#else
    #define LE_DEBUG_ASSERT_ONCE(subsystem, cond, msg_stream_expr) ((void)0)
#endif

