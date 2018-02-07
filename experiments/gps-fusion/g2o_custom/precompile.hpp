#pragma once

#include <g2o/core/base_binary_edge.h>
#include <g2o/core/base_unary_edge.h>
#include <g2o/core/factory.h>
#include <g2o/core/hyper_graph_action.h>
#include <g2o/stuff/macros.h>
#include <g2o/types/sba/types_sba.h>
#include <g2o/types/sba/types_six_dof_expmap.h>
#include <g2o/types/sim3/types_seven_dof_expmap.h>
#include <g2o/types/slam2d/edge_pointxy.h>
#include <g2o/types/slam2d/edge_xy_prior.h>
#include <g2o/types/slam2d/vertex_point_xy.h>
#include <g2o/types/slam3d/edge_se3.h>
#include <g2o/types/slam3d/edge_se3_prior.h>
#include <g2o/types/slam3d/isometry3d_gradients.h>

#ifdef G2O_HAVE_OPENGL
#include "g2o/stuff/opengl_primitives.h"
#include "g2o/stuff/opengl_wrapper.h"
#endif

#include <Eigen/Core>
#include <Eigen/StdVector>

#include <iostream>

#include "g2o_types_custom_api.hpp"
