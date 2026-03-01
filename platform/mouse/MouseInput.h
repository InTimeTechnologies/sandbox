#pragma once

// Dependencies | std
#include <array>
#include <forward_list>

// Dependencies | it
#include "MouseButton.h"

namespace it {
	class MouseInput {
		// Object
		private:
			// Properties
			std::forward_list<MouseButton*> mouseButtonsInTransientState{};
			std::array<MouseButton, static_cast<size_t>(MouseButtonCode::COUNT)> mouseButtons{};

		public:
			// Constructor / Destructor
			MouseInput();
			MouseInput(const MouseInput& other) = default;
			MouseInput(MouseInput&& other) noexcept = default;
			~MouseInput() = default;

			// Operators | assignment
			MouseInput& operator=(const MouseInput& other) = default;
			MouseInput& operator=(MouseInput&& other) noexcept = default;

			// Getters
			const std::forward_list<MouseButton*>& getMouseButtonsToReset() const;
			const std::array<MouseButton, static_cast<size_t>(MouseButtonCode::COUNT)>& getMouseButtons() const;

			const MouseButton& getMouseButton(MouseButtonCode mouseButtonCode) const;
			size_t getMouseButtonIndex(MouseButtonCode mouseButtonCode) const;

			// Functions
			void feedAction(MouseButtonCode code, MouseButton::Action action);
			void reset();
			void resetTransientStates();
			void resetAllTransientStates();
	};
}
