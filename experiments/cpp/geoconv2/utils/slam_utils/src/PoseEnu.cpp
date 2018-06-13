#include "PoseEnu.h"
#include "CoordEcef.h"
#include "CoordEnu.h"
#include "CoordWgs84.h"

PoseEnu::PoseEnu() : CoordEnu() {
}

PoseEnu::PoseEnu(double e, double n, double u, double heading, double pitch,
                 double roll)
    : CoordEnu(e, n, u), heading(heading), pitch(pitch), roll(roll) {
}
