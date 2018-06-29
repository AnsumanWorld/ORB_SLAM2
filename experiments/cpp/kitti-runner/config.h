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

    bool use_ext_pose() const {
        return _use_ext_pose;
    }
    bool wait_for_stdin() const {
        return _wait_for_stdin;
    }
    fs::path const& ext_pose_data() const {
        return _ext_pose_data;
    }

    fs::path const& images_dir() const {
        return _images_dir;
    }

    fs::path const& output_traj_path() const {
        return _output_traj_path;
    }

    fs::path const& settings_path() const {
        return _settings_path;
    }

    fs::path const& vocabulary_path() const {
        return _vocabulary_path;
    }

    std::size_t const& ext_pose_skip_freq() const {
        return _ext_pose_skip_freq;
    }

    std::size_t const& frame_interval_ms() const {
        return _frame_interval_ms;
    }

    std::size_t const& min_init_frames_with_ext_pose() const {
        return _min_init_frames_with_ext_pose;
    }

    void print(std::ostream& os) const {
        const std::string sep = " : ";
        os << "_use_ext_pose" << sep << _use_ext_pose << '\n';
        os << "_wait_for_stdin" << sep << _wait_for_stdin << '\n';
        os << "_ext_pose_data" << sep << _ext_pose_data << '\n';
        os << "_images_dir" << sep << _images_dir << '\n';
        os << "_output_traj_path" << sep << _output_traj_path << '\n';
        os << "_settings_path" << sep << _settings_path << '\n';
        os << "_vocabulary_path" << sep << _vocabulary_path << '\n';
        os << "_ext_pose_skip_freq" << sep << _ext_pose_skip_freq << '\n';
        os << "_frame_interval_ms" << sep << _frame_interval_ms << '\n';
        os << "_min_init_frames_with_ext_pose" << sep
           << _min_init_frames_with_ext_pose << '\n';
    }

private:
    void parse(int argc, char** argv) {
        po::options_description cmd_opts("options");
        // clang-format off
        cmd_opts.add_options()
            ("help", "produce help message")
            ("use_ext_pose", po::value<bool>(&_use_ext_pose)->required(), "enable/disable using external pose from 'ext_pose_data'")
            ("wait_for_stdin", po::value<bool>(&_wait_for_stdin)->required(), "if true program halts waiting for keyboard input, used to support `Attach to debugger`")
            ("ext_pose_data", po::value<fs::path>(&_ext_pose_data), "external pose in KITTI format example: ground truth")
            ("images_dir", po::value<fs::path>(&_images_dir)->required(), "directory containing images(png)")
            ("output_traj_path", po::value<fs::path>(&_output_traj_path)->required(), "output key-frame trajecotry file path")
            ("settings_path", po::value<fs::path>(&_settings_path)->required(), "yml setting file required for ORB-SLAM2")
            ("vocabulary_path", po::value<fs::path>(&_vocabulary_path)->required(), "input vocabulary file(bag of words)")
            ("ext_pose_skip_freq", po::value<std::size_t>(&_ext_pose_skip_freq), "skip external pose, eg. Attach frame 100 external pose and skip 10 (if ext_pose_skip_freq = 10), next attach frame 101 with external pose")
            ("frame_interval_ms", po::value<std::size_t>(&_frame_interval_ms), "control frames per second")
            ("min_init_frames_with_ext_pose", po::value<std::size_t>(&_min_init_frames_with_ext_pose), "minimum number of frames attached with external pose after startup (before skipping begins)")
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

    bool _use_ext_pose = false;
    bool _wait_for_stdin = false;
    fs::path _ext_pose_data;
    fs::path _images_dir;
    fs::path _output_traj_path;
    fs::path _settings_path;
    fs::path _vocabulary_path;
    std::size_t _ext_pose_skip_freq = -1;
    std::size_t _frame_interval_ms = -1;
    std::size_t _min_init_frames_with_ext_pose = -1;
};
