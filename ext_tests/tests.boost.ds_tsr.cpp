#include "stdafx.h"
#include "ext_ds/ds_tsr_args.h"
#include "ext_ds/ds_tsr.h"
#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

// Naming conventions:

// file name = test suite name 

namespace tests { // <- all tests live in this namespace
    namespace tests_boost_ext_tests { // <- namespace name = project name

                                      // <- fixture name = <test suite name>_fixture
        struct ds_tsr_tests_fixture {
            ds_tsr_tests_fixture() {}

            ~ds_tsr_tests_fixture() {}

            template<class TRange>
            int distance(TRange const& r_) const
            {
                return static_cast<int>(boost::distance(r_));
            }

            auto ds_tsr_test_data_dir() const
            {
                return "data/tests.ds_tsr.garching-first-25-frames";
            }

            auto ds_tsr_vocabulary_path() const
            {
                return "Vocabulary/ORBvoc.bin";
            }
        };

        // test suite name = file name
        BOOST_FIXTURE_TEST_SUITE(ds_tsr_tests, ds_tsr_tests_fixture);

        using namespace ORB_SLAM2;

        BOOST_AUTO_TEST_CASE(ds_tsr_args_parsing_must_throw_with_invalid_cli_arguments)
        {
            {
                char const* argv[] = {"--boo"};
                BOOST_REQUIRE_THROW({
                    auto args = ext::ds_tsr_args::parse_command_line(1, argv);
                }, boost::program_options::error);
            }

            {
                BOOST_REQUIRE_THROW({
                    auto args = ext::ds_tsr_args::parse_command_line(0, nullptr);
                }, boost::program_options::error);
            }

            {
                char const* argv[] = {"--orb-vocabulary=promise",  "-c promise"};
                BOOST_REQUIRE_THROW({
                    auto args = ext::ds_tsr_args::parse_command_line(1, argv);
                }, boost::program_options::error);
            }
        }

        BOOST_AUTO_TEST_CASE(ds_tsr_args_parsing_must_succeed_with_valid_cli_arguments)
        {
            {
                char const* argv[] = {
                    "doit.exe", 
                    "--orb-vocabulary=boo", 
                    "--camera-settings=baa", 
                    "--images=baz", 
                    "--tsr-info=far"
                };
                BOOST_REQUIRE_NO_THROW({
                    auto args = ext::ds_tsr_args::parse_command_line(distance(argv), argv);
                });
            }
        }

        BOOST_AUTO_TEST_CASE(ds_tsr_args_parsing_must_succeed_with_valid_short_cli_arguments)
        {
            try {
                char const* argv[] = {
                    "doit.exe", 
                    "-o=boo", 
                    "-c=baa", 
                    "-i=baz", 
                    "-t=far"
                };
                auto args = ext::ds_tsr_args::parse_command_line(distance(argv), argv);
            } catch (std::exception const& ex_) {
                BOOST_REQUIRE_MESSAGE(false, "unexpected exception thrown: " << typeid(ex_).name() << " reason: " << ex_.what());
            }
        }

        BOOST_AUTO_TEST_CASE(ds_tsr_args_have_expected_values)
        {
            char const* argv[] = {
                "doit.exe",
                "--orb-vocabulary=boo",
                "--camera-settings=baa",
                "--images=baz",
                "--tsr-info=far"
            };
            auto args = ext::ds_tsr_args::parse_command_line(distance(argv), argv);

            BOOST_CHECK_EQUAL("boo", args.orb_vocabulary());
            BOOST_CHECK_EQUAL("baa", args.camera_settings());
            BOOST_CHECK_EQUAL("baz", args.images());
            BOOST_CHECK_EQUAL("far", args.tsr_info());
        }

        BOOST_AUTO_TEST_CASE(ds_tsr_args_test_values_exist_in_test_folder)
        {
            BOOST_TEST_MESSAGE("Please assure that working directory is the same as the location of the test binary executable. When running tests in Visual Studio Test Runner use the provided tests.runsettings file.");
            BOOST_CHECK(fs::exists(this->ds_tsr_test_data_dir()));
            BOOST_CHECK(fs::exists(this->ds_tsr_test_data_dir() / fs::path{"images"}));
            BOOST_CHECK(fs::exists(this->ds_tsr_test_data_dir() / fs::path{"images"} / fs::path{"000000.jpg"}));
            BOOST_CHECK(fs::exists(this->ds_tsr_test_data_dir() / fs::path{"config.yaml"}));
            BOOST_CHECK(fs::exists(this->ds_tsr_test_data_dir() / fs::path{"results.txt"}));
            BOOST_CHECK(fs::exists(this->ds_tsr_test_data_dir() / fs::path{"video.mov"}));
            BOOST_CHECK(fs::exists(this->ds_tsr_vocabulary_path()));
        }


        BOOST_AUTO_TEST_SUITE_END();
    }
}

