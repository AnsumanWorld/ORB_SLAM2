#include "precompile.hpp"

#include "util/util.hpp"

double gaussian_noise() {
    static auto seed =
        std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine generator(seed);

    static const double mean = 0.0;
    static const double stddev = 0.5;
    static std::normal_distribution<double> distribution(mean, stddev);
    return distribution(generator);
}

int main(int, char**) {
    using namespace ORB_SLAM2::experiments;
    const char seperator = ' ';
    std::size_t num_pts = 100;
    auto points_inner = util::generate_circle(num_pts, 5.0, 0.0, 0.0);
    auto points_outer = util::generate_circle(num_pts, 10.0, 0.0, 0.0);
    std::ofstream ofile("KeyFrameTrajectory.txt");
    const double dummy_timestamp = 999.99;
    double z, a, b, c, d;
    z = a = b = c = d = 0.0;
    for (std::size_t i = 0; i < num_pts; ++i) {
        ofile << dummy_timestamp << seperator << points_inner[i].first
              << seperator << points_inner[i].second << seperator << z
              << seperator << a << seperator << b << seperator << c << seperator
              << d << seperator << points_outer[i].first << seperator
              << points_outer[i].second << '\n';
    }

    std::ofstream ofile2("KeyFrameTrajectory-NoisyGPS.txt");
    for (std::size_t i = 0; i < num_pts; ++i) {
        ofile2 << dummy_timestamp << seperator << points_inner[i].first
               << seperator << points_inner[i].second << seperator << z
               << seperator << a << seperator << b << seperator << c
               << seperator << d << seperator
               << points_outer[i].first + gaussian_noise() << seperator
               << points_outer[i].second + gaussian_noise() << '\n';
    }
    return 0;
}
