#include "helper.h"
#include <ctime>

std::mt19937 generator_h(time(0));
std::normal_distribution<double> distribution_h(0, 1);

double deg2rad(const double deg) {
    return deg * slam_utils::slam_utils_constants::pi / 180.0;
}
double rad2deg(const double rad) {
    return rad * 180.0 / slam_utils::slam_utils_constants::pi;
}

double wrap2pi(double in) {
    while (in > slam_utils::slam_utils_constants::pi)
        in -= 2 * slam_utils::slam_utils_constants::pi;
    while (in < -slam_utils::slam_utils_constants::pi)
        in += 2 * slam_utils::slam_utils_constants::pi;
    return in;
}

double new_sample(const double old_mu1, const double old_var1,
                  const double old_mu2, const double old_var2) {
    const double z = old_var1 + old_var2;
    const double new_mu = (old_var1 * old_mu2 + old_var2 * old_mu1) / z;
    const double new_var = old_var1 * old_var2 / z;
    return distribution_h(generator_h) * sqrt(new_var) + new_mu;
}
