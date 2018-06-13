#pragma once

struct YawPitchRoll {
    YawPitchRoll() : yaw(0), pitch(0), roll(0){};
    YawPitchRoll(const double y, const double p, const double r)
        : yaw(y), pitch(p), roll(r){};
    double yaw, pitch, roll;
};
