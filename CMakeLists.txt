cmake_minimum_required(VERSION 3.3)
project(helloc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

if(WIN32)
    set(X_EMB_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-3.2.2-win64)
    find_library( FFMPEG_LIBAVFORMAT NAMES avformat PATHS ${X_EMB_LIBS_PATH} NO_DEFAULT_PATH )
    find_library( FFMPEG_LIBAVCODEC  NAMES avcodec  PATHS ${X_EMB_LIBS_PATH} NO_DEFAULT_PATH )
else(WIN32)
    find_library( FFMPEG_LIBAVFORMAT NAMES avformat )
    find_library( FFMPEG_LIBAVCODEC  NAMES avcodec )
endif(WIN32)

set(SOURCE_FILES main.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#add_library(avcodec SHARED IMPORTED)
#set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION lib/avcodec-57.dll)
#set_target_properties(avcodec PROPERTIES IMPORTED_IMPLIB   lib/avcodec.lib)

link_directories(lib)
include_directories( ${X_EMB_LIBS_PATH}/include )
target_link_libraries(
    ${PROJECT_NAME}
    ${FFMPEG_LIBAVCODEC}
    ${FFMPEG_LIBAVFORMAT}
)
