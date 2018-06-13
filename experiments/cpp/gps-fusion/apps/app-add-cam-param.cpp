#include "precompile.hpp"

int main(int argc, char** argv) {
    if (argc != 6)
        throw std::invalid_argument(
            "usage: app-add-cam-param <input.g2o> <fx> <fy> <cx> <cy>");

    std::string input_g2o{argv[1]};
    std::string fx{argv[2]};
    std::string fy{argv[3]};
    std::string cx{argv[4]};
    std::string cy{argv[5]};

    std::string edge_name = "EDGE_SE3_PROJECT_XYZ:EXPMAP";
    std::string seperator = " ";
    std::ifstream ifile(input_g2o.c_str());
    if (false == ifile.good())
        throw std::runtime_error("unable to open g2o file");

    std::ofstream ofile("added_camera_params.g2o");
    std::string each_line;
    while (std::getline(ifile, each_line)) {
        if (0 == each_line.find(edge_name)) {
            each_line.pop_back();
            each_line += seperator + fx + seperator + fy + seperator + cx +
                         seperator + cy;
        }

        ofile << each_line << '\n';
    }

    return 0;
}
