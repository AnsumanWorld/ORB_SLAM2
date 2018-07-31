#include "ext/run_tracking.h"
#include "ext/slam_object.h"
#include "ext/ds_traficsign.h"
using namespace ORB_SLAM2;

int main(int argc, char** argv)
{
    try {
        ext::ds_traficsign_args args{ argc, argv };
        ext::slam_object slam{ argc, argv };
        auto ds = ext::make_data_source<ext::ds_traficsign>(args);
        cv::FileStorage fSettings(argv[2], cv::FileStorage::READ);
        double fps = fSettings["Camera.fps"];
        ext::run_tracking(slam, ds, fps);
    }
    catch (std::exception ex)
    {
        std::cerr << ex.what() << std::endl;
    }
}
