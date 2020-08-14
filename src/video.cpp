extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>

#include <iostream>

#include "utils.hpp"
#include "video.hpp"

using namespace std;

void saveFrame(uint8_t* data[4], int linesize[4], int width, int height, int iFrame) {
    FILE* pFile;
    char szFilename[32];
    int y;

    // Open file
    sprintf(szFilename, "frame%d.ppm", iFrame);
    pFile = fopen(szFilename, "wb");
    if (pFile == NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for (y = 0; y < height; y++)
        fwrite(data[0] + y * linesize[0], 1, width * 3, pFile);

    // Close file
    fclose(pFile);
}

VideoFile::VideoFile(string filename) {
    formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, filename.c_str(), nullptr, nullptr) != 0) {
        throw AppException("AVLib", "Error opening file");
    }

    cout << "Format: " << formatContext->iformat->long_name << "  Duration: " << formatContext->duration << endl;

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        throw AppException("AVLib", "Error getting streams");
    }

    cout << "Loaded" << endl;

    AVCodecParameters* codecPar = nullptr;

    videoIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        codecPar = formatContext->streams[i]->codecpar;
        if (codecPar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;

            break;
        }
    }
    if (videoIndex < 0) {
        throw AppException("AVLib", "No video streams");
    }

    AVCodec* codec = nullptr;
    codec = avcodec_find_decoder(codecPar->codec_id);
    if (codec == nullptr) {
        throw AppException("AVLib", "Codec not found");
    }

    codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecContext, codecPar);

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        throw AppException("AVLib", "Can't open codec");
    }

    packet = av_packet_alloc();
    frame = av_frame_alloc();

    convertContext = sws_getContext(
        codecContext->width,
        codecContext->height,
        codecContext->pix_fmt,
        codecContext->width,
        codecContext->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        nullptr,
        nullptr,
        nullptr);

    if (av_image_alloc(rgb_data, rgb_linesize, codecContext->width, codecContext->height, AV_PIX_FMT_RGB24, 1) < 0) {
        throw AppException("AVLib", "Error allocating image");
    }

    width = codecContext->width;
    height = codecContext->height;
}

VideoFile::~VideoFile() {
    av_freep(&rgb_data[0]);
    sws_freeContext(convertContext);
    av_free(frame);
    av_packet_free(&packet);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);
}

int my_read_frame(AVFormatContext* s, AVPacket* pkt) {
    EASY_FUNCTION();
    return av_read_frame(s, pkt);
}

void* VideoFile::fetchFrame() {
    EASY_FUNCTION();
    while (my_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == videoIndex) {
            EASY_BLOCK("avcodec_send_packet");
            int resp = avcodec_send_packet(codecContext, packet);
            if (resp < 0) {
                cout << "Error sending data to codec" << endl;
                continue;
            }
            EASY_END_BLOCK;

            while (resp >= 0) {
                EASY_BLOCK("Decode");
                resp = avcodec_receive_frame(codecContext, frame);
                EASY_END_BLOCK;
                if (resp == AVERROR(EAGAIN) || resp == AVERROR_EOF) {
                    break;
                } else if (resp < 0) {
                    cout << "Error receiving frame" << endl;
                    break;
                }

                EASY_BLOCK("Convert");
                sws_scale(
                    convertContext,
                    (uint8_t const* const*)frame->data,
                    frame->linesize,
                    0,
                    codecContext->height,
                    rgb_data,
                    rgb_linesize);
                EASY_END_BLOCK;

                //saveFrame(rgb_data, rgb_linesize, codecContext->width, codecContext->height, 0);
                //return nullptr;
                //return rgb_data[0];
                return frame->data[0];
            }
        }
    }
    return nullptr;
}