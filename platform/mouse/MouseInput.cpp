#include "MouseInput.h"

namespace it {
	// class MouseInput

	// Object | public

	// Constructor / Destructor
	MouseInput::MouseInput() {
		size_t i = 0;
		mouseButtons[i++] = MouseButton{ MouseButtonCode::UNKNOWN, false, false, false, "unknown" };
		mouseButtons[i++] = MouseButton{ MouseButtonCode::BUTTON_CODE_1, false, false, false, "left button" };
		mouseButtons[i++] = MouseButton{ MouseButtonCode::BUTTON_CODE_2, false, false, false, "right button" };
		mouseButtons[i++] = MouseButton{ MouseButtonCode::BUTTON_CODE_3, false, false, false, "middle button" };
		mouseButtons[i++] = MouseButton{ MouseButtonCode::BUTTON_CODE_4, false, false, false, "button 4" };
		mouseButtons[i++] = MouseButton{ MouseButtonCode::BUTTON_CODE_5, false, false, false, "button 5" };
		mouseButtons[i++] = MouseButton{ MouseButtonCode::BUTTON_CODE_6, false, false, false, "button 6" };
		mouseButtons[i++] = MouseButton{ MouseButtonCode::BUTTON_CODE_7, false, false, false, "button 7" };
		mouseButtons[i++] = MouseButton{ MouseButtonCode::BUTTON_CODE_8, false, false, false, "button 8" };
	}

	// Getters
	const std::forward_list<MouseButton*>& MouseInput::getMouseButtonsToReset() const {
		return mouseButtonsInTransientState;
	}
	const std::array<MouseButton, static_cast<size_t>(MouseButtonCode::COUNT)>& MouseInput::getMouseButtons() const {
		return mouseButtons;
	}
	
	const MouseButton& MouseInput::getMouseButton(MouseButtonCode mouseButtonCode) const {
		size_t i = getMouseButtonIndex(mouseButtonCode);
		return mouseButtons[i];
	}
	size_t MouseInput::getMouseButtonIndex(MouseButtonCode mouseButtonCode) const {
		size_t i = static_cast<size_t>(mouseButtonCode);
		size_t count = static_cast<size_t>(MouseButtonCode::COUNT);
		return i < count ? i : count;
	}

	// Functions
	void MouseInput::feedAction(MouseButtonCode mouseButtonCode, MouseButton::Action action) {
		size_t i = getMouseButtonIndex(mouseButtonCode);

		if (!mouseButtons[i].inTransientState())
			mouseButtonsInTransientState.push_front(&mouseButtons[i]);

		mouseButtons[i].feedAction(action);
	}
	void MouseInput::reset() {
		for (size_t i = 0; i < mouseButtons.size(); i++)
			mouseButtons[i].reset();
	}
	void MouseInput::resetTransientStates() {
		for (MouseButton* mouseButton : mouseButtonsInTransientState)
			mouseButton->resetTransientState();
		mouseButtonsInTransientState.clear();
	}
	void MouseInput::resetAllTransientStates() {
		for (size_t i = 0; i < mouseButtons.size(); i++)
			mouseButtons[i].resetTransientState();
		mouseButtonsInTransientState.clear();
	}
}
