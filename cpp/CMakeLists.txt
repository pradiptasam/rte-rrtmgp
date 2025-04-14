###############################################################################
## This assumes you have already run add_subdirectory(${YAKL_HOME} ${YAKL_BIN})
## It also assumes you've enabled CXX and C as languages
###############################################################################
cmake_minimum_required(VERSION 3.27)

# Add RRTMGP source files.
set(CXX_SRC
  rrtmgp/kernels/mo_gas_optics_kernels.cpp
  rrtmgp/mo_rrtmgp_util_reorder.cpp
  rte/expand_and_transpose.cpp
  rte/kernels/mo_fluxes_broadband_kernels.cpp
  rte/kernels/mo_optical_props_kernels.cpp
  rte/kernels/mo_rte_solver_kernels.cpp
)

set (CMAKE_CXX_STANDARD 17)
set(RRTMGP_ENABLE_YAKL FALSE CACHE BOOL "Enable YAKL")
set(RRTMGP_ENABLE_KOKKOS TRUE CACHE BOOL "Enable Kokkos")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(netCDF REQUIRED)
if(NOT netCDF_FOUND)
  message(FATAL_ERROR "netCDF-C not found!")
endif()
message(STATUS "netCDF-C include dirs: ${netCDF_INCLUDE_DIR}")
message(STATUS "netCDF-C libraries: ${netCDF_LIBRARY}")

# find_library(NETCDF_LIBRARY NAMES netcdf PATH /sw/spack-levante/netcdf-c-4.9.2-ytsqvg/lib)
# find_path(NETCDF_INCLUDE_DIR NAMES netcdf.h PATH /sw/spack-levante/netcdf-c-4.9.2-ytsqvg/include)

# Add RRTMGP as a library and make sure we link against YAKL
add_library(rrtmgp STATIC ${CXX_SRC} ${netCDF_LIBRARY})
if (RRTMGP_ENABLE_YAKL)
  include(${YAKL_HOME}/yakl_utils.cmake)
  yakl_process_target(rrtmgp)
  target_include_directories(rrtmgp PUBLIC ${YAKL_HOME})
  message(STATUS "YAKL Flags: ${YAKL_COMPILER_FLAGS}")
endif()

if (RRTMGP_ENABLE_KOKKOS)
  include(FetchContent)
  # configure kokkos 4.4 repository link
  FetchContent_Declare(
    kokkos
    URL https://github.com/kokkos/kokkos/releases/download/4.4.01/kokkos-4.4.01.tar.gz
    URL_HASH MD5=eafd0d42c9831858aa84fde78576644c)

  # disable build of C++23 mdspan experimental support for now
  set(Kokkos_ENABLE_IMPL_MDSPAN OFF CACHE BOOL "Experimental mdspan support")

  # by default, build the Kokkos serial backend for CPU
  set(Kokkos_ENABLE_SERIAL ON CACHE BOOL "Kokkos Serial backend")
  set(Kokkos_ARCH_NATIVE ON CACHE BOOL "Kokkos native architecture optimisations")

  FetchContent_MakeAvailable(kokkos)

  message(STATUS "RRTMGP: Enabling Kokkos")
  # find_package(Kokkos REQUIRED)
  target_link_libraries(rrtmgp Kokkos::kokkos)
  target_compile_definitions(rrtmgp PUBLIC RRTMGP_ENABLE_KOKKOS)
endif()

if (netCDF_LIBRARY AND netCDF_INCLUDE_DIR)
  message(STATUS "RRTMGP: Enabling NetCDF")
  target_include_directories(rrtmgp PUBLIC ${netCDF_INCLUDE_DIR})
else()
  message(WARNING "NetCDF not found; rrtmgp will not be built")
endif()

# Special compile flags for C++ source
target_include_directories(rrtmgp PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(rrtmgp PUBLIC ${CMAKE_SOURCE_DIR}/rte)
target_include_directories(rrtmgp PUBLIC ${CMAKE_SOURCE_DIR}/rte/kernels)
target_include_directories(rrtmgp PUBLIC ${CMAKE_SOURCE_DIR}/rrtmgp)
target_include_directories(rrtmgp PUBLIC ${CMAKE_SOURCE_DIR}/rrtmgp/kernels)

if (RRTMGP_ENABLE_KOKKOS)
  add_subdirectory(icon-driver)
endif()
