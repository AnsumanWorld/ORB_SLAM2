#pragma once

#include <atomic>
#include <vector>
#include <opencv2/core/types.hpp>

#include "Map.h"

namespace ORB_SLAM2
{
class statistics
{
public:
    // currently using singleton
    // todo: use dependency injection in users of this class
    static statistics& get();
    statistics();

    void update_frame_count(const std::size_t&);
    void update_keyframe_count(const std::size_t&);

    void update_frame_keypoint_stats(const std::vector<cv::KeyPoint>&);
    void update_keyframe_keypoint_stats(const std::vector<cv::KeyPoint>&);

	void update_mappoint_count(const std::size_t&);
    void update_semantic_mappoint_count(const std::size_t&);

    void update_map_stats(Map*);
    void print_stats();

private:
    double percent(const std::atomic<std::size_t>&,
		const std::atomic<std::size_t>&);

    std::atomic<std::size_t> _frames;
    std::atomic<std::size_t> _keyframes;

    std::atomic<std::size_t> _frame_keypoints;
    std::atomic<std::size_t> _keyframe_keypoints;

    std::atomic<std::size_t> _frame_semantic_kp;
    std::atomic<std::size_t> _keyframe_semantic_kp;

    std::atomic<std::size_t> _mp;
    std::atomic<std::size_t> _semantic_mp;

    std::atomic<std::size_t> _map_mp_count;
    std::atomic<std::size_t> _map_kf_count;

    std::atomic<std::size_t> _map_kp_count;
    std::atomic<std::size_t> _map_semantic_kp_count;
    std::atomic<std::size_t> _map_semantic_mps;
};

} //namespace ORB_SLAM2