/// @file    UILayout.h
/// @author  Daoming Wang (KeitherJinn@outlook.com)
/// @par     **Digipen Email**
///    daoming.wang@digipen.edu
/// @date    11-13-25
/// @brief   File containing the UILayout and SingleUIMapping class definitions
#pragma once

#include <map>
#include <string>

namespace LaurelEye {
    class SingleUIMapping {
    public:
        SingleUIMapping()
            : current("NULL"), left("NULL"), right("NULL"), up("NULL"), down("NULL") {}
        SingleUIMapping(std::string _current, std::string _left, std::string _right, std::string _up, std::string _down)
            : current(_current), left(_left), right(_right), up(_up), down(_down) {}

        SingleUIMapping(const SingleUIMapping& s) {
            current = s.current;
            left = s.left;
            right = s.right;
            up = s.up;
            down = s.down;
        }

        ~SingleUIMapping() = default;

        SingleUIMapping& operator=(const SingleUIMapping& s) {
            if (this != &s) {
                current = s.current;
                left = s.left;
                right = s.right;
                up = s.up;
                down = s.down;
            }

            return *this;
        }

        void setCurrent(const std::string& name) { current = name; }
        std::string getCurrent() { return current; }
        void setLeft(const std::string& name) { left = name; }
        std::string getLeft() { return left; }
        void setRight(const std::string& name) { right = name; }
        std::string getRight() { return right; }
        void setUp(const std::string& name) { up = name; }
        std::string getUp() { return up; }
        void setDown(const std::string& name) { down = name; }
        std::string getDown() { return down; }
        const std::string getInvalid() { return invalid; }

    private:
        std::string current = "NULL";
        std::string left = "NULL";
        std::string right = "NULL";
        std::string up = "NULL";
        std::string down = "NULL";

        const std::string invalid = "NULL";
    };

    class UILayout {
    public:
        UILayout()
            : current(nullptr), name("Default") {}
        ~UILayout();

        void addMappingUnit(SingleUIMapping* mapping);
        void deleteMappingUnit(const std::string& name);
        void deleteMappingUnit(SingleUIMapping* mapping);

        std::string getCurrent() { return current->getCurrent(); }
        std::string getLeft() { return current->getLeft(); }
        std::string getRight() { return current->getRight(); }
        std::string getUp() { return current->getUp(); }
        std::string getDown() { return current->getDown(); }
        void setCurrent(const std::string& name);

        std::unordered_map<std::string, SingleUIMapping*> getInputMap() const { return inputMap; }

        void toLeft();
        void toRight();
        void toUp();
        void toDown();

        void setName(const std::string& newName) { name = newName; }
        std::string getName() const { return name; }

    private:
        std::string name = "Default";

        SingleUIMapping* current = nullptr;
        std::unordered_map<std::string, SingleUIMapping*> inputMap = std::unordered_map<std::string, SingleUIMapping*>();
    };
} // namespace LaurelEye
