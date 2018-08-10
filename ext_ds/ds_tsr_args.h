#pragma once

#include "ext/messages.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace ORB_SLAM2 {
    namespace ext {
        class ds_tsr_args {
            fs::path
                _orb_vocabulary{},
                _camera_settings{},
                _images{},
                _tsr_info{};
        public:
            ds_tsr_args() = default;

            ds_tsr_args(int argc, char const* const argv[])
            {
                po::options_description desc{"Options"};
                po::variables_map vm;
                try {
                    desc.add_options()
                        ("help,h", "Help screen")
                        ("orb-vocabulary,o", po::value(&_orb_vocabulary)->required(), "Path to orb-vocabulary")
                        ("camera-settings,c", po::value(&_camera_settings)->required(), "Path to camera settings (yaml)")
                        ("images,i", po::value(&_images)->required(), "Path to folder with input images")
                        ("tsr-info,t", po::value(&_tsr_info)->required(), "Path to traffic-signs recognition results");

                    po::store(po::parse_command_line(argc, argv, desc), vm);
                    po::notify(vm);
                } catch (const po::error &ex_) {
                    if (vm.count("help"))
                        std::cout << desc << std::endl;
                    else
                        std::cout << ex_.what() << std::endl << "For help run: run_dashcam --help" << std::endl;
                    throw;
                }
            }

            auto const& orb_vocabulary() const
            {
                return _orb_vocabulary;
            }

            auto const& camera_settings() const
            {
                return _camera_settings;
            }

            auto const& images() const
            {
                return _images;
            }

            auto const& tsr_info() const
            {
                return _tsr_info;
            }
        };
    }
}