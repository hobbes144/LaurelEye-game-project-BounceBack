/// @file AudioLoader.h
// muti thread loading, not use currently(handle by fmod is faster unless you have speacial data struct)
#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace LaurelEye::Audio {

    /// @brief Single background thread that processes audio load jobs.
    ///        Owned by AudioSystem — one instance for the whole engine.
    class AudioLoader {
    public:
        AudioLoader();
        ~AudioLoader();

        // Non-copyable, non-movable — owns a thread
        AudioLoader(const AudioLoader&) = delete;
        AudioLoader& operator=(const AudioLoader&) = delete;

        // Submit a load job — returns immediately
        void submit(std::function<void()> job);

        // Finish all pending jobs — call on shutdown or scene unload
        void flush();

    private:
        void workerLoop();

        std::thread worker;
        std::queue<std::function<void()>> jobs;
        std::mutex mutex;
        std::condition_variable cv;
        std::atomic<bool> running = true;
    };

} // namespace LaurelEye::Audio
