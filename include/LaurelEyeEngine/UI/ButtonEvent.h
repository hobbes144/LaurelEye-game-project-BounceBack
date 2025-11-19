/// @file    ButtonEvent.h
/// @author  Daoming Wang (KeitherJinn@outlook.com)
/// @par     **Digipen Email**
///    daoming.wang@digipen.edu
/// @date    11-13-25
/// @brief   File containing the button event implementation for the LaurelEyeEngine
/// Inherits from the IEvent class
#pragma once

#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/events/IEvent.h"

namespace LaurelEye {
    class ButtonEvent : public IEvent {
    public:
        explicit ButtonEvent(const std::string& name)
            : buttonName(name) {}

        virtual ~ButtonEvent() = default;

        std::string GetData() const { return buttonName; }

    private:
        std::string buttonName;
    };
} // namespace LaurelEye
