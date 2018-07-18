#include "PoseLocal.h"

PoseLocal::PoseLocal() : CoordLocal() {
}

PoseLocal::PoseLocal(double x, double y, double z) : CoordLocal(x, y, z) {
}

PoseLocal::PoseLocal(double x, double y, double z, double heading, double pitch,
                     double roll)
    : CoordLocal(x, y, z), heading(heading), pitch(pitch), roll(roll) {
}
