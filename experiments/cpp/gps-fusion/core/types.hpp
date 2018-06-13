#pragma once

#include "precompile.hpp"

namespace ORB_SLAM2 {
namespace experiments {
class pose {
public:
    pose();

    // IO
    void read(std::istream&);
    void write(std::ostream&);

    // Position
    double x() const;
    void x(double);
    double y() const;
    void y(double);
    double z() const;
    void z(double);

    // Orientation (Quaternion)
    double a() const;
    void a(double);
    double b() const;
    void b(double);
    double c() const;
    void c(double);
    double d() const;
    void d(double);

    // Geo
    double latitude() const;
    void latitude(double);
    double longitude() const;
    void longitude(double);

    bool has_gps() const;

private:
    std::vector<double> _position;
    std::vector<double> _orientation;
    std::vector<double> _gps;
};
} // namespace experiments
} // namespace ORB_SLAM2
