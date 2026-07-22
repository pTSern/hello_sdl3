#pragma once
#include "math/Vec2.hpp"
#include <cmath>

inline bool Vec2::is_zero() const {
    return x == 0.F && y == 0.F;
}

inline bool Vec2::is_one() const {
    return x == 1.F && y == 1.F;
}

inline void Vec2::set(float xx, float yy) {
    x = xx;
    y = yy;
}

inline void Vec2::set(const Vec2 &other) {
    x = other.x;
    y = other.y;
}

inline void Vec2::add(float xx, float yy) {
    x += xx;
    y += yy;
}

inline void Vec2::add(const Vec2 &other) {
    x += other.x;
    y += other.y;
}

inline void Vec2::scale(float scalar) {
    x *= scalar;
    y *= scalar;
}

inline void Vec2::scale(float xx, float yy) {
    x *= xx;
    y *= yy;
}

inline void Vec2::scale(const Vec2 &other) {
    x *= other.x;
    y *= other.y;
}

inline float Vec2::length_squared() const {
    return (x * x + y * y);
}

inline Vec2::Vec2() : x(0.F), y(0.F) {  }
inline Vec2::Vec2(float xx, float yy) : x(xx), y(yy) {}
inline Vec2::Vec2(const Vec2 &other) {
    set(other);
}

inline Vec2::~Vec2() {

}
