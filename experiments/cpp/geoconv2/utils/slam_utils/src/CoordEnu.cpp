#include "CoordEnu.h"
#include "CoordEcef.h"
#include "CoordLocal.h"
#include "PoseWgs84.h"

CoordEnu::CoordEnu() {
}

CoordEnu::CoordEnu(double e, double n, double u) : east(e), north(n), up(u) {
}

CoordEcef
CoordEnu::toEcef(const CoordEcef& refEcef, const CoordWgs84& refWgs84) const {
    const double& phir = refWgs84.getLat();
    const double& lambdar = refWgs84.getLon();
    const double X = -sin(lambdar) * east - sin(phir) * cos(lambdar) * north +
                     cos(phir) * cos(lambdar) * up + refEcef.X;
    const double Y = cos(lambdar) * east - sin(phir) * sin(lambdar) * north +
                     cos(phir) * sin(lambdar) * up + refEcef.Y;
    const double Z = cos(phir) * north + sin(phir) * up + refEcef.Z;
    return CoordEcef(X, Y, Z);
}

CoordEcef CoordEnu::toEcef(const CoordEcef& refEcef) const {
    return this->toEcef(refEcef, refEcef.toWgs84());
}

CoordEcef CoordEnu::toEcef(const CoordWgs84& refWgs84) const {
    return this->toEcef(refWgs84.toEcef(), refWgs84);
}

CoordLocal CoordEnu::toCoordLocal(const PoseWgs84& reference) const {
    // Rotate the coordinate according to reference rotation
    const double& phi = reference.getRoll();
    const double& psi = reference.getHeading();
    const double& theta = reference.getPitch();
    const double sinphi = sin(phi);
    const double cosphi = cos(phi);
    const double sinpsi = sin(psi);
    const double cospsi = cos(psi);
    const double sintheta = sin(theta);
    const double costheta = cos(theta);

    const double x =
        cospsi * costheta * east + sinpsi * costheta * north - sintheta * up;
    const double y = (sinpsi * cospsi * sintheta - cosphi * sinpsi) * east +
                     (sinphi * sinpsi * sintheta + cosphi * cospsi) * north +
                     sinphi * costheta * up;
    const double z = (cosphi * cospsi * sintheta + sinphi * sinpsi) * east +
                     (cosphi * sinpsi * sintheta - sinphi * cospsi) * north +
                     cosphi * costheta * up;

    return CoordLocal(x, y, z);
}

CoordWgs84 CoordEnu::toWgs84(const CoordWgs84& reference) const {
    return this->toEcef(reference).toWgs84();
}
