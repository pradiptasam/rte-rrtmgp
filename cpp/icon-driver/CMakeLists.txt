cmake_minimum_required(VERSION 3.0)
project(standalone C CXX)

# find_library(NETCDF_LIBRARY NAMES netcdf PATHS /sw/spack-levante/netcdf-c-4.9.2-ytsqvg/lib)
# find_path(NETCDF_INCLUDE_DIR NAMES netcdf.h PATHS /sw/spack-levante/netcdf-c-4.9.2-ytsqvg/include)

# set(CMAKE_BUILD_TYPE "RelWithDebInfo")

# set(CMAKE_CXX_FLAGS "-std=gnu++17 -O2 -g -march=native -mtune=native" CACHE STRING "RelWithDebInfo C++ flags" FORCE)

# find_library(NETCDF_LIBRARY NAMES netcdf_c++4 PATHS /sw/spack-levante/netcdf-cxx4-4.3.1-sbk7ks/lib)
# find_path(NETCDF_INCLUDE_DIR NAMES netcdf PATHS /sw/spack-levante/netcdf-cxx4-4.3.1-sbk7ks/include)

find_library(netCDFCXX_LIBRARIES NAMES netcdf-cxx4 PATHS /work/k20200/k202170/spack-sw/netcdf-cxx4-4.3.1-b7fmi4/lib64)
find_path(netCDFCXX_INCLUDE_DIR NAMES netcdf PATHS /work/k20200/k202170/spack-sw/netcdf-cxx4-4.3.1-b7fmi4/include)
#
# set(NetCDF_DIR /work/k20200/k202170/spack-sw/netcdf-c-4.9.2-rp3zpy/lib64/cmake/netCDF)

find_package(netCDF REQUIRED)
if(NOT netCDF_FOUND)
  message(FATAL_ERROR "netCDF-C not found!")
endif()
message(STATUS "netCDF-C include dirs: ${netCDF_INCLUDE_DIR}")
message(STATUS "netCDF-C libraries: ${netCDF_LIBRARIES}")

# Find the netCDF-C++4 library.
# find_package(netCDFCxx REQUIRED)
# if(NOT netCDFCxx_FOUND)
#   message(FATAL_ERROR "NetCDF-C++4 not found!")
# endif()
# message(STATUS "NetCDF-C++4 include dirs: ${netCDFCxx_INCLUDE_DIRS}")
# message(STATUS "NetCDF-C++4 libraries: ${netCDFCxx_LIBRARIES}")

set(TARGET_NAME icon_radiation)
set(
    SOURCES ${CMAKE_SOURCE_DIR}/main.cpp
   )

add_library(${TARGET_NAME} MODULE ${SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ../rte)
target_include_directories(${TARGET_NAME} PUBLIC ../rte/kernels)
target_include_directories(${TARGET_NAME} PUBLIC ../rrtmgp)
target_include_directories(${TARGET_NAME} PUBLIC ../rrtmgp/kernels)
target_include_directories(${TARGET_NAME} PUBLIC ../)

message(STATUS "NETCDF_INCLUDE_DIR: ${NETCDF_INCLUDE_DIR}")
if (netCDFCXX_LIBRARIES AND netCDFCXX_INCLUDE_DIR)
  target_include_directories(${TARGET_NAME} PUBLIC ${netCDF_INCLUDE_DIR})
  target_include_directories(${TARGET_NAME} PUBLIC ${netCDFCXX_INCLUDE_DIR})
else()
  message(WARNING "NetCDFCXX not found; allsky will not be built")
  return() # Skip the rest of the file
endif()

if (RRTMGP_ENABLE_YAKL)
  include(${YAKL_HOME}/yakl_utils.cmake)
  yakl_process_target(${TARGET_NAME})
  target_compile_definitions(${TARGET_NAME} PUBLIC RRTMGP_ENABLE_YAKL)
  target_link_libraries(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../../build/librrtmgp.a" "${CXX_LINK}" "${NETCDF_LIBRARY}")
endif()
if (RRTMGP_ENABLE_KOKKOS)
  find_package(Kokkos REQUIRED)
  # target_link_libraries(optical_props_pybind Kokkos::kokkos)
  target_compile_definitions(${TARGET_NAME} PUBLIC RRTMGP_ENABLE_KOKKOS)
  target_link_libraries(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../../build/librrtmgp.a" "${CXX_LINK}" "${NETCDF_LIBRARIES}" Kokkos::kokkos)
endif()

# Remove the unwanted architecture flags from CMAKE_CXX_FLAGS
if(CMAKE_CXX_FLAGS)
  message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
  string(REPLACE "-march=znver3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REPLACE "-mtune=znver3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  message(STATUS "CMAKE_CXX_FLAGS is not set")
endif()
