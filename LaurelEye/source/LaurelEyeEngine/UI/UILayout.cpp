#include "LaurelEyeEngine/UI/UILayout.h"

namespace LaurelEye {
    UILayout::~UILayout() {
        for ( const auto& [key, val] : inputMap ) {
            delete val;
        }
        inputMap.clear();
        current = nullptr;
    }

    void UILayout::addMappingUnit(SingleUIMapping* mapping) {
        bool isFirst = inputMap.empty();
        inputMap[mapping->getCurrent()] = mapping;
        if ( isFirst ) {
            current = inputMap[mapping->getCurrent()];
        }
    }

    void UILayout::deleteMappingUnit(const std::string& name) {
        inputMap.erase(name);
        current = inputMap.begin()->second;
    }

    void UILayout::deleteMappingUnit(SingleUIMapping* mapping) {
        bool isCurrent = current->getCurrent() == mapping->getCurrent();
        inputMap.erase(mapping->getCurrent());
        if ( isCurrent ) {
            current = inputMap.begin()->second;
        }
    }

    void UILayout::setCurrent(const std::string& name) {
        current = inputMap[name];
    }

    void UILayout::toLeft() {
        if ( current->getCurrent() != current->getInvalid() ) {
            if ( current->getLeft() != current->getInvalid() ) {
                auto iter = inputMap.find(current->getLeft());
                if ( iter != inputMap.end() ) {
                    current = iter->second;
                }
            }
        }
    }

    void UILayout::toRight() {
        if ( current->getCurrent() != current->getInvalid() ) {
            if ( current->getRight() != current->getInvalid() ) {
                auto iter = inputMap.find(current->getRight());
                if ( iter != inputMap.end() ) {
                    current = iter->second;
                }
            }
        }
    }

    void UILayout::toUp() {
        if ( current->getCurrent() != current->getInvalid() ) {
            if ( current->getUp() != current->getInvalid() ) {
                auto iter = inputMap.find(current->getUp());
                if ( iter != inputMap.end() ) {
                    current = iter->second;
                }
            }
        }
    }

    void UILayout::toDown() {
        if ( current->getCurrent() != current->getInvalid() ) {
            if ( current->getDown() != current->getInvalid() ) {
                auto iter = inputMap.find(current->getDown());
                if ( iter != inputMap.end() ) {
                    current = iter->second;
                }
            }
        }
    }

} // namespace LaurelEye
