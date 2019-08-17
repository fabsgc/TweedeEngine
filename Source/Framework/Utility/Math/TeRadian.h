#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

namespace te
{
    class TE_UTILITY_EXPORT Radian
    {
    public:
        constexpr Radian() = default;
        constexpr Radian(const Radian&) = default;
        constexpr Radian& operator= (const Radian&) = default;

        constexpr explicit Radian(float r) : _rad(r) {}
        constexpr Radian& operator= (const float& f) { _rad = f; return *this; }

        Radian(const Degree& d);
        Radian& operator= (const Degree& d);

        /** Returns the value of the angle in degrees. */
        float ValueDegrees() const;

        /** Returns the value of the angle in radians. */
        constexpr float ValueRadians() const { return _rad; }

        /** Wraps the angle in [0, 2 *  PI) range. */
        Radian Wrap();

        const Radian& operator+ () const { return *this; }
        Radian operator+ (const Radian& r) const { return Radian(_rad + r._rad); }
        Radian operator+ (const Degree& d) const;
        Radian& operator+= (const Radian& r) { _rad += r._rad; return *this; }
        Radian& operator+= (const Degree& d);
        Radian operator- () const { return Radian(-_rad); }
        Radian operator- (const Radian& r) const { return Radian(_rad - r._rad); }
        Radian operator- (const Degree& d) const;
        Radian& operator-= (const Radian& r) { _rad -= r._rad; return *this; }
        Radian& operator-= (const Degree& d);
        Radian operator* (float f) const { return Radian(_rad * f); }
        Radian operator* (const Radian& f) const { return Radian(_rad * f._rad); }
        Radian& operator*= (float f) { _rad *= f; return *this; }
        Radian operator/ (float f) const { return Radian(_rad / f); }
        Radian& operator/= (float f) { _rad /= f; return *this; }

        friend Radian operator* (float lhs, const Radian& rhs) { return Radian(lhs * rhs._rad); }
        friend Radian operator/ (float lhs, const Radian& rhs) { return Radian(lhs / rhs._rad); }
        friend Radian operator+ (Radian& lhs, float rhs) { return Radian(lhs._rad + rhs); }
        friend Radian operator+ (float lhs, const Radian& rhs) { return Radian(lhs + rhs._rad); }
        friend Radian operator- (const Radian& lhs, float rhs) { return Radian(lhs._rad - rhs); }
        friend Radian operator- (const float lhs, const Radian& rhs) { return Radian(lhs - rhs._rad); }

        bool operator<  (const Radian& r) const { return _rad < r._rad; }
        bool operator<= (const Radian& r) const { return _rad <= r._rad; }
        bool operator== (const Radian& r) const { return _rad == r._rad; }
        bool operator!= (const Radian& r) const { return _rad != r._rad; }
        bool operator>= (const Radian& r) const { return _rad >= r._rad; }
        bool operator>  (const Radian& r) const { return _rad > r._rad; }

    private:
        float _rad = 0.0f;
    };
}