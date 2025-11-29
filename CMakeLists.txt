# JanumachineTerrainFormat/CMakeList.txt

cmake_minimum_required (VERSION 3.21)

project (
	JanumachineTerrainFormat
	VERSION 1.0.0
	DESCRIPTION "Janumachine Terrain Format Library"
	LANGUAGES C CXX
)

# version defines
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# ensure consitency for multi-config generators
foreach(OUTPUTCONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin)
endforeach()

# set common compiler settings (warnings, flags)
if(MSVC)
	add_compile_options(/W4 /permissive- /Zc:preprocessor /EHsc)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# set default build type for single-config generators
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build type." FORCE)
endif()

# add subprojects
add_subdirectory(jtf)
add_subdirectory(jtf_testing)