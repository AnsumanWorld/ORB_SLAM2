#include "ext/run_tracking.h"
#include "ext/slam_object.h"
#include "ext/ds_dashcam.h"
using namespace ORB_SLAM2;

int main(int argc, char** argv)
{
	try {
		ext::ds_dashcam_args args{ argc, argv };
		ext::slam_object slam{ argc, argv };
		ext::run_tracking(slam, ext::ds_dashcam{ args });
	}
	catch (std::exception ex)
	{
		std::cerr << ex.what() << std::endl;
	}
}
