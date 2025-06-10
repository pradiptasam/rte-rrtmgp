cmake_minimum_required(VERSION 3.0)
project(standalone C CXX Fortran)

# find_library(NETCDF_LIBRARY NAMES netcdf PATHS /sw/spack-levante/netcdf-c-4.9.2-ytsqvg/lib)
# find_path(NETCDF_INCLUDE_DIR NAMES netcdf.h PATHS /sw/spack-levante/netcdf-c-4.9.2-ytsqvg/include)

set(CMAKE_BUILD_TYPE "DEBUG")

# stop the build if YAKL is enabled
if (RRTMGP_ENABLE_YAKL)
  message(FATAL_ERROR "RRTMGP: YAKL is not supported in this build")
endif()

# set(CMAKE_CXX_FLAGS "-std=gnu++17 -O2 -g -march=native -mtune=native" CACHE STRING "RelWithDebInfo C++ flags" FORCE)

# find_library(NETCDF_LIBRARY NAMES netcdf_c++4 PATHS /sw/spack-levante/netcdf-cxx4-4.3.1-sbk7ks/lib)
# find_path(NETCDF_INCLUDE_DIR NAMES netcdf PATHS /sw/spack-levante/netcdf-cxx4-4.3.1-sbk7ks/include)

# find_library(netCDFCXX_LIBRARIES NAMES netcdf-cxx4 PATHS /work/k20200/k202170/spack-sw/netcdf-cxx4-4.3.1-b7fmi4/lib64)
# find_path(netCDFCXX_INCLUDE_DIR NAMES netcdf PATHS /work/k20200/k202170/spack-sw/netcdf-cxx4-4.3.1-b7fmi4/include)
#
# set(NetCDF_DIR /work/k20200/k202170/spack-sw/netcdf-c-4.9.2-rp3zpy/lib64/cmake/netCDF)
# find_library(netCDF_LIBRARY NAMES netcdf PATHS /work/k20200/k202170/spack-sw/netcdf-c-4.9.2-rp3zpy/lib64)
# set(netCDF_DIR /work/k20200/k202170/spack-sw/netcdf-c-4.9.2-rp3zpy/lib64/cmake/netCDF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(netCDF REQUIRED)
if(NOT netCDF_FOUND)
  message(FATAL_ERROR "netCDF-C not found!")
endif()
message(STATUS "netCDF-C include dirs: ${netCDF_INCLUDE_DIR}")
message(STATUS "netCDF-C libraries: ${netCDF_LIBRARY}")

# Find NetCDF-C++4
find_package(netCDF-CXX4 REQUIRED)
if(NOT netCDF-CXX4_FOUND)
  message(FATAL_ERROR "NetCDF-C++4 not found!")
endif()
message(STATUS "NetCDF-C++4 include dirs: ${netCDF-CXX4_INCLUDE_DIRS}")
message(STATUS "NetCDF-C++4 libraries: ${netCDF-CXX4_LIBRARIES}")

# Find the netCDF-C++4 library.
# find_package(netCDFCxx REQUIRED)
# if(NOT netCDFCxx_FOUND)
#   message(FATAL_ERROR "NetCDF-C++4 not found!")
# endif()
# message(STATUS "NetCDF-C++4 include dirs: ${netCDFCxx_INCLUDE_DIRS}")
# message(STATUS "NetCDF-C++4 libraries: ${netCDFCxx_LIBRARIES}")

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

# message(STATUS "NETCDF_INCLUDE_DIR: ${netCDF_INCLUDE_DIR}")
# if (netCDF-CXX4_LIBRARIES AND netCDF-CXX4_INCLUDE_DIRS AND netCDF_LIBRARY)
#   target_include_directories(${TARGET_NAME} PUBLIC ${netCDF_INCLUDE_DIR})
#   target_include_directories(${TARGET_NAME} PUBLIC ${netCDF-CXX4_INCLUDE_DIRS})
# else()
#   message(WARNING "NetCDFCXX not found; allsky will not be built")
#   return() # Skip the rest of the file
# endif()

# Define common include directories
set(COMMON_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/rte
    ${CMAKE_SOURCE_DIR}/rte/kernels
    ${CMAKE_SOURCE_DIR}/rrtmgp
    ${CMAKE_SOURCE_DIR}/rrtmgp/kernels
    ${CMAKE_SOURCE_DIR}/extensions/cloud_optics
    ${CMAKE_SOURCE_DIR}/examples
    ${CMAKE_SOURCE_DIR}
)

# Create the library from rte_rrtmgp_interface.cpp
set(LIB_NAME rrtmgp_interface)
add_library(${LIB_NAME} STATIC ${CMAKE_CURRENT_SOURCE_DIR}/rte_rrtmgp_interface.cpp)
set_target_properties(
  ${LIB_NAME}
  PROPERTIES  C_STANDARD 99
              C_STANDARD_REQUIRED ON)

target_include_directories(
  ${LIB_NAME}
  PUBLIC
    ${COMMON_INCLUDE_DIRS}
    "${netCDF_INCLUDE_DIR}"
    "${netCDF-CXX4_INCLUDE_DIRS}"
  INTERFACE
    $<BUILD_INTERFACE:$<$<COMPILE_LANGUAGE:C,CXX>:${CMAKE_CURRENT_SOURCE_DIR}>>
  PRIVATE
    $<BUILD_INTERFACE:$<$<COMPILE_LANGUAGE:C,CXX>:${CMAKE_CURRENT_BINARY_DIR}>>)


# set(TARGET_NAME icon_radiation)
# set(
#     SOURCES
#     ${CMAKE_CURRENT_SOURCE_DIR}/read_data.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/rte_rrtmgp_interface.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
#    )

if (RRTMGP_ENABLE_KOKKOS)
  target_compile_definitions(${LIB_NAME} PUBLIC RRTMGP_ENABLE_KOKKOS)
  target_link_libraries(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/../../build/librrtmgp.a" 
                                          "${CXX_LINK}"
                                          "${netCDF_LIBRARY}"
                                          "${netCDF-CXX4_LIBRARIES}"
                                          Kokkos::kokkos)
endif()

set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/modules)

set(FORTRAN_INTERFACE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/rte_rrtmgp_interface.f90)
set(C_WRAPPER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/rte_rrtmgp_wrapper.cpp)

set(INTERFACE_LIB_NAME rrtmgp_fortran_interface)
add_library(${INTERFACE_LIB_NAME} STATIC 
    ${C_WRAPPER_SOURCE} 
    ${FORTRAN_INTERFACE_SOURCE}
)

set_target_properties(
  ${INTERFACE_LIB_NAME}
  PROPERTIES
    Fortran_MODULE_DIRECTORY ${CMAKE_Fortran_MODULE_DIRECTORY}
)

target_include_directories(
  ${INTERFACE_LIB_NAME}
  PUBLIC
  $<BUILD_INTERFACE:$<$<COMPILE_LANGUAGE:Fortran>:${CMAKE_Fortran_MODULE_DIRECTORY}>>
    ${COMMON_INCLUDE_DIRS}
    "${netCDF_INCLUDE_DIR}"
    "${netCDF-CXX4_INCLUDE_DIRS}"
)

target_link_libraries(${INTERFACE_LIB_NAME} PUBLIC ${LIB_NAME})

target_compile_definitions(${INTERFACE_LIB_NAME} PUBLIC RRTMGP_ENABLE_KOKKOS)
target_link_libraries(${INTERFACE_LIB_NAME} PUBLIC Kokkos::kokkos)

# Create the executable from main.cpp and read_data.cpp
set(TARGET_NAME icon_radiation)
set(EXEC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/read_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(${TARGET_NAME} ${EXEC_SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ${COMMON_INCLUDE_DIRS})
target_include_directories(${TARGET_NAME} PUBLIC ${netCDF_INCLUDE_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${netCDF-CXX4_INCLUDE_DIRS})

# Link the executable with the library
target_link_libraries(${TARGET_NAME} PRIVATE ${LIB_NAME})

# Configure the executable with Kokkos if needed
if (RRTMGP_ENABLE_KOKKOS)
  target_compile_definitions(${TARGET_NAME} PUBLIC RRTMGP_ENABLE_KOKKOS)
endif()

# add_executable(${TARGET_NAME} ${SOURCES})
# # add_library(${TARGET_NAME} MODULE ${SOURCES})
# target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/rte)
# target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/rte/kernels)
# target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/rrtmgp)
# target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/rrtmgp/kernels)
# target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/extensions/cloud_optics)
# target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/examples)
# target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR})


# if (RRTMGP_ENABLE_KOKKOS)
#   # find_package(Kokkos REQUIRED)
#   # target_link_libraries(${TARGET_NAME} Kokkos::kokkos)
#   target_compile_definitions(${TARGET_NAME} PUBLIC RRTMGP_ENABLE_KOKKOS)
#   message(STATUS "netCDF-C libraries: ${netCDF_LIBRARY}")
#   target_link_libraries(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../../build/librrtmgp.a" "${CXX_LINK}" "${netCDF_LIBRARY}" "${netCDF-CXX4_LIBRARIES}" Kokkos::kokkos)
# endif()
