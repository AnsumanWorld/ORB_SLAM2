#include "stdafx.h"
#include "ext_ds/ds_tsr.h"
#include "ext/run_tracking.h"
#include "ext/slam_object.h"
#include <opencv2/core.hpp>

using namespace ORB_SLAM2;

int main(int argc, char* argv[])
{
    try {
        ext::ds_tsr_args args{argc, argv};
        ext::slam_object slam{args.orb_vocabulary().string(), args.camera_settings().string()};
        
        cv::FileStorage fSettings{args.camera_settings().string(), cv::FileStorage::READ};
        int max_fps = fSettings["Camera.fps"];

        ext::run_tracking(slam, ext::make_data_source<ext::ds_tsr>(args), max_fps);
    } catch (std::exception ex) {
        std::cerr << ex.what() << std::endl;
    }
}
