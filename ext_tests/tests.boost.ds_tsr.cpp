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
        };

        // test suite name = file name
        BOOST_FIXTURE_TEST_SUITE(ds_tsr_tests, ds_tsr_tests_fixture);

        using namespace ORB_SLAM2;

        BOOST_AUTO_TEST_CASE(ds_tsr_args_must_throw_with_invalid_cli_arguments)
        {
            {
                char const* argv[] = {"--boo"};
                BOOST_REQUIRE_THROW(ext::ds_tsr_args args(1, argv), boost::program_options::error);
            }

            {
                BOOST_REQUIRE_THROW(ext::ds_tsr_args args(0, nullptr), boost::program_options::error);
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
                    "--image-folder=baz", 
                    "--tsr-info=far"
                };
                BOOST_REQUIRE_NO_THROW(ext::ds_tsr_args args(boost::distance(argv), argv));
            }
        }

        BOOST_AUTO_TEST_CASE(ds_tsr_args_must_succeed_with_valid_short_cli_arguments)
        {
            {
                char const* argv[] = {
                    "doit.exe", 
                    "-o boo", 
                    "-c baa", 
                    "-i baz", 
                    "-t far"
                };
                BOOST_REQUIRE_NO_THROW(ext::ds_tsr_args args(boost::distance(argv), argv));
            }
        }

        BOOST_AUTO_TEST_SUITE_END();
    }
}

