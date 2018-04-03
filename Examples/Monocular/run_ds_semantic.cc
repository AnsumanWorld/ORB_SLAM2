#include "ext/run_tracking.h"
#include "ext/slam_object.h"
#include "ext/ds_semantic.h"
using namespace ORB_SLAM2;

int main(int argc, char** argv)
{
	try {
		ext::ds_semantic_args args{ argc, argv };
		ext::slam_object slam{ argc, argv };
		ext::run_tracking(slam, ext::ds_semantic{ args });
	}
	catch (std::exception ex)
	{
		std::cerr << ex.what() << std::endl;
	}
}
