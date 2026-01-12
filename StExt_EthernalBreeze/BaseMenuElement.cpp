#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	BaseMenuElement::BaseMenuElement() { }

	void BaseMenuElement::Resize() { BaseUiElement::Resize(); }

	void BaseMenuElement::Init() 
	{
		IsSelected = false;
		IsHovered = false;
		IsEnabled = true;
		BehaviorFlags = UiElementBehaviorFlags::Empty;
		BaseUiElement::Init();
	}

	void BaseMenuElement::Draw() { BaseUiElement::Draw(); }

	void BaseMenuElement::Update() { BaseUiElement::Update(); }

	inline bool BaseMenuElement::HasBehavior(UiElementBehaviorFlags flag) { return (static_cast<uint>(BehaviorFlags) & static_cast<uint>(flag)) != 0UL; }
	inline bool BaseMenuElement::CanInteract() { return IsEnabled && HasBehavior(UiElementBehaviorFlags::Interactable); }

	BaseMenuElement* BaseMenuElement::FindElementAt(int globalX, int globalY)
	{
		if (!IsVisible || IsHiden) return Null;

		const bool isHit = (globalX > GlobalRectLeft && globalX < GlobalRectRight) && (globalY > GlobalRectTop && globalY < GlobalRectBottom);
		return isHit ? this : Null;
	}

	BaseMenuElement::~BaseMenuElement()	{ }
}