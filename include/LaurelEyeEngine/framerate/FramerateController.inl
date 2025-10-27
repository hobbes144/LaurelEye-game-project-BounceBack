#ifndef FRAMERATECONTROLLER_INL
#define FRAMERATECONTROLLER_INL

#include <chrono>
#include <thread>

template <typename T>
FramerateController<T>* FramerateController<T>::getController() {
    static FramerateController<T> controller;
    return &controller;
}

template <typename T>
FramerateController<T>::FramerateController()
    : targetFrameTime(static_cast<T>(1.0 / 60.0)),
      timeAtFrameStart(getTime()),
      timeAtFrameEnd(getTime()),
      renderTime(0),
      lastFrameTime(0),
      frameCount(0),
      framesSinceLastFPSQuery(0),
      timeOfLastFPSQuery(getTime()),
      physicsAccumulatorTimestep(static_cast<T>(1.0 / 120.0)),
      physicsAccumulator(0)
#ifdef ENABLE_RATE_CONTROLLERS
      ,
      rateControllersRegistered(0),
      rateControllerTargetRates(),
      rateControllerTimes()
#endif
#ifdef ENABLE_ADDITIONAL_ACCUMULATORS
      ,
      accumulatorsRegistered(0),
      accumulatorTimesteps(),
      accumulatorTimes()
#endif
{
}

template <typename T>
void FramerateController<T>::setTargetFramerate(unsigned int framerate) {
    targetFrameTime = static_cast<T>(1.0) / framerate;
}

template <typename T>
void FramerateController<T>::startFrame() {
    timeAtFrameStart = getTime();
}

template <typename T>
void FramerateController<T>::endFrame() {
    renderTime = getTime() - timeAtFrameStart;
    frameCount++;
    framesSinceLastFPSQuery++;

    // Busy-wait until target frame time (for now)
    while ( (getTime() - timeAtFrameStart) < targetFrameTime ) {}

    timeAtFrameEnd = getTime();
    lastFrameTime = timeAtFrameEnd - timeAtFrameStart;
    physicsAccumulator += lastFrameTime;

#ifdef ENABLE_ADDITIONAL_ACCUMULATORS
    for ( unsigned int i = 0; i < accumulatorsRegistered; ++i )
        accumulatorTimes[i] += lastFrameTime;
#endif
}

template <typename T>
T FramerateController<T>::getFPS() {
    T lastQuery = timeOfLastFPSQuery;
    long int numFrames = framesSinceLastFPSQuery;
    timeOfLastFPSQuery = getTime();
    framesSinceLastFPSQuery = 0;
    return numFrames / (timeOfLastFPSQuery - lastQuery);
}

template <typename T>
T FramerateController<T>::getRenderTime() {
    return renderTime;
}

template <typename T>
T FramerateController<T>::getFrameTime() {
    return lastFrameTime;
}

template <typename T>
T FramerateController<T>::getTime() {
    using clock = std::chrono::high_resolution_clock;
    using seconds = std::chrono::duration<T>;
    static auto startTime = clock::now();
    auto now = clock::now();
    return std::chrono::duration_cast<seconds>(now - startTime).count();
}

template <typename T>
T FramerateController<T>::getPhysicsTimestep() {
    return physicsAccumulatorTimestep;
}

template <typename T>
void FramerateController<T>::setPhysicsTimestep(T timestep) {
    physicsAccumulatorTimestep = timestep;
}

template <typename T>
bool FramerateController<T>::shouldUpdatePhysics() const {
    return physicsAccumulator >= physicsAccumulatorTimestep;
}

template <typename T>
void FramerateController<T>::consumePhysicsTime() {
    physicsAccumulator -= physicsAccumulatorTimestep;
}

template <typename T>
void FramerateController<T>::clearPhysicsTime() {
    physicsAccumulator = static_cast<T>(0.0);
}

template <typename T>
T FramerateController<T>::getAccumulatorAlpha() const {
    return physicsAccumulator / physicsAccumulatorTimestep;
}

#ifdef ENABLE_RATE_CONTROLLERS
template <typename T>
const unsigned int FramerateController<T>::createRateController(unsigned int rate) {
    unsigned int id = rateControllersRegistered++;
    rateControllerTimes.push_back(getTime());
    rateControllerTargetRates.push_back(static_cast<T>(1.0) / rate);
    return id;
}

template <typename T>
bool FramerateController<T>::rateControllerShouldFire(const unsigned int id) {
    if ( id >= rateControllersRegistered ) return true;

    T current = getTime();
    if ( (current - rateControllerTimes[id]) > rateControllerTargetRates[id] ) {
        rateControllerTimes[id] = current;
        return true;
    }
    return false;
}
#endif

#ifdef ENABLE_ADDITIONAL_ACCUMULATORS
template <typename T>
const unsigned int FramerateController<T>::createAccumulator(const T timestep) {
    unsigned int id = accumulatorsRegistered++;
    accumulatorTimesteps.push_back(timestep);
    accumulatorTimes.push_back(0);
    return id;
}

template <typename T>
bool FramerateController<T>::accumulatorShouldFire(const unsigned int id) const {
    return (accumulatorTimes[id] >= accumulatorTimesteps[id]);
}

template <typename T>
void FramerateController<T>::consumeAccumulator(const unsigned int id) {
    accumulatorTimes[id] -= accumulatorTimesteps[id];
}

template <typename T>
T FramerateController<T>::getAccumulatorAlpha(const unsigned int id) const {
    return accumulatorTimes[id] / accumulatorTimesteps[id];
}
#endif

#endif // FRAMERATECONTROLLER_INL
