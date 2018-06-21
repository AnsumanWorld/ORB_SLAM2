#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#include "constants.h"

double deg2rad(const double deg);
double rad2deg(const double rad);
double wrap2pi(double in);

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

double new_sample(const double old_mu1, const double old_var1,
                  const double old_mu2, const double old_var2);
