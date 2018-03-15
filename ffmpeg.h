#ifndef FFMPEG_H
#define FFMPEG_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
extern "C"
{
#include "include/libavformat/avformat.h"
}
#include "libyuv.h"
#include "AutoToucher.h"

class VideoCaptureFfmpeg{
public:
    VideoCaptureFfmpeg() : video_stream(0), ic(NULL), enc(NULL), codec(NULL),
    pkt(NULL), avf(NULL),frame_id(-1),width(0), height(0){}
    ~VideoCaptureFfmpeg(){}
    
    int init();
    int printFileInformation(std::string& filename);
    int open(std::string& filename);
    //int getFrameYUV420P(uint8_t* yuv420pBuffer);
    int getFrameBGR(uint8_t* bgrBuffer);
    int getFrameBGR(uint8_t* bgrBuffer,int frame_id);
    int free();

    int width;
    int height;
    
    //当前有效帧号
    int frame_id;

private:
    //视频流索引
    int video_stream;

    //保存封装流格式信息，解封装（flv, avi, rmvb, mp4）
    AVFormatContext* ic;

    //保存解码器信息
    AVCodecContext* enc;

    //解码器
    AVCodec* codec;

    //视频流数据包（解码前）
    AVPacket* pkt;

    //解码后的帧信息(解码后)
    AVFrame* avf;

    //file name
    std::string file_path;

    // // //bgr桢信息
    // // uint8_t* bgrBuffer;


    ////////////////////////////////////////////////////////
    //encode
    AVFrame* avf_encode;
    int uint8ToAvframe(uint8_t* gray_image, AVFrame* avf_gray);

};

#endif