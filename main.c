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

void print_coded_params(AVCodecParameters *p) ;

// rtp:// use 2 ports, udp:// use mpegts by default
// video parameters set by url query parameters or in av_dict_set(dic)
// http://ffmpeg.org/ffmpeg-protocols.html#rtp
// http://ffmpeg.org/ffmpeg-protocols.html#udp
// ffmpeg -re -loop 1 -i <JPEG> -r 60 -an -vcodec mjpeg -f mpegts udp://127.0.0.1:43210

// https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/demuxing_decoding.c
// https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/avio_reading.c


int main(int argc, char** argv) {

    av_log_set_level(AV_LOG_VERBOSE) ;

    // Register all formats and codecs
    av_register_all() ;
    //avcodec_register_all() ;
    avformat_network_init() ;

    AVFormatContext *fmt_ctx = avformat_alloc_context() ;

    fail_nz( avformat_open_input( &fmt_ctx, "udp://192.168.10.130:43210", NULL, NULL ), 255 ) ;
    fail_nz( avformat_find_stream_info(fmt_ctx, NULL), 255 ) ;
    fail_false(fmt_ctx->nb_streams == 1, 255, "Number of streams not 1") ;
    fail_false(fmt_ctx->streams[0]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO, 255, "Not a video stream") ;

    //av_dump_format(fmtCtx, 0, argv[1], 0) ;

    //AVCodec *codec = avcodec_find_decoder( AV_CODEC_ID_MJPEG ) ;
    AVCodec *dec = avcodec_find_decoder( fmt_ctx->streams[0]->codecpar->codec_id ) ;
    //printf( "Codec id: %d\n", fmtCtx->streams[0]->codecpar->codec_id ) ;
    fail_null( dec, 255, "Can't find codec" ) ;

    printf("Format: %s\n", fmt_ctx->iformat->long_name) ;
    printf("Codec: %s\n",  dec->long_name) ;

    AVCodecContext *dec_ctx = avcodec_alloc_context3(dec) ;
    fail_null( dec_ctx, 255, "Can't allocate decoder context" ) ;

    print_coded_params( fmt_ctx->streams[0]->codecpar ) ;

    fail_nz( avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[0]->codecpar) , 255 ) ;
    fail_nz( avcodec_open2( dec_ctx, dec, NULL ), 255 ) ;

    printf("%dx%d\n", dec_ctx->width, dec_ctx->height) ;

    // av_frame_free?
    AVFrame *frame = av_frame_alloc() ;
    fail_null(frame, 255, "Can't allocate frame") ;

    // free packet?
    AVPacket pkt ;
    av_init_packet(&pkt) ;
    pkt.data = NULL ; // demuxer fill it
    pkt.size = 0 ;

    while ( av_read_frame(fmt_ctx, &pkt) >= 0 ) {

    }

    //numBytes = avpicture_get_size(PIX_FMT_RGB24, 620, 621) ;
    //buffer = (uint8_t *)av_malloc( numBytes*sizeof(uint8_t) ) ;
    //avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, 620, 621) ;

//av_read_frame()
    //avcodec_close()
    //avformat_free_context(fmtCtx) ;
    // free

    avcodec_free_context(&dec_ctx) ;
    avformat_close_input(&fmt_ctx) ;
    av_free( &fmt_ctx ) ; // no need if avformat_open_input() fails

    return 0 ;
}

void print_coded_params(AVCodecParameters *p) {
}

void print_error(int code) {
    char buf[1024] ;
    int r = av_strerror(code, buf, 1024) ;
    if(r==0)
        fprintf(stderr, "Error: %s", buf) ;
    else
        fprintf(stderr, "Can't print error description for code: %d\n", code) ;
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
        fprintf(stderr, "%s\n", message) ;
        exit(code) ;
    }
}

void fail_false(int bool, int code, char* message) {
    fail_true(!bool, code, message) ;
}

void fail_null(void* ptr, int code, char* message) {
    fail_true(ptr == (void*)0, code, message) ;
}
