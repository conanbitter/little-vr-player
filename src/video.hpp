#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <string>

using namespace std;

class VideoFile {
   private:
    AVFormatContext* formatContext;
    AVCodecContext* codecContext;
    AVPacket* packet;
    AVFrame* frame;
    struct SwsContext* convertContext;
    uint8_t* rgb_data[4];
    int rgb_linesize[4];
    int videoIndex;
    int width;
    int height;

   public:
    explicit VideoFile(string filename);
    ~VideoFile();
    void* fetchFrame();
    void getSize(int& w, int& h) {
        w = width;
        h = height;
    }
};