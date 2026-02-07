#pragma once

#include <cstdint>
#include <string>

namespace LaurelEye::Animations {

    using AnimationHandle = uint32_t;

    struct Animation {
        static constexpr AnimationHandle InvalidAnimation = UINT32_MAX;
        enum class Type {
            Basic,
            Skeletal
        };

        Animation(Type _type = Type::Basic) : type(_type) {}

        virtual ~Animation() {}

        AnimationHandle handle = Animation::InvalidAnimation;
        Type type;
        std::string name;
        double duration;
    };

    /// @brief Checks whether a given animation handle is valid.
    /// @param h The handle to validate.
    /// @return `true` if the handle refers to a valid animation, otherwise `false`.
    inline bool isValidAnimation(AnimationHandle h) noexcept {
        return h != Animation::InvalidAnimation;
    }

} // namespace LaurelEye
