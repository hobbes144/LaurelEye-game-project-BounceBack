///// @file AudioLoader.cpp
//#include "LaurelEyeEngine/audio/AudioLoader.h"
//
//namespace LaurelEye::Audio {
//
//    AudioLoader::AudioLoader() {
//        worker = std::thread(&AudioLoader::workerLoop, this);
//    }
//
//    AudioLoader::~AudioLoader() {
//        flush();
//        {
//            std::scoped_lock lock(mutex);
//            running = false;
//        }
//        cv.notify_one();
//        worker.join();
//    }
//
//    void AudioLoader::submit(std::function<void()> job) {
//        {
//            std::scoped_lock lock(mutex);
//            jobs.push(std::move(job));
//        }
//        cv.notify_one();
//    }
//
//    void AudioLoader::flush() {
//        std::unique_lock lock(mutex);
//        cv.wait(lock, [this]() { return jobs.empty(); });
//    }
//
//    void AudioLoader::workerLoop() {
//        while ( true ) {
//            std::function<void()> job;
//            {
//                std::unique_lock lock(mutex);
//                cv.wait(lock, [this]() {
//                    return !jobs.empty() || !running.load();
//                });
//
//                if ( !running.load() && jobs.empty() ) return;
//
//                job = std::move(jobs.front());
//                jobs.pop();
//            }
//            job();
//            cv.notify_all(); // wake flush() if it's waiting
//        }
//    }
//
//} // namespace LaurelEye::Audio
