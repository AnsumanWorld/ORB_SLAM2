#pragma once
#include <iostream>
#include "ds_dashcam.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <boost/algorithm/string.hpp>

using namespace std;

namespace ORB_SLAM2 {
	namespace ext {
		enum class gps_offset { long_val = 12, long_direction, lat_val, lat_direction };

		class gps_reader
		{
			std::string				_video_file;

		public:
			gps_reader(std::string video_file_):_video_file(video_file_)
			{
			}

			//convert gps value(degree minutes and second(dms) to degree decimal(dd))
			double convert_gps_to_dec(std::string gps_str)
			{
				double gps_val = stod(gps_str);
				int degree_val = (int)gps_val / 100;
				double minutes_val = gps_val - (degree_val * 100);
				double deg_dec_val = degree_val + (minutes_val / 60);
				return deg_dec_val;
			}

			bool get_gps_from_Str(std::string src, double &long_val, double &lat_val)
			{
				bool gps_status = false;
				int long_direction = 1;
				int lat_direction = 1;
				//check for character A or V from gps string
				//A for active(is good)
				//V for void(there's an unexpected problem with the position reading.)
				std::vector<std::string> result;
				boost::split(result, src, boost::is_any_of(","));

				if ((20 <= result.size()) && (!result[9].compare("A")))
				{
					std::string long_str = result[(int)gps_offset::long_val];
					std::string lat_str = result[(int)gps_offset::lat_val];

					if (!result[(int)gps_offset::long_direction].compare("S"))
						long_direction = -1;

					if (!result[(int)gps_offset::lat_direction].compare("W"))
						lat_direction = -1;

					if ((false == long_str.empty()) && (false == lat_str.empty()))
					{
						gps_status = true;
						long_val = convert_gps_to_dec(long_str) * long_direction;
						lat_val = convert_gps_to_dec(lat_str) * lat_direction;
					}

				}
				return gps_status;
			}

			bool read_gps(std::vector<gps_info_t> &_vec_gps_info)
			{
				AVCodecContext    *pCodecCtxOrig = NULL;
				AVCodecContext    *pCodecCtx = NULL;
				AVCodec           *pCodec = NULL;
				AVFrame           *pFrame = NULL;
				int               frameFinished;
				AVPacket          packet;
				AVSubtitle		  *sub = NULL;
				AVFormatContext   *pFormatCtx = NULL;
				int               subtitle_index = -1;
				bool              gps_status = 0;

				av_register_all();

				if (avformat_open_input(&pFormatCtx, _video_file.c_str(), NULL, NULL) != 0)
				{
					std::cerr << "failed to open!!! " << _video_file << std::endl;
					return false;
				}
				if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
				{
					std::cerr << "failed to retieve " << _video_file << std::endl;
					return false; // Couldn't find stream information
				}

				for (int index = 0; index < pFormatCtx->nb_streams; index++)
				{
					if (pFormatCtx->streams[index]->codec->codec_type == AVMEDIA_TYPE_SUBTITLE)
					{
						subtitle_index = index;
						break;
					}
				}

				if (subtitle_index == -1)
				{
					std::cout << "failed to find gps info from " << _video_file << std::endl;
					return false;
				}
				// Get a pointer to the codec context for the video stream
				pCodecCtxOrig = pFormatCtx->streams[subtitle_index]->codec;

				// Find the decoder for the video stream
				pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
				if (pCodec == NULL)
				{
					std::cerr << "Unsupported codec!" << std::endl;
					return false; // Codec not found
				}
				// Copy context
				pCodecCtx = avcodec_alloc_context3(pCodec);
				if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0)
				{
					std::cerr << "Couldn't copy codec context" << std::endl;
					return false; // Error copying codec context
				}

				// Open codec
				if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
				{
					std::cerr << "Could not open codec" << std::endl;
					return false; // Could not open codec
				}

				sub = new AVSubtitle();

				while (av_read_frame(pFormatCtx, &packet) >= 0)
				{
					// Is this a packet from the subtitle stream?
					if (packet.stream_index == subtitle_index)
					{
						// Decode subtitle frame
						avcodec_decode_subtitle2(pCodecCtx, sub, &frameFinished, &packet);
						if (frameFinished)
						{
							gps_info_t gps_info_obj;

							for (int j = 0; j < sub->num_rects; j++)
							{
								if ((sub->rects[j]->type == SUBTITLE_ASS) && (sub->rects[j]->ass))
								{
									bool gps_status = get_gps_from_Str(sub->rects[j]->ass, gps_info_obj._longitude, gps_info_obj._latitude);
									if (gps_status)
									{
										int frame_duration = sub->end_display_time - sub->start_display_time;
										gps_info_obj._timestamp = sub->pts / frame_duration;
										gps_info_obj._frame_number = pCodecCtx->frame_number;
										gps_info_obj._gps_str = sub->rects[j]->ass;
										_vec_gps_info.push_back(gps_info_obj);
									}
									break;
								}
							}

							avsubtitle_free(sub);
						}
					}
					av_free_packet(&packet);
				}
				// Free the YUV frame
				delete sub;
				// Close the codecs
				avcodec_close(pCodecCtx);
				avcodec_close(pCodecCtxOrig);

				// Close the video file
				avformat_close_input(&pFormatCtx);

				if (_vec_gps_info.size())
					gps_status = true;

				return gps_status;
			}
			~gps_reader()
			{
			}
		};

		ds_dashcam_args::ds_dashcam_args(int argc, char** argv)
		{
			if (argc < 5) {
				std::cout << "Usage: ./run_dashcam_monocular <path_to_vocabulary> <path_to_camera_settings> <path_to_video> <path_to_csv_file>" << std::endl;
			}
			_path_to_video = argv[3];
			_path_to_csv_file = (argc >= 5) ? argv[4] : "";
		}

		ds_dashcam_args::ds_dashcam_args()
		{
		}

		//read video file and read gps from video
		ds_dashcam::ds_dashcam(ds_dashcam_args& dashcam_args_) : _dashcam_args(dashcam_args_)
		{
			_data_source_status = read_video(_dashcam_args._path_to_video);
			if (_data_source_status)
			{
				gps_reader gps_reader_obj(_dashcam_args._path_to_video);
				_gps_input_status = gps_reader_obj.read_gps(_vec_gps_info);
				get_next_item();
			}
		}

		//read video file
		bool ds_dashcam::read_video(std::string video_file)
		{
			bool video_status = false;
			video_status = _video_capture.open(video_file);
			if (false == video_status)
				std::cout << "failed to open " << video_file << std::endl;
			else
				load_next_frame();
			return video_status;
		}

		//collect next image and gps data in _item
		void  ds_dashcam::get_next_item()
		{
			if (false == _image.empty())
			{
				pos_info_opt_t gps_ds;
				gps_ds = load_next_gps();
				image_t image = _image;
				double diff_timestamp = _timestamp;
				load_next_frame();
				diff_timestamp = diff_timestamp - _timestamp;
				_item = std::make_tuple(diff_timestamp, image, boost::none, gps_ds);
			}				
		}

		//collect next from from video
		void ds_dashcam::load_next_frame()
		{
			_video_capture >> _image;
			_timestamp = _video_capture.get(CV_CAP_PROP_POS_MSEC);			

			if (false == _image.empty())
				_image_index++;
			else
				_image_index = 0;
		}

		//collect next gps from gps vector
		pos_info_opt_t ds_dashcam::load_next_gps()
		{
			pos_info_opt_t gps_ds;
			if (_gps_input_status)
			{
				if(_vec_gps_info.size())
				{
					//std::cout << _image_index<<" : _timestamp = "<< _timestamp << " gps timestamp =" << _vec_gps_info[_gps_index]._timestamp << std::endl;
					if ((int64_t)_timestamp >= _vec_gps_info[_gps_index]._timestamp)
					{
						
						long double lat = _vec_gps_info[_gps_index]._latitude, lon = _vec_gps_info[_gps_index]._longitude, alt = 0;
						gps_ds = pos_info{ Eigen::Vector3d(lat ,lon ,alt) ,(Eigen::Matrix3d::Random()) };
						_gps_index++;
					}
				}
			}
			return  gps_ds;
		}

		ds_dashcam ds_dashcam::begin()
		{
			if (_data_source_status)
				return ds_dashcam(_dashcam_args);

			return ds_dashcam();
		}

		ds_dashcam ds_dashcam::end()
		{
			return ds_dashcam();
		}

		bool ds_dashcam::equal(const ds_dashcam& other) const
		{
			return _image_index == other._image_index;
		}

		void ds_dashcam::increment()
		{
			get_next_item();
		}

		std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> & ds_dashcam::dereference() const
		{
			return (std::tuple<time_point_t, image_t, tsr_info_opt_t, pos_info_opt_t> & const)_item;
		}

	}
}