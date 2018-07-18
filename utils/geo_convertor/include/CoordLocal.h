#pragma once

class PoseWgs84;
class CoordWgs84;
class CoordEnu;
class SphericalCoordinates;

class CoordLocal {
public:
    CoordLocal();
    CoordLocal(double x, double y, double z);

    CoordEnu toEnu(const PoseWgs84& reference) const;
    CoordWgs84 toWgs84(const PoseWgs84& reference) const;
    SphericalCoordinates toSphericalCoordinates() const;
    // Return Euclidean distance to another coordinate
    double distTo(const CoordLocal& other) const;
    // Return heading (angle in x-y-plane) to other coordinate
    double headingTo(const CoordLocal& other) const;
    // Return inclination ("pitch", angle between movement and x-y-plane) to
    // other coordinate
    double elevationTo(const CoordLocal& other) const;

    bool operator==(const CoordLocal& other) const;
    bool operator!=(const CoordLocal& other) const;
    virtual bool isInitialized() const;

    double x;
    double y;
    double z;
};