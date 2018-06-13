#pragma once

#include <limits>

class CoordEnu;
class CoordEcef;
class CoordLocal;
class PoseWgs84;

class CoordWgs84 {
public:
    CoordWgs84();
    CoordWgs84(double lat, double lon, double alt = 0);

    CoordEcef toEcef() const;
    CoordEnu toEnu(const CoordWgs84& reference) const;
    CoordLocal toCoordLocal(const PoseWgs84& reference) const;

    double getLat() const;
    double getLon() const;
    double getAlt() const;
    virtual bool isInitialized() const;

    bool operator==(const CoordWgs84& other) const;
    bool operator!=(const CoordWgs84& other) const;

private:
    double lat;
    double lon;
    double alt;
};