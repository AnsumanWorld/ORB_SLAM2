#include "log.h"

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

    fs::path const& image_source() const {
        return _image_source;
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

    bool show_viewer() const {
        return _show_viewer;
    }

    void print(std::ostream& os) const {
        const std::string sep = " : ";
        os << "_wait_for_stdin" << sep << _wait_for_stdin << '\n';
        os << "_image_source" << sep << _image_source << '\n';
        os << "_settings_path" << sep << _settings_path << '\n';
        os << "_vocabulary_path" << sep << _vocabulary_path << '\n';
        os << "_frame_interval_ms" << sep << _frame_interval_ms << '\n';
        os << "_show_viewer" << sep << _show_viewer << '\n';
    }

private:
    void parse(int argc, char** argv) {
        po::options_description cmd_opts("options");
        // clang-format off
        cmd_opts.add_options()
            ("help", "produce help message")
            ("wait_for_stdin", po::value<bool>(&_wait_for_stdin)->required(), "if true program halts waiting for keyboard input, used to support `Attach to debugger`")
            ("image_source", po::value<fs::path>(&_image_source)->required(), "input source are images directory|video path|camera id")
            ("settings_path", po::value<fs::path>(&_settings_path)->required(), "yml setting file required for ORB-SLAM2")
            ("vocabulary_path", po::value<fs::path>(&_vocabulary_path)->required(), "input vocabulary file(bag of words)")
            ("frame_interval_ms", po::value<std::size_t>(&_frame_interval_ms), "control frames per second")
            ("show_viewer", po::value<bool>(&_show_viewer), "if true show pangolin viewer")
        ;
        // clang-format on
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, cmd_opts), vm);

        if (vm.count("help")) {
            LOG_MSG << cmd_opts << std::endl;
            return;
        }

        po::notify(vm);
    }

    bool _wait_for_stdin = false;
    fs::path _image_source;
    fs::path _settings_path;
    fs::path _vocabulary_path;
    std::size_t _frame_interval_ms = -1;
    bool _show_viewer = false;
};
