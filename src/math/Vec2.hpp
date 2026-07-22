#pragma once

class Vec2 {
    public:
        float x{0.F};
        float y{0.F};

        float angle(const Vec2 &other) const;

        inline bool is_zero() const;
        inline bool is_one() const;

        inline void set(float x, float y);
        inline void set(const Vec2 &other);

        inline void add(float x, float y);
        inline void add(const Vec2 &other);
        inline void scale(float scalar);
        inline void scale(float x, float y);
        inline void scale(const Vec2 &other);

        void clamp();

        float length() const;
        inline float length_squared() const;

        inline Vec2(float x, float y);
        inline Vec2(const Vec2 &other);
        inline Vec2();

        void log();

        ~Vec2();
};

#include "Vec2.ipp"
