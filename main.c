#include <stdio.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


void print_error(int code) ;
void fail(int error, int code) ;
void fail_nz(int error, int code) ;
void fail_true(int bool, int code, char* message) ;
void fail_false(int bool, int code, char* message) ;
void fail_null(void* ptr, int code, char* message) ;


// rtp:// use 2 ports, udp:// use mpegts by default
// video parameters set by url query parameters or in av_dict_set(dic)
// http://ffmpeg.org/ffmpeg-protocols.html#rtp
// http://ffmpeg.org/ffmpeg-protocols.html#udp
// ffmpeg -re -loop 1 -i <JPEG> -r 60 -an -vcodec mjpeg -f mpegts udp://127.0.0.1:43210


int main(int argc, char** argv) {

    av_log_set_level(AV_LOG_VERBOSE) ;

    av_register_all() ;
    avcodec_register_all() ;
    avformat_network_init() ;

    AVFormatContext *fmtCtx = avformat_alloc_context() ;

    fail_nz( avformat_open_input( &fmtCtx, "udp://127.0.0.1:43210", NULL, NULL ), 255 ) ;
    fail_nz( avformat_find_stream_info(fmtCtx, NULL), 255 ) ;
    fail_false(fmtCtx->nb_streams == 1, 255, "Number of streams not 1") ;

    av_dump_format(fmtCtx, 0, argv[1], 0) ;

    AVCodec *codec = avcodec_find_decoder( AV_CODEC_ID_MJPEG ) ;
    //AVCodec *codec = avcodec_find_decoder( fmtCtx->streams[0]->codec->codec_id ) ;
    fail_null( codec, 255, "Can't find codec" ) ;

    printf("Format: %s\n", fmtCtx->iformat->long_name) ;
    printf("Codec: %s\n",  codec->long_name) ;

    AVCodecContext *ccCtx = avcodec_alloc_context3(codec) ;
    fail_nz( avcodec_open2( ccCtx, codec, NULL ), 255 ) ;

    printf("%dx%d\n", ccCtx->width, ccCtx->height) ;

    numBytes = avpicture_get_size(PIX_FMT_RGB24, 620, 621) ;
    buffer = (uint8_t *)av_malloc( numBytes*sizeof(uint8_t) ) ;
    avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, 620, 621) ;

//av_read_frame()
    //avcodec_close()
    //avformat_free_context(fmtCtx) ;
    free( fmtCtx ) ; // no need if avformat_open_input() fails

    return 0 ;
}

void print_error(int code) {
    char buf[1024] ;
    int r = av_strerror(code, buf, 1024) ;
    if(r==0)
        printf(stderr, "Error: %s", buf) ;
    else
        printf(stderr, "Can't print error description for code: %d\n", code) ;
}

void fail(int error, int code) {
    print_error(error) ;
    exit(code) ;
}

void fail_nz(int error, int code) {
    if(error != 0) {
        fail(error, code) ;
    }
}

void fail_true(int bool, int code, char* message) {
    if(bool == 1) {
        printf(stderr, "%s\n", message) ;
        exit(code) ;
    }
}

void fail_false(int bool, int code, char* message) {
    fail_true(!bool, code, message) ;
}

void fail_null(void* ptr, int code, char* message) {
    fail_true(ptr == (void*)0, code, message) ;
}
