#pragma once

// Dependencies | std
#include <list>

// Dependencies | glm
#include <glm/vec2.hpp>

// Dependencies | core
#include <core/Rect.h>

namespace it {
	namespace ui {
		class Widget {
			// Object
			private:
				// Properties
				Rect geometry{ { 0, 0 }, { 0, 0 } };

				std::list<Widget*> widgets{};

			public:
				// Constructor / Destructor

				// Functions
				Widget* widgetAtPosition(glm::ivec2 position);
		};
	}
}
