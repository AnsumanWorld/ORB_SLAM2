#include "ORBExtractor.h"

#include <boost/filesystem.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

namespace fs = boost::filesystem;

void extract_from_video(const std::string& video_file,
                        ORB_SLAM2::ORBextractor& extractor) {
    std::cout << "Extracting ORB from video file\n";
    cv::VideoCapture video_capture(video_file);
    cv::Mat frame;
    std::size_t total_kp{0};
    while (video_capture.read(frame)) {
        std::vector<cv::KeyPoint> key_points;
        cv::Mat mask;
        cv::Mat descriptors;
        extractor(frame, mask, key_points, descriptors);
        total_kp += key_points.size();
    }

    std::cout << "Total key-points = " << total_kp << std::endl;
}

void extract_from_images(const std::string& folder,
                         ORB_SLAM2::ORBextractor& extractor) {
    std::cout << "Extracting ORB from image folder\n";
    std::vector<fs::path> image_files;
    std::copy(fs::directory_iterator(folder), fs::directory_iterator(),
              std::back_inserter(image_files));
    std::sort(image_files.begin(), image_files.end());

    std::size_t total_kp{0};
    for (auto const& file : image_files) {
        auto image = cv::imread(file.generic_string(), cv::IMREAD_GRAYSCALE);
        if (image.empty()) {
            throw std::runtime_error("Failed to load image!");
        }

        std::vector<cv::KeyPoint> key_points;
        cv::Mat descriptors;
        cv::Mat mask;
        extractor(image, mask, key_points, descriptors);
        total_kp += key_points.size();
    }

    std::cout << "Total key-points = " << total_kp << std::endl;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        throw std::runtime_error(
            "Invalid arguments Usage: orb-extract"
            "<video|images> <videofile|images_folder> <settings-file>");
    }

    std::string mode = argv[1];
    std::string source = argv[2];
    std::string settings = argv[3];

    cv::FileStorage fSettings(settings, cv::FileStorage::READ);
    if (!fSettings.isOpened()) {
        throw std::runtime_error("Failed to open settings file");
    }

    int nFeatures = fSettings["ORBextractor.nFeatures"];
    float fScaleFactor = fSettings["ORBextractor.scaleFactor"];
    int nLevels = fSettings["ORBextractor.nLevels"];
    int fIniThFAST = fSettings["ORBextractor.iniThFAST"];
    int fMinThFAST = fSettings["ORBextractor.minThFAST"];

    std::cout << std::endl << "ORB Extractor Parameters: " << std::endl;
    std::cout << "- Number of Features: " << nFeatures << std::endl;
    std::cout << "- Scale Levels: " << nLevels << std::endl;
    std::cout << "- Scale Factor: " << fScaleFactor << std::endl;
    std::cout << "- Initial Fast Threshold: " << fIniThFAST << std::endl;
    std::cout << "- Minimum Fast Threshold: " << fMinThFAST << std::endl;

    ORB_SLAM2::ORBextractor extractor(nFeatures, fScaleFactor, nLevels,
                                      fIniThFAST, fMinThFAST);

    if ("video" == mode)
        extract_from_video(source, extractor);
    else if ("images" == mode)
        extract_from_images(source, extractor);
    else
        throw std::runtime_error("Invalid mode");

    return 0;
}
