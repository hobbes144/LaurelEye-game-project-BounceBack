#pragma once
#include "LaurelEyeEngine/events/IEvent.h"

namespace LaurelEye {
    class WindowResizeEvent : public IEvent {
    public:
        WindowResizeEvent(int _width, int _height)
            : width(_width), height(_height) {}

        int width;
        int height;
    };
}

