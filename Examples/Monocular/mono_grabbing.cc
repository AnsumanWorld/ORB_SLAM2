//#include "System.h"

#include <opencv2/opencv.hpp>
#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    enum class status : int
    {
        success = 0,
        failure = -1
    };

    int ret_val{static_cast<int>(status::success)};

    if (3 == argc) {
        std::string vocabulary_file{argv[1]};
        std::string settings_file{argv[2]};

        auto const camera_id = 0;
        auto const camera_window_name = "grabber view";
        // Try to use default camera
        cv::VideoCapture video_capture{camera_id};

        if (video_capture.isOpened()) {
            cv::Mat grabbed_img;
            cv::namedWindow(camera_window_name, 1);
            cv::Mat frame;
            
            // get a new frame from camera
            while (video_capture.read(frame)) { 
                cv::cvtColor(frame, grabbed_img, cv::COLOR_BGR2GRAY);
                cv::imshow(camera_window_name, grabbed_img);
                if (cv::waitKey(30) >= 0) {
                    break;
                }
            }

            cv::destroyWindow(camera_window_name);

            //ORB_SLAM2::System slam(vocabulary_file, settings_file,
            //    ORB_SLAM2::System::MONOCULAR);
            //cv::Mat frame;
            //while (video_capture.read(frame)) {
            //    auto current_video_time = video_capture.get(CV_CAP_PROP_POS_MSEC);
            //    slam.TrackMonocular(frame, current_video_time);
            //    //std::this_thread::sleep_for(std::chrono::milliseconds(30));
            //}

            //slam.Shutdown();
            //slam.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");
        } else {
            std::cerr << "Cannot grab from " << camera_id << '\n';
            ret_val = static_cast<int>(status::failure);
        }
    } else {
        std::cerr << "Invalid input\n";
        std::cerr
            << "Usage: mono_grabbing VocabularyFile SettingsFile\n";
        std::cerr << "Example: /ORB_SLAM2/Examples/Monocular/mono_grabbing "
            << "/ORB_SLAM2/Vocabulary/ORBVoc.txt "
            << "/ORB_SLAM2/Examples/Monocular/TUM1.yaml\n";
        ret_val = static_cast<int>(status::failure);
    }

    return ret_val;
}
