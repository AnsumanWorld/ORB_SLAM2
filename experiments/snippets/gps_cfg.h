#pragma once

#include <string>
#include <mutex>

#include "ext/messages.h"

namespace ORB_SLAM2
{
namespace ext
{
enum class gps_option
{
    dont_use_gps = 0,
    unary_gps_edge = 1,
    binary_gps_edge = 2,
    unary_and_binary_gps_edge = 3
};

enum class gps_source
{
    none = 0,
    ground_truth = 1,
    oxts = 2
};

class gps_cfg
{
public:
    static gps_cfg& get();
    gps_cfg();
    void gps_origin(const ext::pos_info_opt_t&);
    ext::pos_info_opt_t gps_origin() const;
    void ba_fuse_option(gps_option);
    gps_option ba_fuse_option() const;
    void add_vo_edge(bool);
    bool add_vo_edge() const;
    void pose_optimization(bool);
    bool pose_optimization() const;
    void source_gps(gps_source);
    gps_source source_gps() const;

private:
    gps_option _ba_fuse_option;
    gps_source _gps_source;
    bool _add_vo_edge;
    bool _pose_optimization;
    ext::pos_info_opt_t _gps_origin;
    mutable std::mutex _cfg_lock;
};
} // namespace ext
} // namespace ORB_SLAM2
