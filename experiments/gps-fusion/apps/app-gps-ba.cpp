#include "precompile.hpp"

#include "gps_ba.hpp"
#include "macros.hpp"

int main(int argc, char** argv)
{
    LOG_SCOPE;

    if (argc != 2)
        throw std::invalid_argument("usage: app-gps-ba <config.json>");

    std::string filepath = argv[1];
    std::ifstream ifile(filepath);
    if (false == ifile.good())
        throw std::runtime_error("unable to open file " + filepath);

    ORB_SLAM2::experiments::config cfg(ifile);
    ORB_SLAM2::experiments::gps_ba ba_(cfg);
    ba_.fuse_gps();

    return 0;
}
