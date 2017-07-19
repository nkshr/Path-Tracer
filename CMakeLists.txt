cmake_minimum_required(VERSION 2.8.4)

cmake_policy(SET CMP0054 NEW)

project(pathtracer)

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	message("OS IS LINUX")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-narrowing -march=native -m64 -O3 -static -funroll-loops")
endif()

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
	message("OPENMP FOUND")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(SOURCE_FILES
    src/material.cpp
    src/objects.cpp
    src/camera.cpp
    src/scene.cpp
    src/renderer.cpp
	src/attenuation.cpp
	src/spectrum.cpp
	lib/rand48/rand48.c
    lib/lodepng/lodepng.cpp
    lib/tiny_obj_loader/tiny_obj_loader.cc
	)

set(HEADER_FILES
	src/camera.h
	src/material.h
	src/objects.h
	src/ray.h
	src/renderer.h
	src/scene.h
	src/vector.h
	src/attenuation.h
	src/spectrum.h
	src/common.h
	lib/rand48/erand48.h
	lib/lodepng/lodepng.h
	lib/tiny_obj_loader/tiny_obj_loader.h
	)

set(DATA
	data/Pope_absorp_coefs.csv
	data/ciexyz31.csv
	)

add_executable(pathtracer src/main.cpp ${SOURCE_FILES} ${HEADER_FILES} ${DATA})
#add_executable(spectrum_template_generator src/spectrum_template_generator.cpp)

add_test(NAME attenuation_test
	COMMAND attenuation_test.exe
	 [WORKING_DIRECTORY build])
