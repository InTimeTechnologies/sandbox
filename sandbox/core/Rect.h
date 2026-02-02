#pragma once

// Dependencies | glm
#include <glm/vec2.hpp>

namespace it {
	namespace ui {
		struct Rect {
			// Properties
			glm::ivec2 position{ 0, 0 };
			glm::ivec2 size{ 0, 0 };

			// Operators | assignment
			bool operator==(const Rect& other) const = default;
			bool operator!=(const Rect& other) const = default;

			// Functions
			int x() const;
			int y() const;
			int width() const;
			int height() const;

			int top() const;
			int bottom() const;
			int right() const;
			int left() const;

			glm::ivec2 topRight() const;
			glm::ivec2 topLeft() const;
			glm::ivec2 bottomRight() const;
			glm::ivec2 bottomLeft() const;
			glm::ivec2 center() const;

			bool contains(int x, int y) const;
			bool contains(glm::ivec2 point) const;
			bool contains(const Rect& other) const;
			bool intersects(const Rect& other) const;

			Rect merged(const Rect& other) const;
			Rect intersected(const Rect& other) const;
			Rect normalized() const;

			Rect& merge(const Rect& other);
			Rect& intersect(const Rect& other);
			Rect& normalize();

			bool isValid() const;
		};
	}
}
