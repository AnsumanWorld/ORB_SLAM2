#pragma once

namespace constants {
const double a = 6378137.0;                    // Semi major axis of Earth [m]
const double b = 6356752.314;                  // Semi minor axis of Earth [m]
const double esqr = 1 - pow(b, 2) / pow(a, 2); // Eccentricity
} // namespace constants