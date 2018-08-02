#pragma once
#include <iostream>
#include <utils.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <boost/algorithm/string.hpp>

using namespace std;

namespace utils {
    //check for character A or V from gps string
    //A for active(is good)
    //V for void(there's an unexpected problem with the position reading.)
    bool valid_gps(const std::string &src)
    {
        std::vector<std::string> result;
        int active_gps_offset = 9;
        boost::split(result, src, boost::is_any_of(","));
        return ((20 <= result.size()) && (!result[active_gps_offset].compare("A")));
    }

    bool extract_gps_from_video(const std::string &video_file_, std::vector<gps_subtitle> &gps_subtitles_)
    {
        AVCodecContext    *pCodecCtxOrig = NULL;
        AVCodecContext    *pCodecCtx = NULL;
        AVCodec           *pCodec = NULL;
        AVFrame           *pFrame = NULL;
        int               frameFinished;
        AVPacket          packet;
        AVSubtitle        *sub = NULL;
        AVFormatContext   *pFormatCtx = NULL;
        int               subtitle_index = -1;
        bool              extract_gps_status = 0;

        av_register_all();

        if (avformat_open_input(&pFormatCtx, video_file_.c_str(), NULL, NULL) != 0)
        {
            std::cerr << "failed to open!!! " << video_file_ << std::endl;
            return false;
        }
        if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
        {
            std::cerr << "failed to retieve " << video_file_ << std::endl;
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
            std::cout << "failed to find gps info from " << video_file_ << std::endl;
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
                    gps_subtitle gps_list;
                    
                    for (int j = 0; j < sub->num_rects; j++)
                    {
                        if ((sub->rects[j]->type == SUBTITLE_ASS) && (sub->rects[j]->ass)&& valid_gps(sub->rects[j]->ass))
                        {
							int frame_duration = sub->end_display_time - sub->start_display_time;
							gps_list._timestamp = sub->pts / frame_duration;
							gps_list._frame_number = pCodecCtx->frame_number;
							gps_list._gps_str = sub->rects[j]->ass;
							gps_subtitles_.push_back(gps_list);
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

        if (gps_subtitles_.size())
            extract_gps_status = true;

        return extract_gps_status;
    }

}