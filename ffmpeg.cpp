#include"ffmpeg.h"
int VideoCaptureFfmpeg::init(){
    avcodec_register_all();  
    av_register_all();  
    avformat_network_init();  
    
    pkt = av_packet_alloc();
    avf = av_frame_alloc(); 
    return 0;
}
int VideoCaptureFfmpeg::free(){
    av_frame_free(&avf);
    av_packet_free(&pkt);
    avcodec_close(enc);
    avformat_close_input(&ic);  
    avformat_network_deinit();  
    return 0;
}
int VideoCaptureFfmpeg::open(std::string& filename){
    file_path = filename;
    if(avformat_open_input(&ic,filename.c_str(),0,0)!=0){
        printf("[ffmpeg Err] : can not open input video\n");
        return -1;
    }
    this->printFileInformation(filename);

    //流里可能包含视频流，音频流，或者字母流，找到视频流对应的流索引，并找到对应的解码器
    for(int i = 0; i < ic->nb_streams; ++i)
    {   
        enc = ic->streams[i]->codec;
        if(enc->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            width = enc->width;
            height = enc->height;
            video_stream = i;
            AVCodec* codec = avcodec_find_decoder(enc->codec_id);
            if(!codec)
            {
                printf("[ffmpeg Err] : video code not find!\n");
                return -2;
            }

            int err = avcodec_open2(enc, codec, NULL);
            if( err!= 0)
            {   
                printf("[ffmpeg Err] : can not open codec!\n ");
                return -3;
            }
        }
    }
    return 0;

}

int VideoCaptureFfmpeg::printFileInformation(std::string& filename){
    printf("------------- File Information ------------------\n");  
    av_dump_format(ic,0,filename.c_str(),0);  
    printf("-------------------------------------------------\n"); 
    return 0; 
}

int VideoCaptureFfmpeg::getFrameBGR(uint8_t* bgrBuffer){
    bool right_Frame = false;
    for(;;)
    {
        int err = av_read_frame(ic,pkt);
        if(err!=0)
        {
            return -1;
        }
        if(pkt->stream_index == video_stream)
        {  
            //送到解码器
            //printf("frame_id = %d\n", frame_id);
            int retPackt = avcodec_send_packet(enc, pkt);
            if(retPackt < 0)
            {
                printf("[ffmpeg Err] : send packet error!\n");
                continue;
            }
            int retDecode = avcodec_receive_frame(enc,avf);
            switch (retDecode) {
                case 0://成功
                    //printf("got a frame !\n");
                    right_Frame = true;
                    printf("height is %d,  width is %d \n",avf->height, avf->width);
                    //printf("data format is %d\n", avf->format);
                    yuv_to_bgr( bgrBuffer, avf->data[0], avf->data[1], avf->data[2], avf->width, avf->height, 1, 601);
                    // for(int i=0; i< 10; ++i)
                    // {
                    //     printf("%d  " , bgrBuffer[i]); 
                    //     printf("%d  \n", avf->data[2][i]);
                    // }
                    //printf("\n");
                    frame_id++;    
                    break;

                case AVERROR_EOF:
                    printf("[ffmpeg Err] :the decoder has been fully flushed,\
                            and there will be no more output frames.\n");
                    break;

                case AVERROR(EAGAIN):
                    printf("[ffmpeg WARNING] :Resource temporarily unavailable\n");
                    break;

                case AVERROR(EINVAL):
                    printf("[ffmpeg Err] :Invalid argument\n");
                    break;
                default:
                    break;
            }

            
        }
        if(right_Frame)
            break;
        
    }  
    return 0; 
}

// still have problom , HOW the ptr go back to the head of document?
int VideoCaptureFfmpeg::getFrameBGR(uint8_t* bgrBuffer,int target_id){
    if(avformat_open_input(&ic,file_path.c_str(),0,0)!=0){
        printf("[ffmpeg Err] : can not open input video\n");
        return -1;
    }
    frame_id = -1;
    bool right_Frame = false;
    memset(pkt, 0, sizeof(AVPacket));  
    //av_packet_unref(pkt); 
    for(;;)
    {
        int err = av_read_frame(ic,pkt);
        if(err!=0)
        {
            printf("fuck %d\n", err);
            return -1;
        }
        if(pkt->stream_index == video_stream)
        {  
            //送到解码器
            //printf("frame_id = %d\n", frame_id);
            int retPackt = avcodec_send_packet(enc, pkt);
            if(retPackt < 0)
            {
                printf("[ffmpeg Err] : send packet error!\n");
                continue;
            }
            int retDecode = avcodec_receive_frame(enc,avf);
            switch (retDecode) {
                case 0://成功
                    //printf("got a frame !\n");
                    right_Frame = true;
                    printf("height is %d,  width is %d \n",avf->height, avf->width);
                    printf("data format is %d\n", avf->format);
                    yuv_to_bgr( bgrBuffer, avf->data[0], avf->data[1], avf->data[2], avf->width, avf->height, 1, 601);
                    // for(int i=0; i< 10; ++i)
                    // {
                    //     printf("%d  " , bgrBuffer[i]); 
                    //     printf("%d  \n", avf->data[2][i]);
                    // }
                    // printf("\n");
                    frame_id++;    
                    break;

                case AVERROR_EOF:
                    printf("[ffmpeg Err] :the decoder has been fully flushed,\
                            and there will be no more output frames.\n");
                    break;

                case AVERROR(EAGAIN):
                    printf("[ffmpeg WARNING] :Resource temporarily unavailable\n");
                    break;

                case AVERROR(EINVAL):
                    printf("[ffmpeg Err] :Invalid argument\n");
                    break;
                default:
                    break;
            }
        }
        if(right_Frame && frame_id == target_id )
            return 0; 
    }  
    return -1; 
}

////////////////////
int VideoCaptureFfmpeg::uint8ToAvframe(uint8_t* gray_image, AVFrame* avf_gray){
    ////
}