cmake_minimum_required(VERSION 2.6)

set(FIBJS_ROOT_DIR ${CMAKE_SOURCE_DIR}/../fibjs)

include(${FIBJS_ROOT_DIR}/fibjs/tools/arch.cmake)
include(${FIBJS_ROOT_DIR}/fibjs/tools/os.cmake)
include(${FIBJS_ROOT_DIR}/fibjs/tools/platform.cmake)
include(${FIBJS_ROOT_DIR}/fibjs/tools/subdirs.cmake)

project(fxjslib)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

file(GLOB_RECURSE src_list "src/*.c*")
file(GLOB_RECURSE fibjs_src_list "${FIBJS_ROOT_DIR}/fibjs/src/*.c*")
add_library(fxjslib ${src_list} ${fibjs_src_list})

set(BIN_DIR ${PROJECT_SOURCE_DIR}/../../bin/${OS}_${ARCH}_${BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH ${BIN_DIR})

include(CheckIncludeFiles)
include(CheckCSourceCompiles)

set(CMAKE_C_FLAGS "${BUILD_OPTION}")

check_include_files(iconv.h HAVE_ICONV_H)

check_c_source_compiles("void posix_spawnp();
	__asm__(\".symver posix_spawnp,posix_spawnp@GLIBC_2.2.5\");
	void main(void){posix_spawnp();}" HAVE_GLIB_C_225_H)

check_c_source_compiles("void posix_spawnp();
	__asm__(\".symver posix_spawnp,posix_spawnp@GLIBC_2.2\");
	void main(void){posix_spawnp();}" HAVE_GLIB_C_22_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.2.5\");
	void main(void){fcntl();}" HAVE_GLIB_C_FCNTL_225_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.0\");
	void main(void){fcntl();}" HAVE_GLIB_C_FCNTL_2_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.4\");
	void main(void){fcntl();}" HAVE_GLIB_C_FCNTL_24_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.17\");
	void main(void){fcntl();}" HAVE_GLIB_C_FCNTL_217_H)

configure_file(${FIBJS_ROOT_DIR}/fibjs/tools/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(${FIBJS_ROOT_DIR}/fibjs/tools/gitinfo.h.in ${CMAKE_CURRENT_BINARY_DIR}/gitinfo.h)

set(flags "-fsigned-char -fmessage-length=0 -fdata-sections -ffunction-sections -D_FILE_OFFSET_BITS=64")
set(ccflags "${ccflags} ${CXX_STD_VER_FLAG}")

if(${OS} STREQUAL "Darwin")
	set(flags "${flags} -mmacosx-version-min=10.9")
endif()

if(${BUILD_TYPE} STREQUAL "release")
	set(flags "${flags} -O3 -s ${BUILD_OPTION} -w -fvisibility=hidden")

	if(${OS} STREQUAL "FreeBSD")
		set(flags "${flags} -fno-omit-frame-pointer")
	else()
		set(flags "${flags} -fomit-frame-pointer")
	endif()

	add_definitions(-DNDEBUG=1)
endif()

if(${BUILD_TYPE} STREQUAL "debug")
	set(flags "${flags} -g -O0 ${BUILD_OPTION} -Wall -Wno-overloaded-virtual")
	add_definitions(-DDEBUG=1)
endif()

set(CMAKE_C_FLAGS "${flags}")
set(CMAKE_CXX_FLAGS "${flags} ${ccflags}")

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${FIBJS_ROOT_DIR}/fibjs/include"
	"${FIBJS_ROOT_DIR}/vender"
	"${FIBJS_ROOT_DIR}/vender/v8"
	"${FIBJS_ROOT_DIR}/vender/v8/include"
	"${FIBJS_ROOT_DIR}/vender/mbedtls"
	"${FIBJS_ROOT_DIR}/vender/zlib/include"
	"${FIBJS_ROOT_DIR}/vender/msgpack/include"
	"${CMAKE_CURRENT_BINARY_DIR}"
)
