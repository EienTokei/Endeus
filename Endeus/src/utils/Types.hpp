#pragma once

namespace endeus {

	struct Vec2f {
		float x = 0.f;
		float y = 0.f;

		bool operator==(const Vec2f& other) const {
			return this->x == other.x && this->y == other.y;
		}

		bool operator!=(const Vec2f& other) const {
			return !(*this == other);
		}
	};

	struct Rect {
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;

		bool operator==(const Rect& other) const {
			return this->x == other.x && this->y == other.y
				&& this->width == other.width && this->height == other.height;
		}

		bool operator!=(const Rect& other) const {
			return !(*this == other);
		}
	};

} // namespace endeus
