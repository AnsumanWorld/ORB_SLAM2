#include <Eigen/Core>
#include <Eigen/StdVector>

#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

#include <iostream>
#include <tuple>
#include <vector>

using geo_list_t = std::vector<std::tuple<double, double, double>>;

Eigen::Matrix3d geo_cov(geo_list_t& geos) {
    auto n = geos.size();
    cv::Mat_<double> samples(n, 3);
    for (std::size_t i = 0; i < n; ++i) {
        samples(i, 0) = std::get<0>(geos[i]);
        samples(i, 1) = std::get<1>(geos[i]);
        samples(i, 2) = std::get<2>(geos[i]);
    }

    cv::Mat cov, mu;
    cv::calcCovarMatrix(samples, cov, mu, CV_COVAR_NORMAL | CV_COVAR_ROWS);
    cov = cov / (samples.rows - 1);
    Eigen::Matrix3d eret;
    cv::cv2eigen(cov, eret);
    return eret;
}

int main(int, char**) {
    cv::Mat_<double> samples =
        (cv::Mat_<double>(3, 2) << 500.0, 350.2, 500.5, 355.8, 498.7, 352.0);

    geo_list_t geos;
    geos.push_back(std::make_tuple(500.0, 350.2, 1));
    geos.push_back(std::make_tuple(500.5, 355.8, 2));
    geos.push_back(std::make_tuple(498.7, 352.0, 3));

    std::cout << geo_cov(geos) << std::endl;

    return 0;
}
