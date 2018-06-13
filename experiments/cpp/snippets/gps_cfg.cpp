#include "gps_cfg.h"

namespace ORB_SLAM2 {
namespace ext {
gps_cfg& gps_cfg::get() {
    static gps_cfg cfg;
    return cfg;
}

gps_cfg::gps_cfg()
    : _ba_fuse_option(gps_option::dont_use_gps)
    , _gps_source(gps_source::none)
    , _add_vo_edge(false) {
}

void gps_cfg::gps_origin(const ext::pos_info_opt_t& origin) {
    std::lock_guard<std::mutex> lock(_cfg_lock);
    _gps_origin = origin;
}

ext::pos_info_opt_t gps_cfg::gps_origin() const {
    std::lock_guard<std::mutex> lock(_cfg_lock);
    return _gps_origin;
}

void gps_cfg::ba_fuse_option(gps_option option) {
    _ba_fuse_option = option;
}

gps_option gps_cfg::ba_fuse_option() const {
    return _ba_fuse_option;
}

void gps_cfg::add_vo_edge(bool val) {
    _add_vo_edge = val;
}

bool gps_cfg::add_vo_edge() const {
    return _add_vo_edge;
}

void gps_cfg::pose_optimization(bool val) {
    _pose_optimization = val;
}

bool gps_cfg::pose_optimization() const {
    return _pose_optimization;
}

void gps_cfg::source_gps(gps_source s) {
    _gps_source = s;
}

gps_source gps_cfg::source_gps() const {
    return _gps_source;
}
} // namespace ext
} // namespace ORB_SLAM2
