#include "types_slamext.h"

#include <g2o/core/factory.h>

namespace g2o
{
G2O_REGISTER_TYPE_GROUP(slamext);

G2O_REGISTER_TYPE(EDGE_SE3_EXPMAP_DISTANCE, EdgeSE3ExpmapGPSDist);
G2O_REGISTER_TYPE(EDGE_SE3_EXPMAP_PRIOR_2D, EdgeSE3ExpmapPrior3D);

G2O_REGISTER_ACTION(EdgeSE3ExpmapGPSDistWriteGnuplotAction);

#ifdef G2O_HAVE_OPENGL
G2O_REGISTER_ACTION(EdgeSE3ExpmapGPSDistDrawAction);
#endif
} // namespace g2o
