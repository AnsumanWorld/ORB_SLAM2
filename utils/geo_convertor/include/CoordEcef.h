#pragma once

#include <cmath>

class CoordEnu;
class CoordWgs84;

class CoordEcef {
public:
    CoordEcef();
    CoordEcef(double X, double Y, double Z);

    CoordEnu toEnu(const CoordEcef& refEcef, const CoordWgs84& refWgs84) const;
    CoordEnu toEnu(const CoordEcef& refEcef) const;
    CoordEnu toEnu(const CoordWgs84& refWgs84) const;

    CoordWgs84 toWgs84() const;

    double X;
    double Y;
    double Z;
};