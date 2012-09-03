
cmake_minimum_required (VERSION 2.6)
project (VillanovaEngine)


set (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)



set (SOURCE_FILES
	src/async-queue.c
	src/buffer.c
	src/clock.c
	src/codec.c
	src/codec-types.c
	src/colorspace.c
	src/demuxer.c
	src/engine.c
	src/error.c
	src/frame.c
	src/input.c
	src/list.c
	src/mem.c
	src/modules.c
	src/output.c
	src/queue.c
	src/stream.c
)



set (INPUT_SOURCE_FILES
	src/input/file.c)

set (OUTPUT_SOURCE_FILES
	src/output/alsa.c
	src/output/xv.c)

set (DEMUX_SOURCE_FILES
	src/demux/mpeg.c
	src/demux/libavformat.c)

set (CODEC_SOURCE_FILES
	src/codec/libavcodec.c)




find_package (Threads)
find_package (PkgConfig)



pkg_check_modules (ALSA REQUIRED alsa)
pkg_check_modules (XV REQUIRED xv)
pkg_check_modules (LIBAVFORMAT REQUIRED libavformat)
pkg_check_modules (LIBAVCODEC REQUIRED libavcodec)
pkg_check_modules (LIBSWSCALE REQUIRED libswscale)



check_include_files (libavcodec/avcodec.h HAVE_LIBAVCODEC_AVCODEC_H)

configure_file (${PROJECT_SOURCE_DIR}/include/villanova-engine/config.h.in 
				${PROJECT_BINARY_DIR}/config.h)




set (OUTPUT_INCLUDE_DIRS
	${ALSA_INCLUDE_DIRS}
	${XV_INCLUDE_DIRS})

set (DEMUX_INCLUDE_DIRS
	${LIBAVFORMAT_INCLUDE_DIRS})

set (CODEC_INCLUDE_DIRS
	${LIBAVCODEC_INCLUDE_DIRS}
	${LIBSWSCALE_INCLUDE_DIRS})



set (OUTPUT_LIBRARIES
	${ALSA_LIBRARIES}
	${XV_LIBRARIES})

set (DEMUX_LIBRARIES
	${LIBAVFORMAT_LIBRARIES})

set (CODEC_LIBRARIES
	${LIBAVCODEC_LIBRARIES}
	${LIBSWSCALE_LIBRARIES})



include_directories (
	include
	include/villanova-engine
	${PROJECT_BINARY_DIR}
	
	${OUTPUT_INCLUDE_DIRS}
	${DEMUX_INCLUDE_DIRS}
	${CODEC_INCLUDE_DIRS}
)


add_library (villanova-engine SHARED
	${SOURCE_FILES}
	${INPUT_SOURCE_FILES}
	${OUTPUT_SOURCE_FILES}
	${CODEC_SOURCE_FILES}
	${DEMUX_SOURCE_FILES}
)


target_link_libraries (villanova-engine
	${CMAKE_THREAD_LIBS_INIT}
	${INPUT_LIBRARIES}
	${OUTPUT_LIBRARIES}
	${CODEC_LIBRARIES}
	${DEMUX_LIBRARIES}
)



add_executable (player player.c)
target_link_libraries (player villanova-engine) 

