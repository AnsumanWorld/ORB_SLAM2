#include "stdafx.h"
#include "ext/run_tracking.h"
#include "ext/slam_object.h"
#include "ext/ds_tsr.h"
using namespace ORB_SLAM2;

int main(int argc, char** argv)
{
    try {
        ext::ds_tsr_args args{ argc, argv };
        ext::slam_object slam{ args.orb_vocabulary().string(), args.camera_settings().string() };
        auto ds = ext::make_data_source<ext::ds_tsr>(args);
        cv::FileStorage fSettings{args.camera_settings().string(), cv::FileStorage::READ};
        double fps = fSettings["Camera.fps"];
        ext::run_tracking(slam, ds, fps);
    }
    catch (std::exception ex)
    {
        std::cerr << ex.what() << std::endl;
    }
}
