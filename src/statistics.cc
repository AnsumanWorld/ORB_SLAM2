#include "statistics.h"
#include <iostream>
#include <string>

namespace ORB_SLAM2
{

statistics& statistics::get()
{
    static statistics instance;
    return instance;
}

statistics::statistics()
: _frames(0)
, _keyframes(0)
, _frame_keypoints(0)
, _keyframe_keypoints(0)
, _frame_semantic_kp(0)
, _keyframe_semantic_kp(0)
, _mp(0)
, _semantic_mp(0)
, _map_mp_count(0)
, _map_kf_count(0)
, _map_kp_count(0)
, _map_semantic_kp_count(0)
, _map_semantic_mps(0)
{

}

void statistics::update_frame_count(const std::size_t& count)
{
    _frames += count;
}

void statistics::update_keyframe_count(const std::size_t& count)
{
    _keyframes += count;
}

void statistics::update_frame_keypoint_stats(
    const std::vector<cv::KeyPoint>& kps)
{
	auto semantic_kp = std::count_if(kps.begin(),
		kps.end(),
		[](const cv::KeyPoint& kp) { return kp.class_id != -1; });
    
    _frame_keypoints += kps.size();
    _frame_semantic_kp += semantic_kp;
}

void statistics::update_keyframe_keypoint_stats(
    const std::vector<cv::KeyPoint>& kps)
{
	auto semantic_kp = std::count_if(kps.begin(),
		kps.end(),
		[](const cv::KeyPoint& kp) { return kp.class_id != -1; });

    _keyframe_keypoints += kps.size();
    _keyframe_semantic_kp += semantic_kp;
}

void statistics::update_mappoint_count(const std::size_t& count)
{
    _mp += count;
}

void statistics::update_semantic_mappoint_count(const std::size_t& count)
{
    _semantic_mp += count;
}

void statistics::update_map_stats(Map* map)
{
    _map_mp_count += map->MapPointsInMap();
    _map_kf_count += map->KeyFramesInMap();
    
    auto kfs = map->GetAllKeyFrames();
    auto mps = map->GetAllMapPoints();

    _map_semantic_mps += std::count_if(mps.begin(), mps.end(), 
        [](MapPoint* mp) { return mp->is_semantic(); });

    std::for_each(kfs.begin(), kfs.end(), 
    [this](KeyFrame* kf)
    {
        this->_map_kp_count += kf->mvKeys.size();
		this->_map_semantic_kp_count += std::count_if(kf->mvKeys.begin(),
            kf->mvKeys.end(),
		    [](const cv::KeyPoint& kp) { return kp.class_id != -1; });
    });
}

void statistics::print_stats()
{
    const std::string seperator = ", ";
    const std::string newline = "\n";
    std::string frame_stats;
    frame_stats += "Frames = " + std::to_string(_frames) 
    + seperator + "Key-frames = " + std::to_string(_keyframes)
    + seperator + "Percent = " + std::to_string(percent(_keyframes, _frames))
    + newline;

    std::string kp_stats;
    kp_stats += "Frames Key-points = " + std::to_string(_frame_keypoints)
    + seperator + "Key-frames Key-points = " + std::to_string(_keyframe_keypoints)
    + seperator + "Percent = " 
    + std::to_string(percent(_keyframe_keypoints, _frame_keypoints))
    + newline;

    std::string semantic_kp_stats;
    semantic_kp_stats += "Frames Semantic Key-points = " + std::to_string(_frame_semantic_kp)
    + seperator + "Key-frames Semantic Key-points = " 
    + std::to_string(_keyframe_semantic_kp)
    + seperator + "Percent = "
    + std::to_string(percent(_keyframe_semantic_kp, _frame_semantic_kp))
    + newline;

    std::string mp_stats;
    mp_stats += "Map-points = " + std::to_string(_mp)
    + seperator + "Semantic Map-points = " + std::to_string(_semantic_mp)
    + seperator + "Percent = " + std::to_string(percent(_semantic_mp, _mp))
    + newline;

    std::string kf_kp_mp_stats;
	kf_kp_mp_stats += "Key-Frames Key-points = " + std::to_string(_keyframe_keypoints)
    + seperator + "Map-points = " + std::to_string(_mp)
    + seperator + "Percent = " + std::to_string(percent(_mp, _keyframe_keypoints))
    + newline;

    std::string kf_semantic_kp_mp_stats;
	kf_semantic_kp_mp_stats += "Key-Frames Semantic Key-points = "
    + std::to_string(_keyframe_semantic_kp)
    + seperator + "Semantic Map-points = " + std::to_string(_semantic_mp)
    + seperator + "Percent = " 
    + std::to_string(percent(_semantic_mp, _keyframe_semantic_kp))
    + newline;

    std::string map_kf_stats;
    map_kf_stats += "Key-Frames = " + std::to_string(_keyframes)
    + seperator + "Map Key-frames = " + std::to_string(_map_kf_count)
    + seperator + "Percent = " 
    + std::to_string(percent(_map_kf_count, _keyframes))
    + newline;

    std::string map_kp_stats;
    map_kp_stats += "Key-frames Key-points = " 
    + std::to_string(_keyframe_keypoints)
    + seperator + "Map Key-frames Key-points = " + std::to_string(_map_kp_count)
    + seperator + "Percent = " 
    + std::to_string(percent(_map_kp_count, _keyframe_keypoints))
    + newline;

    std::string map_semantic_kp_stats;
    map_semantic_kp_stats += "Key-frames Semantic Key-points = " 
    + std::to_string(_keyframe_semantic_kp)
    + seperator + "Map Semantic Key-points = " + std::to_string(_map_semantic_kp_count)
    + seperator + "Percent = " 
    + std::to_string(percent(_map_semantic_kp_count, _keyframe_semantic_kp))
    + newline;

    std::string map_mp_stats;
    map_mp_stats += "Map-points = " + std::to_string(_mp)
    + seperator + "Map Map-points = " + std::to_string(_map_mp_count)
    + seperator + "Percent = "
    + std::to_string(percent(_map_mp_count, _mp))
    + newline;

    std::string map_semantic_kp_mp_stats;
    map_semantic_kp_mp_stats += "Map Semantic Key-points = "
    + std::to_string(_map_semantic_kp_count)
    + seperator + "Map Semantic Map-points = "
    + std::to_string(_map_semantic_mps) + seperator
    + std::to_string(percent(_map_semantic_mps, _map_semantic_kp_count))
    + newline;

    std::cout << "\n*** Statistics ***\n";
    std::cout << frame_stats;
    std::cout << kp_stats;
    std::cout << semantic_kp_stats;
    std::cout << mp_stats;
    std::cout << kf_kp_mp_stats;
    std::cout << kf_semantic_kp_mp_stats;
    std::cout << "\nPoint Cloud Statistics\n";
    std::cout << map_kf_stats;
    std::cout << map_kp_stats;
    std::cout << map_mp_stats;
    std::cout << map_semantic_kp_mp_stats;
    std::cout << "--- Statistics ---\n";
}

double statistics::percent(
    const std::atomic<std::size_t>& nr, const std::atomic<std::size_t>& dr)
{
    if(dr == 0)
        return 0.0;
    return (100.0 * nr) / dr;
}

} // namespace ORB_SLAM2