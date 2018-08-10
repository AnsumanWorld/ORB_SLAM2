#include "stdafx.h"
#include "ext_ds/ds_tsr_args.h"
#include "ext_ds/ds_tsr.h"

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
        };

        // test suite name = file name
        BOOST_FIXTURE_TEST_SUITE(ds_tsr_tests, ds_tsr_tests_fixture);

        using namespace ORB_SLAM2;

        BOOST_AUTO_TEST_CASE(ds_tsr_args_must_throw_with_invalid_cli_arguments)
        {
            {
                char const* argv[] = {"--boo"};
                // This is why I hat macros: The curly bracket notation for constructors does not work inside the macro... :-(
                BOOST_REQUIRE_THROW({
                    ext::ds_tsr_args args(1, argv);
                }, boost::program_options::error);
            }

            {
                BOOST_REQUIRE_THROW({
                    ext::ds_tsr_args args(0, nullptr);
                }, boost::program_options::error);
            }

            {
                char const* argv[] = {"--orb-vocabulary=promise",  "-c promise"};
                BOOST_REQUIRE_THROW(ext::ds_tsr_args args(1, argv), boost::program_options::error);
            }
        }

        BOOST_AUTO_TEST_CASE(ds_tsr_args_must_succeed_with_valid_cli_arguments)
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
                    ext::ds_tsr_args args(distance(argv), argv);
                });
            }
        }

        struct test_args {
            std::string s{};
            int i{};
            double d{};
        };

        auto parse_command_line(int argc_, char const* argv_[])
        {
            namespace po = boost::program_options;

            po::options_description desc{"Options"};
            std::string s{};
            int i{};
            double d{};

            desc.add_options()
                ("help,h", "Help screen")
                ("string,s", po::value(&s)->required(), "a string")
                ("int,i", po::value(&i)->required(), "an integer")
                ("double,d", po::value(&d)->required(), "a double");

            po::variables_map vm;
            po::store(po::parse_command_line(argc_, argv_, desc), vm);
            po::notify(vm);

            return test_args {
                s, i, d
            };
        }

        BOOST_AUTO_TEST_CASE(ds_tsr_args_explore_behavior_of_boost_options_in_long_format)
        {
            namespace po = boost::program_options;

            {
                char const* argv[] = {
                    "doit.exe",
                    "--string=boo",
                    "--int=42",
                    "--double=42.323"
                };

                test_args args{};
                try {
                    args = parse_command_line(distance(argv), argv);
                } catch (std::exception const& ex_) {
                    BOOST_REQUIRE_MESSAGE(false, "unexpected exception thrown: " << typeid(ex_).name() << " reason: " << ex_.what());
                }

                BOOST_CHECK_EQUAL("boo", args.s);
                BOOST_CHECK_EQUAL(42, args.i);
                BOOST_CHECK_EQUAL(42.323, args.d);
            }

            { 
                // despite the user manual, this format is obviously not supported:
                char const* argv[] = {
                    "doit.exe",
                    "--string boo",
                    "--int 42",
                    "--double 42.323"
                };

                test_args args{};
                BOOST_REQUIRE_THROW({
                    args = parse_command_line(distance(argv), argv);
                }, po::error);
            }
        }
        
        BOOST_AUTO_TEST_CASE(ds_tsr_args_explore_behavior_of_boost_options_in_short_format)
        {
            {
                char const* argv[] = {
                    "doit.exe",
                    "-s=boo",
                    "-i=42",
                    "-d=42.323"
                };

                test_args args{};
                try {
                    args = parse_command_line(distance(argv), argv);
                } catch (std::exception const& ex_) {
                    BOOST_REQUIRE_MESSAGE(false, "unexpected exception thrown: " << typeid(ex_).name() << " reason: " << ex_.what());
                }

                BOOST_CHECK_EQUAL("boo", args.s);
                BOOST_CHECK_EQUAL(42, args.i);
                BOOST_CHECK_EQUAL(42.323, args.d);
            }
        }

        BOOST_AUTO_TEST_CASE(ds_tsr_args_must_succeed_with_valid_short_cli_arguments)
        {
            try {
                char const* argv[] = {
                    "doit.exe", 
                    "-o=boo", 
                    "-c=baa", 
                    "-i=baz", 
                    "-t=far"
                };
                ext::ds_tsr_args args{distance(argv), argv};
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
            ext::ds_tsr_args args(distance(argv), argv);

            BOOST_CHECK_EQUAL("boo", args.orb_vocabulary());
            BOOST_CHECK_EQUAL("baa", args.camera_settings());
            BOOST_CHECK_EQUAL("baz", args.images());
            BOOST_CHECK_EQUAL("far", args.tsr_info());
        }

        BOOST_AUTO_TEST_CASE(ds_tsr_args_have_expected_values_with_short_notation)
        {
            char const* argv[] = {
                "doit.exe",
                "-oboo",
                "-cbaa",
                "-ibaz",
                "-tfar"
            };
            ext::ds_tsr_args args(distance(argv), argv);

            BOOST_CHECK_EQUAL("boo", args.orb_vocabulary());
            BOOST_CHECK_EQUAL("baa", args.camera_settings());
            BOOST_CHECK_EQUAL("baz", args.images());
            BOOST_CHECK_EQUAL("far", args.tsr_info());
        }

        BOOST_AUTO_TEST_SUITE_END();
    }
}

