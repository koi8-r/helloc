#include <stdio.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>


void print_error(int code) ;
void fail(int error, int code) ;
void fail_nz(int error, int code) ;
void fail_true(int bool, int code, char* message) ;
void fail_false(int bool, int code, char* message) ;
void fail_null(void* ptr, int code, char* message) ;


int main(int argc, char** argv) {

    av_log_set_level(AV_LOG_VERBOSE) ;

    // Register all formats and codecs
    av_register_all() ;
    //avcodec_register_all() ;
    avformat_network_init() ;

    AVFormatContext *fmt_ctx = avformat_alloc_context() ;

    fail_nz( avformat_open_input( &fmt_ctx, "test_512.jpg", NULL, NULL ), 255 ) ;
    fail_nz( avformat_find_stream_info(fmt_ctx, NULL), 254 ) ;
    fail_false(fmt_ctx->nb_streams == 1, 253, "Number of streams not 1") ;
    fail_false(fmt_ctx->streams[0]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO, 252, "Not a video stream") ;

    //av_dump_format(fmt_ctx, 0, argv[1], 0) ;

    //AVCodec *codec = avcodec_find_decoder( AV_CODEC_ID_MJPEG ) ;
    AVCodec *dec = avcodec_find_decoder( fmt_ctx->streams[0]->codecpar->codec_id ) ;
    fail_null( dec, 251, "Can't find codec" ) ;

    printf("Format: %s\n", fmt_ctx->iformat->long_name) ;
    printf("Codec: %s\n",  dec->long_name) ;

    AVCodecContext *dec_ctx = avcodec_alloc_context3(dec) ;
    fail_null( dec_ctx, 250, "Can't allocate decoder context" ) ;

    fail_nz( avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[0]->codecpar) , 249 ) ;
    fail_nz( avcodec_open2( dec_ctx, dec, NULL ), 248 ) ;

    // TODO: check WxH non zero
    printf("%dx%d\n", dec_ctx->width, dec_ctx->height) ;


    // av_frame_free?
    AVFrame *frame = av_frame_alloc() ;
    fail_null(frame, 247, "Can't allocate frame") ;

    // free packet?
    AVPacket pkt ;
    av_init_packet(&pkt) ;
    pkt.data = NULL ; // demuxer fill it
    pkt.size = 0 ;

    static uint8_t *video_dst_data[4] = { NULL } ;
    static int video_dst_linesize[4] ;
    int frame_finished = 0 ;
    int avrfr = 0 ;
    while ( avrfr >= 0 ) {
        avrfr = av_read_frame(fmt_ctx, &pkt) ;
        printf("av_read_frame: %d\n", avrfr) ;
        print_error(avrfr) ;

        // Packet may contains single or multiple frames
        int  pkt_size = pkt.size ;
        unsigned char *pkt_data = pkt.data ;

        do {
            printf("pkt size: %d\n", pkt.size) ;
            int bytes = avcodec_decode_video2(dec_ctx, frame, &frame_finished, &pkt) ;
            printf("Decode %d bytes\n", bytes) ;
            fail_true(bytes < 0, 246, "Can't decode packet frames") ;
            printf("pkt size: %d\n", pkt.size) ;
            pkt_data += pkt.size ; // pkt.size or bytes var ?
            pkt_size -= pkt.size ;

            //if(got_picture) {
            // mpegts/mpeg2video error
            fail_false(frame_finished, 245, "Can't got picture during decode packet frames");

            // width|heigth|format must be equals in frame and dec_ctx
            printf("Picture number: %d\n", frame->coded_picture_number) ;
            printf("Pix fmt: %s\n", av_get_pix_fmt_name(dec_ctx->pix_fmt)) ;
            printf("Key frame?: %d\n", frame->pict_type == AV_PICTURE_TYPE_I) ;

            int img_bytes = av_image_alloc(
                video_dst_data, video_dst_linesize,
                dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt, 1
            ) ;

            //TODO: check img_bytes < 0
            printf("Allocate %d bytes for image\n", img_bytes);

            av_image_copy(
                video_dst_data, video_dst_linesize,
                (const uint8_t **) frame->data, frame->linesize,
                dec_ctx->pix_fmt, dec_ctx->width, dec_ctx->height
            );
            printf("Image copied to buffer\n");

            //img_convert PIX_FMT_YUV420P, PIX_FMT_RGB24
            //http://dranger.com/ffmpeg/tutorial08.html
            //https://www.codeproject.com/Tips/112274/FFmpeg-Tutorial

            av_freep(&video_dst_data[0]);
            // FIXME: ??? segfault av_frame_free( &frame ) ;
            //}

        } while ( pkt_size > 0 ) ;

        av_packet_unref( &pkt ) ;
        //break ; // FIXME: stub
    }

    // Flush cached frames
    pkt.data = NULL ;
    pkt.size = 0 ;
    do {
        int bytes = avcodec_decode_video2(dec_ctx, frame, &frame_finished, &pkt) ;
        printf("Flush %d bytes\n", bytes) ;
    } while (frame_finished) ;

    //numBytes = avpicture_get_size(PIX_FMT_RGB24, 620, 621) ;
    //buffer = (uint8_t *)av_malloc( numBytes*sizeof(uint8_t) ) ;
    //avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, 620, 621) ;

    avcodec_free_context(&dec_ctx) ;
    avformat_close_input(&fmt_ctx) ;
    // ??? segfault
    //av_free( &fmt_ctx ) ; // no need if avformat_open_input() fails

    return 0 ;
}

void print_coded_params(AVCodecParameters *p) {
}

void print_error(int code) {
    char buf[1024] ;
    int r = av_strerror(code, buf, 1024) ;
    if(r==0)
        fprintf(stderr, "Error: %s\n", buf) ;
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
