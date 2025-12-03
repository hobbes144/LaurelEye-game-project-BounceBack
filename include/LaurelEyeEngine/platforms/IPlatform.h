#pragma once

namespace LaurelEye {

    class IPlatform {
    public:
        virtual ~IPlatform() = default;

        virtual void initialize() = 0;
        virtual void update() = 0;
        virtual void shutdown() = 0;
    };

} // namespace LaurelEye
