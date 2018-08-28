#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <ostream>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

class config {
public:
    config(int argc, char** argv) {
        parse(argc, argv);
    }

    bool wait_for_stdin() const {
        return _wait_for_stdin;
    }

    fs::path const& output_traj_path() const {
        return _output_traj_path;
    }

    fs::path const& images_dir() const {
        return _images_dir;
    }

    fs::path const& depth_dir() const {
        return _depth_dir;
    }

    fs::path const& settings_path() const {
        return _settings_path;
    }

    fs::path const& vocabulary_path() const {
        return _vocabulary_path;
    }

    std::size_t const& frame_interval_ms() const {
        return _frame_interval_ms;
    }
    
private:
    void parse(int argc, char** argv) {
        po::options_description cmd_opts("options");
        // clang-format off
        cmd_opts.add_options()
            ("help", "produce help message")
            ("wait_for_stdin", po::value<bool>(&_wait_for_stdin)->required(), "if true program halts waiting for keyboard input, used to support `Attach to debugger`")
            ("images_dir", po::value<fs::path>(&_images_dir)->required(), "directory containing images(png)")
            ("depth_dir", po::value<fs::path>(&_depth_dir)->required(), "directory containing depth images(png)")
            ("settings_path", po::value<fs::path>(&_settings_path)->required(), "yml setting file required for ORB-SLAM2")
            ("vocabulary_path", po::value<fs::path>(&_vocabulary_path)->required(), "input vocabulary file(bag of words)")
            ("frame_interval_ms", po::value<std::size_t>(&_frame_interval_ms), "control frames per second")
            ("output_traj_path", po::value<fs::path>(&_output_traj_path)->required(), "output key-frame trajecotry file path")
        ;
        // clang-format on
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, cmd_opts), vm);

        if (vm.count("help")) {
            std::cout << cmd_opts << std::endl;
            return;
        }

        po::notify(vm);
    }

    bool _wait_for_stdin = false;
    fs::path _images_dir;
    fs::path _depth_dir;
    fs::path _settings_path;
    fs::path _vocabulary_path;
    std::size_t _frame_interval_ms = -1;
    fs::path _output_traj_path;
};
