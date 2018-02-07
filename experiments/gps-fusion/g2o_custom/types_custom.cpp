#include "precompile.hpp"

#include "types_custom.hpp"

namespace g2o
{
G2O_REGISTER_TYPE_GROUP(custom);

G2O_REGISTER_TYPE(EDGE_POINT_XY_DISTANCE, EdgePointXYDistance);
G2O_REGISTER_TYPE(EDGE_SE3_EXPMAP_DISTANCE, EdgeSE3ExpmapDistance);
G2O_REGISTER_TYPE(EDGE_SE3_EXPMAP_PRIOR_2D, EdgeSE3ExpmapPrior2D);

G2O_REGISTER_ACTION(EdgePointXYDistanceWriteGnuplotAction);
G2O_REGISTER_ACTION(EdgeSE3ExpmapDistanceWriteGnuplotAction);

#ifdef G2O_HAVE_OPENGL
G2O_REGISTER_ACTION(EdgePointXYDistanceDrawAction);
G2O_REGISTER_ACTION(EdgeSE3ExpmapDistanceDrawAction);
#endif
} // namespace g2o
