#include <fstream>
#include <iomanip>
#include "utils/slam_utils/include/PoseWgs84.h"
#include "utils/slam_utils/include/CoordWgs84.h"
#include "utils/slam_utils/include/CoordLocal.h"
#include "utils/slam_utils/include/helper.h"

int main(int argc, char** argv)
{
    if (argc < 9)
    {
        printf("Please use like this:  kitti_gt_to_global.exe <path_to_kitti_gt_file> <tf_lat(deg)> <tf_lon(deg)> <tf_alt(m)> <tf_roll(rad)> <tf_pitch(rad)> <tf_yaw(rad)> <output_file>");
        return -1;
    }

    std::ifstream input_file(argv[1]);

    PoseWgs84 reference_pose(deg2rad(std::stod(argv[2])), deg2rad(std::stod(argv[3])), std::stod(argv[4]), std::stod(argv[7]), std::stod(argv[6]), std::stod(argv[5]));

    std::ofstream output_file(argv[8]);

    // Read the input file
    std::string line;
    int count = 0;
    while (std::getline(input_file, line))
    {
        std::stringstream ss(line);
        double x, y, z;
        double unused;
        for (int i = 0; i < 3; ++i) ss >> unused;
        ss >> x;
        for (int i = 0; i < 3; ++i) ss >> unused;
        ss >> y;
        for (int i = 0; i < 3; ++i) ss >> unused;
        ss >> z;
        // Notice that this is in crazy computer vision coordinates. CoordLocal uses ISO vehicle coordinates (and I still don't know the orientation of the vertical axis)
        CoordLocal coord(z, -x, -y);
        CoordWgs84 coord_global = coord.toWgs84(reference_pose);

        output_file << std::setprecision(15)
            << rad2deg(coord_global.getLat()) << ", "
            << rad2deg(coord_global.getLon()) << ", " << coord_global.getAlt() << "\n";

        printf("%i ", count++);
    }
    return 0;
}