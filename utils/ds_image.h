#pragma once
#include <iostream>
#include <string>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <vector>
#include<opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

namespace fs = boost::filesystem;

namespace utils {
    class ds_image : public boost::iterator_facade<
                    ds_image,
                    const cv::Mat,
                    boost::single_pass_traversal_tag > {
        cv::VideoCapture _video_capture;
        std::vector<fs::path> _image_files;
        std::string _image_folder;
        size_t _image_index{ 0 };
        cv::Mat _image;
        bool _is_video{ true };

        void read_image_folder(const std::string &image_folder_)
        {
            if (false == image_folder_.empty())
            {
                if (fs::exists(image_folder_))
                {
                    std::copy(fs::directory_iterator(image_folder_), fs::directory_iterator(),
                        std::back_inserter(_image_files));
                    if (_image_files.size())
                        std::sort(_image_files.begin(), _image_files.end());                    
                }
                else
                    throw std::runtime_error(image_folder_ + " is not exist");
            }
        }
        void read_video(const std::string &video_file)
        {
            if (false == _video_capture.open(video_file))
                throw std::runtime_error(video_file + " is failed to open");
        }
        public:
            ds_image(const std::string &image_folder_,bool is_video_=true) 
                :_image_folder(image_folder_)
                , _is_video(is_video_)
            {
                if (_is_video)
                    read_video(_image_folder);                    
                else
                    read_image_folder(_image_folder);
                increment();
            }
            ds_image(const ds_image &ds_image_ins_) 
                :_image_folder(ds_image_ins_._image_folder)
                , _is_video(ds_image_ins_._is_video)
            {
                if (false == _image_folder.empty())
                {
                    if (_is_video)
                        read_video(_image_folder);                        
                    else
                        copy(ds_image_ins_._image_files.begin(), ds_image_ins_._image_files.end(), back_inserter(_image_files));
                    increment();
                }
            }
            bool is_video() const
            {
                return _is_video;
            }
            int get_timestamp()
            {
                return  (_is_video)? _video_capture.get(CV_CAP_PROP_POS_MSEC):0;
            }
            ds_image() {}
            bool is_empty() const
            {
                return _image.empty();
            }
            bool equal(const ds_image& other) const {
                return is_empty();
            }
            void increment() { 
                if (_is_video)
                    _video_capture >> _image;                    
                else
                    _image = cv::imread(_image_files[_image_index].generic_string(), CV_LOAD_IMAGE_UNCHANGED);
                _image_index++;
            }
            auto& dereference() const { 
                return _image; 
            }
            auto get_items(const std::string &image_folder_) const
            {
                return boost::make_iterator_range(
                    ds_image(image_folder_),
                    ds_image());
            }
    };
}