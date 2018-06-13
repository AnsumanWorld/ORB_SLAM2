#pragma once

#include <g2o/config.h>

// clang-format off
#ifdef _MSC_VER
    #ifdef G2O_SHARED_LIBS
        #ifdef types_slamext_EXPORTS
            #define G2O_TYPES_SLAMEXT_API __declspec(dllexport)
        #else
            #define G2O_TYPES_SLAMEXT_API __declspec(dllimport)
        #endif
    #else
        #define G2O_TYPES_SLAMEXT_API
    #endif
#else
        #define G2O_TYPES_SLAMEXT_API
#endif
// clang-format on
