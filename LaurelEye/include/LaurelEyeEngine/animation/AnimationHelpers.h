#pragma once

#include "LaurelEyeEngine/animation/resources/SkeletalAnimation.h"
#include "LaurelEyeEngine/math/Transform.h"

#include <algorithm>
#include <cmath>

namespace LaurelEye::Animations {

    static inline double wrapTime(double t, double duration) {
        if ( duration <= 0.0 ) return 0.0;
        t = std::fmod(t, duration);
        if ( t < 0.0 ) t += duration;
        return t;
    }

    static inline Transform sampleChannel(const SkeletalAnimation::AnimationChannel& ch, float t) {
        const auto& keys = ch.keyframes;
        if ( keys.empty() ) return Transform{};
        if ( keys.size() == 1 ) return keys[0].localPose;

        // If time before first/after last, clamp (or loop earlier by wrapping t)
        if ( t <= keys.front().timeStamp ) return keys.front().localPose;
        if ( t >= keys.back().timeStamp ) return keys.back().localPose;

        // Find first key with timeStamp > t
        auto it = std::upper_bound(
            keys.begin(), keys.end(), t,
            [](float value, const SkeletalAnimation::Keyframe& k) { return value < k.timeStamp; });

        const size_t i1 = static_cast<size_t>(it - keys.begin());
        const size_t i0 = i1 - 1;

        const float t0 = keys[i0].timeStamp;
        const float t1 = keys[i1].timeStamp;
        const float denom = (t1 - t0);
        const float alpha = (denom > 1e-6f) ? (t - t0) / denom : 0.0f;

        return Transform::lerp(keys[i0].localPose, keys[i1].localPose, alpha);
    }

} // namespace LaurelEye::Animations
