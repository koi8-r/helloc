cmake_minimum_required(VERSION 3.3)
project(helloc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCE_FILES main.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if( WIN32 )
    message("WINDOWS build")

    set( _V_PROVIDED_LIBS_PATH ${CMAKE_SOURCE_DIR}/ffmpeg-provided/3.2.2/libs64 )
    set( _V_PROVIDED_INCL_PATH ${CMAKE_SOURCE_DIR}/ffmpeg-provided/3.2.2/include64 )

    find_library( FFMPEG_LIBAVUTIL   NAMES avutil   PATHS ${_V_PROVIDED_LIBS_PATH} NO_DEFAULT_PATH )
    find_library( FFMPEG_LIBAVFORMAT NAMES avformat PATHS ${_V_PROVIDED_LIBS_PATH} NO_DEFAULT_PATH )
    find_library( FFMPEG_LIBAVCODEC  NAMES avcodec  PATHS ${_V_PROVIDED_LIBS_PATH} NO_DEFAULT_PATH )

    include_directories( ${_V_PROVIDED_INCL_PATH} )
else( WIN32 )
    find_library( FFMPEG_LIBAVFORMAT NAMES avformat )
    find_library( FFMPEG_LIBAVCODEC  NAMES avcodec )
endif( WIN32 )

#add_library(avcodec SHARED IMPORTED)
#set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION lib/avcodec-57.dll)
#set_target_properties(avcodec PROPERTIES IMPORTED_IMPLIB   lib/avcodec.lib)

#link_directories(lib)

target_link_libraries(
    ${PROJECT_NAME}
    ${FFMPEG_LIBAVUTIL}
    ${FFMPEG_LIBAVCODEC}
    ${FFMPEG_LIBAVFORMAT}
)
