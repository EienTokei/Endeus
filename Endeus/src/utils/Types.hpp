#pragma once

namespace endeus {

	struct Vec2f {
		float x = 0.f;
		float y = 0.f;

		// 非成员函数保证左右操作数的对称性和隐式转换的一致性
		friend bool operator==(const Vec2f& a, const Vec2f& b) {
			return a.x == b.x && a.y == b.y; 
		}

		friend bool operator!=(const Vec2f& a, const Vec2f& b) {
			return !(a == b);
		}

		Vec2f& operator+=(const Vec2f& other) {
			x += other.x;
			y += other.y;
			return *this;
		}

		Vec2f& operator-=(const Vec2f& other) {
			x -= other.x;
			y -= other.y;
			return *this;
		}

		Vec2f& operator*=(float scalar) {
			x *= scalar;
			y *= scalar;
			return *this;
		}
	};

	// 对于外部非成员运算符重载, 没有标记 inline 的话每个包含它的 .cpp 都会生成一份函数体，导致多重定义
	inline Vec2f operator+(Vec2f a, const Vec2f& b) { a += b; return a; }
	inline Vec2f operator-(Vec2f a, const Vec2f& b) { a -= b; return a; }
	inline Vec2f operator*(Vec2f v, float scalar) { v *= scalar; return v; }
	inline Vec2f operator*(float scalar, Vec2f v) { v *= scalar; return v; }

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
