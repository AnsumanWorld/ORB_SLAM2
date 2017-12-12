#include "ORBExtractor.h"

#include <boost/filesystem.hpp>

#include <iostream>
#include <algorithm>
#include <exception>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

namespace fs = boost::filesystem;

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        throw std::runtime_error(
            "Invalid arguments Usage: orb-extract <images_folder> <settings-file>");
    }

    cv::FileStorage fSettings(argv[2], cv::FileStorage::READ);
    if(!fSettings.isOpened())
    {
       throw std::runtime_error("Failed to open settings file"); 
    }

    int nFeatures = fSettings["ORBextractor.nFeatures"];
    float fScaleFactor = fSettings["ORBextractor.scaleFactor"];
    int nLevels = fSettings["ORBextractor.nLevels"];
    int fIniThFAST = fSettings["ORBextractor.iniThFAST"];
    int fMinThFAST = fSettings["ORBextractor.minThFAST"];

    std::cout << std::endl  << "ORB Extractor Parameters: " << std::endl;
    std::cout << "- Number of Features: " << nFeatures << std::endl;
    std::cout << "- Scale Levels: " << nLevels << std::endl;
    std::cout << "- Scale Factor: " << fScaleFactor << std::endl;
    std::cout << "- Initial Fast Threshold: " << fIniThFAST << std::endl;
    std::cout << "- Minimum Fast Threshold: " << fMinThFAST << std::endl;

    ORB_SLAM2::ORBextractor extractor(nFeatures,fScaleFactor, nLevels,
                        fIniThFAST, fMinThFAST);

    std::vector<fs::path> image_files;
    std::copy(fs::directory_iterator(argv[1]), 
            fs::directory_iterator(),
            std::back_inserter(image_files));
    std::sort(image_files.begin(), image_files.end());

    std::size_t total_kp {0};
    for (auto const& file : image_files)
    {
        auto image = cv::imread(file.generic_string(), cv::IMREAD_GRAYSCALE);
        if (image.empty())
        {
            throw std::runtime_error("Failed to load image!");
        }

        std::vector<cv::KeyPoint> key_points;
        cv::Mat descriptors;
        cv::Mat mask;
        extractor(image, mask, key_points, descriptors);
        total_kp += key_points.size();
    }

    std::cout << "Total key-points = " << total_kp << std::endl;
    
    return 0;
}