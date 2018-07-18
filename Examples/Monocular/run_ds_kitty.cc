#include "ext/run_tracking.h"
#include "ext/slam_object.h"
#include "ext/ds_kitty.h"
using namespace ORB_SLAM2;

int main(int argc, char** argv)
{
    try {
        ext::ds_kitty_args args{ argc, argv };
        ext::slam_object slam{ argc, argv };
        //1e6 - hardcorded default wait time set by ramlur
        ext::run_tracking(slam, ext::ds_kitty{ args }, 1e6);
    }
    catch (std::exception ex)
    {
        std::cerr << ex.what() << std::endl;
    }

}
