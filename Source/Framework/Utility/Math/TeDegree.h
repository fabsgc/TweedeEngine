#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

namespace te
{
    class Radian;
    
     /**
      * Wrapper class which indicates a given angle value is in degrees.
      *
      * @note
      * Degree values are interchangeable with Radian values, and conversions will be done automatically between them.
      */
    class TE_UTILITY_EXPORT Degree
    {
    public:
        constexpr Degree() = default;
        constexpr Degree(const Degree& d) = default;
        constexpr Degree&operator= (const Degree& d) = default;

        constexpr explicit Degree(float d) : _deg(d) {}
        constexpr Degree& operator= (const float& f) { _deg = f; return *this; }

        Degree(const Radian& r);
        Degree& operator= (const Radian& r);

        /** Returns the value of the angle in degrees. */
        constexpr float ValueDegrees() const { return _deg; }

        /** Returns the value of the angle in radians. */
        float ValueRadians() const;

        /** Wraps the angle in [0, 360) range */
        Degree Wrap();

        const Degree& operator+ () const { return *this; }
        Degree operator+ (const Degree& d) const { return Degree(_deg + d._deg); }
        Degree operator+ (const Radian& r) const;
        Degree& operator+= (const Degree& d) { _deg += d._deg; return *this; }
        Degree& operator+= (const Radian& r);
        Degree operator- () const { return Degree(-_deg); }
        Degree operator- (const Degree& d) const { return Degree(_deg - d._deg); }
        Degree operator- (const Radian& r) const;
        Degree& operator-= (const Degree& d) { _deg -= d._deg; return *this; }
        Degree& operator-= (const Radian& r);
        Degree operator* (float f) const { return Degree(_deg * f); }
        Degree operator* (const Degree& f) const { return Degree(_deg * f._deg); }
        Degree& operator*= (float f) { _deg *= f; return *this; }
        Degree operator/ (float f) const { return Degree(_deg / f); }
        Degree& operator/= (float f) { _deg /= f; return *this; }

        friend Degree operator* (float lhs, const Degree& rhs) { return Degree(lhs * rhs._deg); }
        friend Degree operator/ (float lhs, const Degree& rhs) { return Degree(lhs / rhs._deg); }
        friend Degree operator+ (Degree& lhs, float rhs) { return Degree(lhs._deg + rhs); }
        friend Degree operator+ (float lhs, const Degree& rhs) { return Degree(lhs + rhs._deg); }
        friend Degree operator- (const Degree& lhs, float rhs) { return Degree(lhs._deg - rhs); }
        friend Degree operator- (const float lhs, const Degree& rhs) { return Degree(lhs - rhs._deg); }

        bool operator<  (const Degree& d) const { return _deg < d._deg; }
        bool operator<= (const Degree& d) const { return _deg <= d._deg; }
        bool operator== (const Degree& d) const { return _deg == d._deg; }
        bool operator!= (const Degree& d) const { return _deg != d._deg; }
        bool operator>= (const Degree& d) const { return _deg >= d._deg; }
        bool operator>  (const Degree& d) const { return _deg > d._deg; }

    private:
        float _deg = 0.0f;
    };
}
