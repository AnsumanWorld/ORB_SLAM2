#include "precompile.hpp"

#include "types.hpp"

namespace ORB_SLAM2 {
namespace experiments {
pose::pose() : _position(3, 0.0), _orientation(4, 0.0), _gps(2, 0.0) {
}

void pose::read(std::istream& is) {
    auto timestamp{0.0};
    is >> timestamp;
    for (int i = 0; i < 3; ++i)
        is >> _position[i];
    for (int i = 0; i < 4; ++i)
        is >> _orientation[i];
    for (int i = 0; i < 2; ++i)
        is >> _gps[i];
}

void pose::write(std::ostream& os) {
    for (int i = 0; i < 3; ++i)
        os << _position[i] << " ";
    for (int i = 0; i < 4; ++i)
        os << _orientation[i] << " ";
    for (int i = 0; i < 2; ++i)
        os << _gps[i] << " ";
}

double pose::x() const {
    return _position[0];
}

void pose::x(double val) {
    _position[0] = val;
}

double pose::y() const {
    return _position[1];
}

void pose::y(double val) {
    _position[1] = val;
}

double pose::z() const {
    return _position[2];
}

void pose::z(double val) {
    _position[2] = val;
}

double pose::a() const {
    return _orientation[0];
}

void pose::a(double val) {
    _orientation[0] = val;
}

double pose::b() const {
    return _orientation[1];
}

void pose::b(double val) {
    _orientation[1] = val;
}

double pose::c() const {
    return _orientation[2];
}

void pose::c(double val) {
    _orientation[2] = val;
}

double pose::d() const {
    return _orientation[3];
}

void pose::d(double val) {
    _orientation[3] = val;
}

double pose::latitude() const {
    return _gps[0];
}

void pose::latitude(double val) {
    _gps[0] = val;
}

double pose::longitude() const {
    return _gps[1];
}

void pose::longitude(double val) {
    _gps[1] = val;
}

bool pose::has_gps() const {
    return latitude() != 0.0 && longitude() != 0.0;
}
} // namespace experiments
} // namespace ORB_SLAM2
