#include <stdio.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


int main(int argc, char** argv) {
    //AVStream *stream ;

    av_log_set_level(AV_LOG_DEBUG) ;
    //av_log_set_callback()
    //av_log_default_callback()

    av_register_all() ;
    avcodec_register_all() ;
    avformat_network_init() ;

    AVFormatContext *fmtCtx = avformat_alloc_context() ;
    // rtp: 2 in ports
    // url or av_dict_set
    // http://ffmpeg.org/ffmpeg-protocols.html#rtp
    // http://ffmpeg.org/ffmpeg-protocols.html#udp
    // ffmpeg -re -loop 1 -i <JPEG> -r 60 -an -vcodec mjpeg -f mpegts udp://127.0.0.1:43210
    int r = avformat_open_input( &fmtCtx, "udp://127.0.0.1:43210", NULL, NULL ) ;
    printf("ctx: %d\n", (unsigned int)fmtCtx) ;

    char buf[1024] ;
    r = av_strerror(r,buf,1024) ;
    printf("%s", buf) ;

    r = avformat_find_stream_info(fmtCtx, NULL) ;
//    printf("%d\n", fmtCtx->nb_streams) ;

    free( fmtCtx ) ; // if avformat_open_input not fail



    return 0 ;
}
