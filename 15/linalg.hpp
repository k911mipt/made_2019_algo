#pragma once
#ifndef LINALG_H_
#define LINALG_H_

#include <random>

namespace {
    const double pi = 3.14159265358979323;
}

namespace linalg {
    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    struct Vector {
    //private:
        double x = 0;
        double y = 0;
    public:

        Vector() = default;
        Vector(double _x, double _y) :
            x(_x),
            y(_y)
        {
        }
        Vector(std::default_random_engine& engine, std::uniform_int_distribution<uint64_t>& distribution) :
            Vector(
                static_cast<double>(distribution(engine)) / distribution.max(),
                static_cast<double>(distribution(engine)) / distribution.max(),
                std::true_type{})
        {
        }

        Vector& operator-=(const Vector& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }
        Vector operator-(const Vector& rhs) const& {
            Vector result(*this);
            result -= rhs;
            return result;
        }

        double Norm() { return sqrt(x * x + y * y); }

        int DirectionFrom(const Vector& rhs) {
            return sgn(Cross(*this, rhs));
        }
        static double Dot(const Vector& lhs, const Vector& rhs) {

        }
        static double Cross(const Vector& lhs, const Vector& rhs) {
            return lhs.x * rhs.y - rhs.x * lhs.y;
        }

    private:
        Vector(double r, double phi, std::true_type) :
            Vector(
                2 * pi * phi,
                sqrt(-2 * log(r)),
                std::false_type{})
        {
        }
        Vector(double two_pi_phi, double sqrt_minus_two_ln_r, std::false_type) :
            Vector(
                cos(two_pi_phi)* sqrt_minus_two_ln_r,
                sin(two_pi_phi)* sqrt_minus_two_ln_r
            )
        {
        }
    };

    //double distance(const Vector& a, const Vector& b) {
    //}
}
#endif // !LINALG_H_