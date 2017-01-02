cmake_minimum_required(VERSION 3.3)
project(helloc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")

set(SOURCE_FILES main.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if( WIN32 )
    message("WINDOWS build")
    set( _V_PROVIDED_LIBS_PATH ${CMAKE_SOURCE_DIR}/ffmpeg-provided/3.2.2/win64/lib )
    set( _V_PROVIDED_INCL_PATH ${CMAKE_SOURCE_DIR}/ffmpeg-provided/3.2.2/win64/include )
else( WIN32 )
    message("LINUX build")
    set( _V_PROVIDED_LIBS_PATH ${CMAKE_SOURCE_DIR}/ffmpeg-provided/3.2.2/lin64/lib )
    set( _V_PROVIDED_INCL_PATH ${CMAKE_SOURCE_DIR}/ffmpeg-provided/3.2.2/lin64/include )

    set( CMAKE_INSTALL_RPATH ${_V_PROVIDED_LIBS_PATH} )
endif( WIN32 )

include_directories( ${_V_PROVIDED_INCL_PATH} )

find_library( FFMPEG_LIBAVUTIL     NAMES avutil     PATHS ${_V_PROVIDED_LIBS_PATH} NO_DEFAULT_PATH )
find_library( FFMPEG_LIBAVFORMAT   NAMES avformat   PATHS ${_V_PROVIDED_LIBS_PATH} NO_DEFAULT_PATH )
find_library( FFMPEG_LIBAVCODEC    NAMES avcodec    PATHS ${_V_PROVIDED_LIBS_PATH} NO_DEFAULT_PATH )
find_library( FFMPEG_LIBSWRESAMPLE NAMES swresample PATHS ${_V_PROVIDED_LIBS_PATH} NO_DEFAULT_PATH )


#add_library(avcodec SHARED IMPORTED)
#set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION lib/avcodec-57.dll)
#set_target_properties(avcodec PROPERTIES IMPORTED_IMPLIB   lib/avcodec.lib)

#link_directories(lib)

target_link_libraries(
    ${PROJECT_NAME}
    ${FFMPEG_LIBAVUTIL}
    ${FFMPEG_LIBAVCODEC}
    ${FFMPEG_LIBAVFORMAT}
    ${FFMPEG_LIBSWRESAMPLE}
)
