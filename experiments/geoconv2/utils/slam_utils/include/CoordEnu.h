#pragma once

class CoordWgs84;
class CoordEcef;
class CoordLocal;
class PoseWgs84;

class CoordEnu
{
public:
    CoordEnu();
    CoordEnu(double e, double n, double u);

    CoordEcef
    toEcef(const CoordEcef& refEcef, const CoordWgs84& refWgs84) const;
    CoordEcef toEcef(const CoordEcef& refEcef) const;
    CoordEcef toEcef(const CoordWgs84& refWgs84) const;

    CoordLocal toCoordLocal(const PoseWgs84& reference) const;
    CoordWgs84 toWgs84(const CoordWgs84& reference) const;

    double east;
    double north;
    double up;
};