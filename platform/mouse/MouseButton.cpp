#include "MouseButton.h"

namespace it {
	// class MouseButton

	// Object | public

	// Functions
	bool MouseButton::inTransientState() const {
		return justPressed || justReleased;
	}
	void MouseButton::feedAction(Action action) {
		switch (action) {
			case Action::PRESSED:
				justPressed = true;
				pressed = true;
				break;
			case Action::RELEASED:
				justReleased = true;
				pressed = false;
				break;
		}
	}
	void MouseButton::reset() {
		pressed = false;
		justPressed = false;
		justReleased = false;
	}
	void MouseButton::resetTransientState() {
		justPressed = false;
		justReleased = false;
	}
	void MouseButton::resetJustPressed() {
		justPressed = false;
	}
	void MouseButton::resetJustReleased() {
		justReleased = false;
	}
}
