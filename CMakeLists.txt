cmake_minimum_required(VERSION 2.8.4)

cmake_policy(SET CMP0054 NEW)

project(pathtracer)

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	message("OS IS LINUX")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-narrowing -march=native -m64 -O3  -funroll-loops")
endif()

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
	message("OPENMP FOUND")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
	add_definitions(/D_CRT_SECURE_NO_WARNINGS)
endif()

set(SOURCE_FILES
	src/attenuation.cpp
	src/color.cpp
	src/common.cpp
	src/geometry.cpp
	src/light.cpp
    src/main.cpp
	src/material.cpp
    src/object.cpp
    src/observer.cpp
    src/scene.cpp
	src/spectrum.cpp
	src/tracer.cpp
	lib/rand48/rand48.c
    lib/lodepng/lodepng.cpp
	)

set(HEADER_FILES
	src/attenuation.h
	src/color.h
	src/common.h
	src/config.h
	src/geometry.h
	src/intersection.h
	src/light.h
	src/material.h
    src/object.h
    src/observer.h
	src/ray.h
    src/scene.h
	src/spectrum.h
	src/tracer.h
	src/triangle.h
	src/vector.h
	lib/rand48/erand48.h
	lib/lodepng/lodepng.h
	#lib/tiny_obj_loader/tiny_obj_loader.h
	)

set(DATA
	data/Pope_absorp_coefs.csv
	data/ciexyz31.csv
	)

add_executable(pathtracer src/main.cpp ${SOURCE_FILES} ${HEADER_FILES} ${DATA})
