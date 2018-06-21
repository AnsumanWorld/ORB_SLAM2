#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    // Todo: infrastructure_init(); if required

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}