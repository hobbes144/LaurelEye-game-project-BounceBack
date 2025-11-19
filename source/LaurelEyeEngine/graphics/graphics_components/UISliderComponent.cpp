///****************************************************************
//// File Name: UISliderComponent
//****************************************************************/
//
//#include "precompiled.h"
//#include "UISliderComponent.h"
//#include "Event/Events/UISliderValueChangedEvent.h"
//#include "Managers/GlobalSoundManager.h"
//
//void UISliderComponent::Initialize()
//{
//	UIElementComponent::Initialize();
//	SetValue(1.0f);
//}
//
//void UISliderComponent::HandleInput(float deltaTime)
//{
//	timeSinceLastInput += deltaTime;
//
//	if (timeSinceLastInput >= inputDelay)
//	{
//		if ((Input::Instance().IsKeyHeld(KeyCode::Left) ||
//			 Input::Instance().IsKeyHeld(KeyCode::A) ||
//			 Input::Instance().IsGamepadButtonHeld(GamepadButton::Dpad_Left) ||
//			 Input::Instance().GetGamepadAxisValue(GamepadAxes::Axis_Left_X, 0) < -0.5f
//			)
//			&& value >= 0.0f) {
//			SetValue(value - 0.01f);
//			timeSinceLastInput = 0.0f;
//		}
//		if ((Input::Instance().IsKeyHeld(KeyCode::Right) ||
//			Input::Instance().IsKeyHeld(KeyCode::D) ||
//			Input::Instance().IsGamepadButtonHeld(GamepadButton::Dpad_Right) ||
//			Input::Instance().GetGamepadAxisValue(GamepadAxes::Axis_Left_X, 0) > 0.5f
//			)
//			&& value <= 1.0f) {
//			SetValue(value + 0.01f);
//			timeSinceLastInput = 0.0f;
//		}
//	}
//}
//
//void UISliderComponent::SetValue(float newValue)
//{
//	value = newValue;
//
//	// Change the visual to represent it
//	if (uiComponent != nullptr)
//	{
//		float sliderRatio = value / 1.0f;
//		Vector3 newScale = uiComponent->GetOwner()->GetLocalScale();
//		newScale.x = sliderRatio * 6.0f;
//
//		uiComponent->GetOwner()->SetLocalScale(newScale);
//	}
//
//	// TODO Create a new value event and send it out
//	UISliderValueChangedEvent event(uiTag, value);
//	EventManager::Instance().BroadcastEvent(event);
//}
//
//void UISliderComponent::SetIsHovered(bool newIsHovered)
//{
//	isHovered = newIsHovered;
//
//	if (isHovered && uiComponent) {
//		uiComponent->GetRenderable()->opacity = 1.0f;
//	}
//	else if (!isHovered && uiComponent) {
//		uiComponent->GetRenderable()->opacity = 0.5f;
//	}
//}
