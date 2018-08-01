#include "ext/run_tracking.h"
#include "ext/slam_object.h"
#include "ext/ds_kitty.h"
using namespace ORB_SLAM2;

int main(int argc, char** argv)
{
    try {
        ext::ds_kitti_args args{ argc, argv };
        ext::slam_object slam{ args.get_val("orbvoc"), args.get_val("setting") };
        auto ds = ext::make_data_source<ext::ds_kitty>(args);
        cv::FileStorage fSettings(args.get_val("setting"), cv::FileStorage::READ);
        double fps = fSettings["Camera.fps"]; 
        ext::run_tracking(slam, ds, fps);
    }
    catch (std::exception ex)
    {
        std::cerr << ex.what() << std::endl;
    }

}
