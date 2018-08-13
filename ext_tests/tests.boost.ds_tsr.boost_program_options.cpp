#include "stdafx.h"
#include <boost/program_options.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>
#include <string>

// Naming conventions:

// file name = test suite name 

namespace tests { // <- all tests live in this namespace
    namespace tests_boost_ext_tests { // <- namespace name = project name

                                      // <- fixture name = <test suite name>_fixture
        struct ds_tsr_boost_program_options_tests_fixture {
            ds_tsr_boost_program_options_tests_fixture() {}

            ~ds_tsr_boost_program_options_tests_fixture() {}

            template<class TRange>
            int distance(TRange const& r_) const
            {
                return static_cast<int>(boost::distance(r_));
            }
        };

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
            po::store(po::parse_command_line(argc_, argv_, desc, 
                po::command_line_style::default_style & ~po::command_line_style::short_allow_adjacent), vm);
            po::notify(vm);

            return test_args{
                s, i, d
            };
        }

        // test suite name = file name
        BOOST_FIXTURE_TEST_SUITE(ds_tsr_boost_program_options_tests, ds_tsr_boost_program_options_tests_fixture);

        BOOST_AUTO_TEST_CASE(options_in_long_format_should_allow_adjacent_arguments_with_equal_sign)
        {
            // long_allow_adjacent
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

        BOOST_AUTO_TEST_CASE(options_in_long_format_should_allow_next_arguments_with_whitespace)
        {
            // long_allow_next
            char const* argv[] = {
                "doit.exe",
                "--string boo",
                "--int 42",
                "--double 42.323"
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

        BOOST_AUTO_TEST_CASE(options_in_short_format_should_allow_next_arguments_with_whitespace)
        {
            char const* argv[] = {
                "doit.exe",
                "-s boo",
                "-i 42",
                "-d 42.323"
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

        BOOST_AUTO_TEST_CASE(options_in_short_format_should_allow_adjacent_arguments_without_separator)
        {
            char const* argv[] = {
                "doit.exe",
                "-sboo",
                "-i42",
                "-d42.323"
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

        BOOST_AUTO_TEST_SUITE_END()
    }
}

