#pragma once

class CoordLocal;

// Polar coordinates
// Assuming incl = 0, azi = 0   (front) ==> X-Axis
// Assuming incl = 0, azi = pi/2 (left) ==> Y-Axis
// Assuming incl = pi/2          (up)   ==> Z-Axis
class SphericalCoordinates
{
public:
    SphericalCoordinates();
    SphericalCoordinates(double dist, double inclination, double azimuth);

    CoordLocal toCoordLocal() const;

    double distance;
    double inclination;
    double azimuth;
};