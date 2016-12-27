cmake_minimum_required(VERSION 3.3)
project(helloc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

if(WINN32)
    set(X_EMB_LIBS_PATH "ffmpeg-20161227-0ff8c6b-win64-dev/lib")

    find_library( FFMPEG_LIBAVFORMAT NAMES avformat PATHS ${X_EMB_LIBS_PATH} NO_DEFAULT_PATH )
    find_library( FFMPEG_LIBAVCODEC  NAMES avcodec  PATHS ${X_EMB_LIBS_PATH} NO_DEFAULT_PATH )
else(WINN32)
    find_library( FFMPEG_LIBAVFORMAT NAMES avformat )
    find_library( FFMPEG_LIBAVCODEC  NAMES avcodec )
endif(WINN32)


set(SOURCE_FILES main.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#add_library(avcodec2 SHARED IMPORTED)
#set_target_properties(avcodec2 PROPERTIES IMPORTED_LOCATION testlib/libavcodec2.so)

#include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-20161227-0ff8c6b-win64-dev/include )
target_link_libraries(
    ${PROJECT_NAME}
    ${FFMPEG_LIBAVFORMAT}
    ${FFMPEG_LIBAVCODEC}
)
