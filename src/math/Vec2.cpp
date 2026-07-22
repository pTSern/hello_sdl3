#include "math/Vec2.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

float Vec2::length() const {
    return std::sqrt(x * x + y * y);
}

void Vec2::clamp() {
    x = std::clamp(x, 0.F, 1.F);
    y = std::clamp(y, 0.F, 1.F);
}

float Vec2::angle(const Vec2 &other) const {
    float len_sq = length_squared();
    float other_len_sq = other.length_squared();
    if (len_sq == 0.F || other_len_sq == 0.F) return 0.F;
    float dot = x * other.x + y * other.y;
    return std::acos(dot / std::sqrt(len_sq * other_len_sq));
}

void Vec2::log() {
    std::cout << "Vec2: " << this << "\nx: " << x << "\ny: " << y << std::endl;
}
