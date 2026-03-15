#include "LaurelEyeEngine/UI/UIComponents/UIInteractionComponent.h"

namespace LaurelEye::UI {

            // Component Accessors
    bool UIInteractionComponent::getIsInteractable() const {
        return isInteractable;
    }
    void UIInteractionComponent::setInteractable(bool newI) {
        isInteractable = newI;
    }

    bool UIInteractionComponent::getIsBlockingRaycasts() const {
        return blockRaycast;
    }
    void UIInteractionComponent::setBlockingRaycasts(bool newBR) {
        blockRaycast = newBR;
    }

    bool UIInteractionComponent::getRecievingHover() const {
        return receiveHover;
    }
    void UIInteractionComponent::setReceiveHover(bool nv) {
        receiveHover = nv;
    }

    bool UIInteractionComponent::getRecievingClick() const {
        return receiveClick;
    }
    void UIInteractionComponent::setReceiveClick(bool nv) {
        receiveClick = nv;
    }

    bool UIInteractionComponent::getDragThreshold() const {
        return dragThreshold;
    }
    void UIInteractionComponent::setDragThreshold(float nv) {
        dragThreshold = nv;
    }

    bool UIInteractionComponent::getRecievingDrag() const {
        return receiveDrag;
    }
    void UIInteractionComponent::setReceiveDrag(bool nv) {
        receiveDrag = nv;
    }

    bool UIInteractionComponent::getFocusable() const {
        return focusable;
    }
    void UIInteractionComponent::setFocusable(bool nv) {
        focusable = nv;
    }

    int UIInteractionComponent::getInteractionPriority() const {
        return interactionPriority;
    }
    void UIInteractionComponent::setInteractionPriority(int nv) {
        interactionPriority = nv;
    }

    // State Accessors
    bool UIInteractionComponent::getIsHovered() const {
        return isHovered;
    }
    void UIInteractionComponent::setIsHovered(bool newIH) {
        isHovered = newIH;
    }

    bool UIInteractionComponent::getWasHovered() const {
        return wasHovered;
    }
    void UIInteractionComponent::setWasHovered(bool newWH) {
        wasHovered = newWH;
    }

    bool UIInteractionComponent::getIsPressed() const {
        return isPressed;
    }
    void UIInteractionComponent::setIsPressed(bool newIP) {
        isPressed = newIP;
    }

    bool UIInteractionComponent::getWasPressed() const {
        return wasPressed;
    }
    void UIInteractionComponent::setWasPressed(bool newWP) {
        wasPressed = newWP;
    }

    bool UIInteractionComponent::getIsDragging() const {
        return isDragging;
    }
    void UIInteractionComponent::setIsDragging(bool nv) {
        isDragging = nv;
    }

    Vector2 UIInteractionComponent::getPressStartPos() const {
        return pressStartPosition;
    }
    void UIInteractionComponent::setPressStartPos(Vector2 nv) {
        pressStartPosition = nv;
    }

    Vector2 UIInteractionComponent::getDragLastPos() const {
        return dragLastPosition;
    }
    void UIInteractionComponent::setDragLastPos(Vector2 nv) {
        dragLastPosition = nv;
    }

    Vector2 UIInteractionComponent::getDragOffset() const {
        return dragOffset;
    }
    void UIInteractionComponent::setDragOffset(Vector2 nv) {
        dragOffset = nv;
    }

}
