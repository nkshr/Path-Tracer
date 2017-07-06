cmake_minimum_required(VERSION 2.8.4)
project(pathtracer)
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
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
    src/kdtree.cpp
    src/texture.cpp
    src/camera.cpp
    src/scene.cpp
    src/renderer.cpp
	src/spectrum.cpp
	src/attenuation.cpp
	src/cie_xyz.cpp
    lib/rand48/rand48.c
    lib/lodepng/lodepng.cpp
    lib/tiny_obj_loader/tiny_obj_loader.cc)

set(HEADER_FILES
	src/aabbox.h
	src/camera.h
	src/kdtree.h
	src/material.h
	src/objects.h
	src/ray.h
	src/renderer.h
	src/scene.h
	src/texture.h
	src/triangle.h
	src/vector.h
	src/spectrum.h
	src/attenuation.h
	src/cie_xyz.h
	src/common.h
	lib/rand48/erand48.h
	lib/lodepng/lodepng.h
	lib/tiny_obj_loader/tiny_obj_loader.h
	)

set(DATA
	data/Pope_absorp.txt
	)

add_executable(pathtracer src/main.cpp ${SOURCE_FILES} ${HEADER_FILES} ${DATA})
add_executable(attenuation_test src/attenuation_test.cpp ${SOURCE_FILES} ${HEADER_FILES} ${DATA})
add_executable(cie_xyz_test src/cie_xyz_test.cpp ${SOURCE_FILES} ${HEADER_FILES} ${DATA})

enable_testing()

add_test(NAME attenuation_test
	COMMAND attenuation_test.exe
	 [WORKING_DIRECTORY build])