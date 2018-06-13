#include "CoordEcef.h"
#include "CoordEnu.h"
#include "CoordWgs84.h"
#include "coord_constants.h"

CoordEcef::CoordEcef() {
}

CoordEcef::CoordEcef(double X, double Y, double Z) : X(X), Y(Y), Z(Z) {
}

CoordEnu
CoordEcef::toEnu(const CoordEcef& refEcef, const CoordWgs84& refWgs84) const {
    const double dX = X - refEcef.X;
    const double dY = Y - refEcef.Y;
    const double dZ = Z - refEcef.Z;
    const double& phir = refWgs84.getLat();
    const double& lambdar = refWgs84.getLon();

    const double e = -sin(lambdar) * dX + cos(lambdar) * dY;
    const double n = -sin(phir) * cos(lambdar) * dX -
                     sin(phir) * sin(lambdar) * dY + cos(phir) * dZ;
    const double u = cos(phir) * cos(lambdar) * dX +
                     cos(phir) * sin(lambdar) * dY + sin(phir) * dZ;
    return CoordEnu(e, n, u);
}

CoordEnu CoordEcef::toEnu(const CoordEcef& refEcef) const {
    return this->toEnu(refEcef, refEcef.toWgs84());
}

CoordEnu CoordEcef::toEnu(const CoordWgs84& refWgs84) const {
    return this->toEnu(refWgs84.toEcef(), refWgs84);
}

CoordWgs84 CoordEcef::toWgs84() const {
    // Following section 2.2.1 of http://www.epsg.org/Portals/0/373-07-2.pdf
    // IOGP Publication 373-7-2 – Geomatics Guidance Note number 7, part 2 –
    // September 2016
    const double eps = constants::esqr / (1 - constants::esqr);
    const double p = sqrt(X * X + Y * Y);
    const double q = atan(Z * constants::a / (p * constants::b));
    const double lat =
        atan((Z + eps * constants::b * pow(sin(q), 3)) /
             (p - constants::esqr * constants::a * pow(cos(q), 3)));
    const double lon = atan2(Y, X);

    const double nu =
        constants::a / sqrt(1 - constants::esqr * pow(sin(lat), 2));
    const double alt = p / cos(lat) - nu;

    return CoordWgs84(lat, lon, alt);
}
