#include "Rect.h"

// Dependencies | std
#include <algorithm>

namespace it {
	namespace ui {
		// struct Rect

		// Object | public

		// Functions
		int Rect::x() const {
			return position.x;
		}
		int Rect::y() const {
			return position.y;
		}
		int Rect::width() const {
			return size.x;
		}
		int Rect::height() const {
			return size.y;
		}

		int Rect::top() const {
			return position.y;
		}
		int Rect::bottom() const {
			return position.y + size.y;
		}
		int Rect::right() const {
			return position.x + size.x;
		}
		int Rect::left() const {
			return position.x;
		}

		glm::ivec2 Rect::topRight() const {
			return { position.x + size.x, position.y };
		}
		glm::ivec2 Rect::topLeft() const {
			return position;
		}
		glm::ivec2 Rect::bottomRight() const {
			return position + size;
		}
		glm::ivec2 Rect::bottomLeft() const {
			return { position.x, position.y + size.y };
		}
		glm::ivec2 Rect::center() const {
			return { position.x + size.x / 2, position.y + size.y / 2 };
		}

		bool Rect::contains(int x, int y) const {
			return x >= left() && x < right() && y >= top() && y < bottom();
		}
		bool Rect::contains(glm::ivec2 point) const {
			return contains(point.x, point.y);
		}
		bool Rect::contains(const Rect& other) const {
			return other.left() >= left() &&
				other.right() <= right() &&
				other.top() >= top() &&
				other.bottom() <= bottom();
		}
		bool Rect::intersects(const Rect& other) const {
			return !(other.left() >= right() ||
				other.right() <= left() ||
				other.top() >= bottom() ||
				other.bottom() <= top());
		}

		Rect Rect::merged(const Rect& other) const {
			const int l = std::min(left(), other.left());
			const int t = std::min(top(), other.top());
			const int r = std::max(right(), other.right());
			const int b = std::max(bottom(), other.bottom());

			return {
				{ l, t },
				{ r - l, b - t }
			};
		}
		Rect Rect::intersected(const Rect& other) const {
			const int l = std::max(left(), other.left());
			const int t = std::max(top(), other.top());
			const int r = std::min(right(), other.right());
			const int b = std::min(bottom(), other.bottom());

			if (l >= r || t >= b)
				return { {0, 0}, {0, 0} }; // invalid

			return {
				{ l, t },
				{ r - l, b - t }
			};
		}
		Rect Rect::normalized() const {
			Rect r = *this;

			if (r.size.x < 0) {
				r.position.x += r.size.x;
				r.size.x = -r.size.x;
			}

			if (r.size.y < 0) {
				r.position.y += r.size.y;
				r.size.y = -r.size.y;
			}

			return r;
		}

		Rect& Rect::merge(const Rect& other) {
			const int l = std::min(left(), other.left());
			const int t = std::min(top(), other.top());
			const int r = std::max(right(), other.right());
			const int b = std::max(bottom(), other.bottom());

			position = { l, t };
			size = { r - l, b - t };

			return *this;
		}
		Rect& Rect::intersect(const Rect& other) {
			const int l = std::max(left(), other.left());
			const int t = std::max(top(), other.top());
			const int r = std::min(right(), other.right());
			const int b = std::min(bottom(), other.bottom());

			if (l >= r || t >= b)
				return *this; // invalid

			position.x = l;
			position.y = t;
			size.x = r - l;
			size.y = b - t;

			return *this;
		}
		Rect& Rect::normalize() {
			if (size.x < 0) {
				position.x += size.x;
				size.x = -size.x;
			}

			if (size.y < 0) {
				position.y += size.y;
				size.y = -size.y;
			}

			return *this;
		}

		bool Rect::isValid() const {
			return size.x > 0 && size.y > 0;
		}
	}
}
