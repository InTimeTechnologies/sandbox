#pragma once

// Dependencies | std
#include <string>

// Dependencies | it
#include "MouseButtonCode.h"

namespace it {
	struct MouseButton {
		// Enums
		enum class Action {
			UNKNOWN,
			PRESSED,
			RELEASED
		};

		// Properties
		MouseButtonCode buttonCode{ MouseButtonCode::UNKNOWN };
		bool pressed{ false };
		bool justPressed{ false };
		bool justReleased{ false };
		std::string description{};

		// Functions
		bool inTransientState() const;
		void feedAction(Action action);
		void reset();
		void resetTransientState();
		void resetJustPressed();
		void resetJustReleased();
	};
}
