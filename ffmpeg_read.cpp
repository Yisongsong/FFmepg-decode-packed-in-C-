#include "ffmpeg.h"
#include<iostream>
#include<fstream>
#include <string>
using namespace std;
int main(){
    printf("teeeest ffmpeg class\n");
    string inputPath = "test.mp4";
    int ret = 0;
    VideoCaptureFfmpeg videoCap;
    ret = videoCap.init();
    ret = videoCap.open(inputPath);
    uint8_t* bgrBuffer = (uint8_t*)malloc( 3 * videoCap.width * videoCap.height * sizeof( uint8_t ) );

    ret = videoCap.getFrameBGR(bgrBuffer);
    while(ret == 0){
        //todo
        printf("frame id is : %d\n", videoCap.frame_id);
        ret = videoCap.getFrameBGR(bgrBuffer);
    }

    FILE* bgrBin;
    bgrBin = fopen("bgr.bin","w");
    fwrite(bgrBuffer,3 * videoCap.width * videoCap.height * sizeof( uint8_t ),1,bgrBin);
    fclose(bgrBin);
    ret = videoCap.free();
    return 0;
}




