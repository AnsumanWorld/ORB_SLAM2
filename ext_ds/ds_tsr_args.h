#pragma once

#include "ext/messages.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <exception>
#include <string>
#include <utility>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace ORB_SLAM2 {
    namespace ext {
        class ds_tsr_args_error : public po::error {
            std::string _help;
        public:
            ds_tsr_args_error(std::string help_, std::string inner_ex_test_)
                : po::error{std::move(inner_ex_test_)}
                , _help{std::move(help_)}
            {
            }

            std::string help() const
            {
                return _help;
            }
        };

        class ds_tsr_args {
            fs::path
                _orb_vocabulary,
                _camera_settings,
                _images,
                _tsr_info;
        public:
            // this should be removed
            ds_tsr_args() = default;

            ds_tsr_args(
                fs::path orb_vocabulary_,
                fs::path camera_settings_,
                fs::path images_,
                fs::path tsr_info_)
                : _orb_vocabulary{orb_vocabulary_}
                , _camera_settings{camera_settings_}
                , _images{images_}
                , _tsr_info{tsr_info_}
            {
            }

            auto orb_vocabulary() const
            {
                return _orb_vocabulary;
            }

            auto camera_settings() const
            {
                return _camera_settings;
            }

            auto images() const
            {
                return _images;
            }

            auto tsr_info() const
            {
                return _tsr_info;
            }

            static auto ds_tsr_args::parse_command_line(int argc, char const* const argv[])
            {
                po::options_description desc{"Options"};
                po::variables_map vm;
                std::string help;

                fs::path 
                    orb_vocabulary,
                    camera_settings,
                    images,
                    tsr_info;

                desc.add_options()
                    ("help,h", "Help screen")
                    ("orb-vocabulary,o", po::value(&orb_vocabulary)->required(), "Path to orb-vocabulary")
                    ("camera-settings,c", po::value(&camera_settings)->required(), "Path to camera settings (yaml)")
                    ("images,i", po::value(&images)->required(), "Path to folder with input images")
                    ("tsr-info,t", po::value(&tsr_info)->required(), "Path to traffic-signs recognition results");

                try {
                    if (!vm.count("help")) {
                        po::store(po::parse_command_line(argc, argv, desc), vm);
                        po::notify(vm);
                    } else {
                        throw po::error{"help requested"};
                    }
                } catch (po::error& ex_) {
                    // create help string
                    std::ostringstream str;
                    str << desc;
                    throw ds_tsr_args_error{str.str(), ex_.what()};
                }

                return ds_tsr_args{
                    orb_vocabulary,
                    camera_settings,
                    images,
                    tsr_info
                };
            }
        };


    }
}