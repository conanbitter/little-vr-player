extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/hwcontext.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}
#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>

#include <iostream>
#include <string>

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

void convert(uint8_t* dst, uint8_t* src[8], int linesize[4], int width, int height) {
    EASY_FUNCTION();
    /*for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int dstp = (y * width + x) * 3;
            int srcp0 = y * linesize[0] + x;
            int srcp1 = (y * linesize[1] + x) * 3;
            uint8_t r = src[0][srcp0];
            uint8_t g = 128;
            uint8_t b = 128;
            dst[dstp] = r;
            dst[dstp + 1] = r;
            dst[dstp + 2] = r;
        }
    }*/
    memcpy(dst, src[0], width * height);
    memcpy(dst + width * height, src[1], width / 2 * height / 2);
}

static AVBufferRef* hw_device_ctx = NULL;
static enum AVPixelFormat hw_pix_fmt;
static char charbuf[1000];

static int hw_decoder_init(AVCodecContext* ctx, const enum AVHWDeviceType type) {
    int err = 0;

    if ((err = av_hwdevice_ctx_create(&hw_device_ctx, type,
                                      NULL, NULL, 0)) < 0) {
        throw AppException("AVLibHW", "Failed to create specified HW device.");
    }
    ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);

    return err;
}

static enum AVPixelFormat get_hw_format(AVCodecContext* ctx,
                                        const enum AVPixelFormat* pix_fmts) {
    const enum AVPixelFormat* p;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt)
            return *p;
    }

    cout << "Failed to get HW surface format." << endl;
    return AV_PIX_FMT_NONE;
}

string getPixelFormat(AVPixelFormat format) {
    //av_get_pix_fmt_string(charbuf, 1000, format);
    //return string(charbuf);
    return string(av_get_pix_fmt_name(format));
}

void printPixFormats(AVBufferRef* ctx) {
    cout << "Acceptable transfer pixel formats:" << endl;
    enum AVPixelFormat* formats;
    av_hwframe_transfer_get_formats(ctx, AV_HWFRAME_TRANSFER_DIRECTION_FROM, &formats, 0);
    for (auto pxf = *formats; *formats != AV_PIX_FMT_NONE; formats++) {
        cout << "- " << getPixelFormat(pxf) << endl;
    }
}

VideoFile::VideoFile(string filename) {
    auto type = AV_HWDEVICE_TYPE_NONE;
    cout << "HW Devices:" << endl;
    while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE)
        cout << "- " << av_hwdevice_get_type_name(type) << endl;

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

    cout << "Videocodec: " << codec->long_name << endl;

    for (int i = 0;; i++) {
        const AVCodecHWConfig* config = avcodec_get_hw_config(codec, i);
        if (!config) {
            cout << "Decoder " << codec->name << " does not support HW acceleration" << endl;
            break;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX) {
            cout << "Best HW config: #" << i << " type:" << av_hwdevice_get_type_name(config->device_type) << endl;
            type = config->device_type;
            hw_pix_fmt = config->pix_fmt;
            break;
        }
    }

    codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecContext, codecPar);

    codecContext->get_format = get_hw_format;

    if (hw_decoder_init(codecContext, type) < 0) {
        throw AppException("AVLib HW", "Error setting up HW config");
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        throw AppException("AVLib", "Can't open codec");
    }

    cout << "Software pixel format:" << getPixelFormat(codecContext->pix_fmt) << endl;
    cout << "Hardware pixel format: " << getPixelFormat(hw_pix_fmt) << endl;

    packet = av_packet_alloc();
    frame = av_frame_alloc();
    sw_frame = av_frame_alloc();  //codecContext->pix_fmt
    //sw_frame->format = AV_PIX_FMT_RGB24;

    //printPixFormats(codecContext->hw_device_ctx);

    convertContext = sws_getContext(
        codecContext->width,
        codecContext->height,
        AV_PIX_FMT_NV12,
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
    readPacket();
}

VideoFile::~VideoFile() {
    av_freep(&rgb_data[0]);
    sws_freeContext(convertContext);
    av_free(frame);
    av_free(sw_frame);
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
                av_packet_unref(packet);
                return frame->data[0];
            }
        }
    }
    return nullptr;
}

int VideoFile::readPacket() {
    EASY_FUNCTION();
    av_packet_unref(packet);
    while (my_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == videoIndex) {
            int resp = avcodec_send_packet(codecContext, packet);
            if (resp < 0) {
                throw AppException("AVLib", "Error sending data to codec");
            }
            return resp;
        }
        av_packet_unref(packet);
    }
    return AVERROR_EOF;
}

int my_hwframe_transfer_data(AVFrame* dst, const AVFrame* src, int flags) {
    EASY_FUNCTION();
    return av_hwframe_transfer_data(dst, src, flags);
}

bool first = true;

void* VideoFile::fetchFrame2() {
    AVFrame* tmp_frame = NULL;
    EASY_FUNCTION();
    for (;;) {
        EASY_BLOCK("Receive Frame");
        int resp = avcodec_receive_frame(codecContext, frame);
        if (resp == AVERROR(EAGAIN)) {
            resp = readPacket();
            if (resp == AVERROR_EOF) {
                return nullptr;
            }
            continue;
        }
        if (resp == AVERROR_EOF) {
            return nullptr;
        }
        if (resp < 0) {
            throw AppException("AVLib", "Error receiving frame");
        }
        EASY_END_BLOCK;
        EASY_BLOCK("Video HW to SW");
        if (frame->format == hw_pix_fmt) {
            /* retrieve data from GPU to CPU */
            if (my_hwframe_transfer_data(sw_frame, frame, 0) < 0) {
                throw AppException("AVLib HW", "Error transferring the data to system memory");
            }
            tmp_frame = sw_frame;
            if (first) {
                //printPixFormats(frame->hw_frames_ctx);
                cout << "Transfered pixel format: " << getPixelFormat((AVPixelFormat)sw_frame->format) << endl;
                first = false;
            }
        } else
            tmp_frame = frame;
        EASY_END_BLOCK;
        EASY_BLOCK("Convert");
        sws_scale(
            convertContext,
            (uint8_t const* const*)tmp_frame->data,
            tmp_frame->linesize,
            0,
            codecContext->height,
            rgb_data,
            rgb_linesize);
        EASY_END_BLOCK;
        //return tmp_frame->data[0];
        return rgb_data[0];
    }
}

void VideoFile::fetchFrame3(void* dst) {
    AVFrame* tmp_frame = NULL;
    EASY_FUNCTION();
    for (;;) {
        EASY_BLOCK("Receive Frame");
        int resp = avcodec_receive_frame(codecContext, frame);
        if (resp == AVERROR(EAGAIN)) {
            resp = readPacket();
            if (resp == AVERROR_EOF) {
                return;
            }
            continue;
        }
        if (resp == AVERROR_EOF) {
            return;
        }
        if (resp < 0) {
            throw AppException("AVLib", "Error receiving frame");
        }
        EASY_END_BLOCK;
        EASY_BLOCK("Video HW to SW");
        if (frame->format == hw_pix_fmt) {
            /* retrieve data from GPU to CPU */
            if (my_hwframe_transfer_data(sw_frame, frame, 0) < 0) {
                throw AppException("AVLib HW", "Error transferring the data to system memory");
            }
            tmp_frame = sw_frame;
            if (first) {
                //printPixFormats(frame->hw_frames_ctx);
                cout << "Transfered pixel format: " << getPixelFormat((AVPixelFormat)sw_frame->format) << endl;
                first = false;
            }
        } else
            tmp_frame = frame;
        EASY_END_BLOCK;
        EASY_BLOCK("Convert");
        /*rgb_data[0] = (uint8_t*)dst;
        sws_scale(
            convertContext,
            (uint8_t const* const*)tmp_frame->data,
            tmp_frame->linesize,
            0,
            codecContext->height,
            rgb_data,
            rgb_linesize);*/
        convert((uint8_t*)dst, tmp_frame->data, tmp_frame->linesize, codecContext->width, codecContext->height);
        EASY_END_BLOCK;
        //return tmp_frame->data[0];
        //memcpy(dst, rgb_data[0], codecContext->width * codecContext->height * 3);
        return;
    }
}