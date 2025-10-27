/// # FramerateController.h
///
/// **Authors**
/// - Anish Murthy (anish.murthy.dev@gmail.com)
///   - **DigiPen Email**: anish.murthy@digipen.edu
/// - Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
///   - (zaldivaralvarez@gmail.com) — Advising and Base Code
///
/// **Course:** CS529  
/// **Date:** 10-25-2024  
///
/// **Brief:** Framerate Controller
///
/// © 2025 DIGIPEN Institute of Technology. All rights reserved.
#ifndef FRAMERATE_CONTROLLER_H
#define FRAMERATE_CONTROLLER_H

#pragma once

#include <chrono>
#include <vector>

/// \class FramerateController
/// \brief Controls frame timing and framerate-independent updates.
///
/// Supports variable template precision (float, double, etc.).
template <typename T = double>
class FramerateController
{
public:
    // Singleton access
    static FramerateController<T>* getController();

    // Constructor
    FramerateController();

    // Framerate management
    void setTargetFramerate(unsigned int framerate);
    void startFrame();
    void endFrame();

    // Frame info
    T getFPS();
    T getRenderTime();
    T getFrameTime();
    static T getTime();

    // Physics accumulator system
    T getPhysicsTimestep();
    void setPhysicsTimestep(T timestep);
    bool shouldUpdatePhysics() const;
    void consumePhysicsTime();
    void clearPhysicsTime();
    T getAccumulatorAlpha() const;

#ifdef ENABLE_RATE_CONTROLLERS
    // Rate controller system (optional)
    const unsigned int createRateController(unsigned int rate);
    bool rateControllerShouldFire(unsigned int id);
#endif

#ifdef ENABLE_ADDITIONAL_ACCUMULATORS
    // Additional accumulators (optional)
    const unsigned int createAccumulator(const T timestep);
    bool accumulatorShouldFire(unsigned int id) const;
    void consumeAccumulator(unsigned int id);
    T getAccumulatorAlpha(unsigned int id) const;
#endif

private:
    // Timing variables
    T targetFrameTime;
    T timeAtFrameStart;
    T timeAtFrameEnd;
    T renderTime;
    T lastFrameTime;

    // FPS tracking
    long long frameCount;
    long long framesSinceLastFPSQuery;
    T timeOfLastFPSQuery;

    // Physics accumulator
    T physicsAccumulatorTimestep;
    T physicsAccumulator;

#ifdef ENABLE_RATE_CONTROLLERS
    unsigned int rateControllersRegistered;
    std::vector<T> rateControllerTargetRates;
    std::vector<T> rateControllerTimes;
#endif

#ifdef ENABLE_ADDITIONAL_ACCUMULATORS
    unsigned int accumulatorsRegistered;
    std::vector<T> accumulatorTimesteps;
    std::vector<T> accumulatorTimes;
#endif
};

/// \typedef FFramerateController
/// \brief Alias for float-based framerate controller.
/// Use this for most gameplay and rendering purposes.
using FFramerateController = FramerateController<float>;

// Include template implementation
#include "FramerateController.inl"

#endif FRAMERATE_CONTROLLER_H
