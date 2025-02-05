cmake_minimum_required(VERSION 3.10)
project(optical_props_pybind)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)
# Also find Kokkos and any other dependencies here:
# find_package(Kokkos REQUIRED)

find_library(NETCDF_LIBRARY NAMES netcdf PATH /sw/spack-levante/netcdf-c-4.9.2-ytsqvg/lib)
find_path(NETCDF_INCLUDE_DIR NAMES netcdf.h PATH /sw/spack-levante/netcdf-c-4.9.2-ytsqvg/include)

set(TARGET_NAME optical_props_pybind)
set(SOURCES ${CMAKE_SOURCE_DIR}/optical_props_pybind.cpp
            ${CMAKE_SOURCE_DIR}/gas_concs_pybind.cpp
            ${CMAKE_SOURCE_DIR}/module.cpp)

pybind11_add_module(${TARGET_NAME} ${SOURCES})
# add_library(${TARGET_NAME} MODULE ${SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ../rte)
target_include_directories(${TARGET_NAME} PUBLIC ../rte/kernels)
target_include_directories(${TARGET_NAME} PUBLIC ../rrtmgp)
target_include_directories(${TARGET_NAME} PUBLIC ../rrtmgp/kernels)
target_include_directories(${TARGET_NAME} PUBLIC ../)
# Link with Kokkos and other necessary libraries.
set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}" 
                                                # OUTPUT_NAME "optical_props"
                                                OUTPUT_NAME "rte_rrtmgp"
                                                SUFFIX ".so")
                                                # SUFFIX "${PYTHON_MODULE_EXTENSION}")

if (NETCDF_LIBRARY AND NETCDF_INCLUDE_DIR)
  target_include_directories(${TARGET_NAME} PUBLIC ${NETCDF_INCLUDE_DIR})
else()
  message(WARNING "NetCDF not found; allsky will not be built")
  return() # Skip the rest of the file
endif()

if (RRTMGP_ENABLE_YAKL)
  include(${YAKL_HOME}/yakl_utils.cmake)
  yakl_process_target(${TARGET_NAME})
  target_compile_definitions(${TARGET_NAME} PUBLIC RRTMGP_ENABLE_YAKL)
  target_link_libraries(${TARGET_NAME} PRIVATE pybind11::module "${CMAKE_CURRENT_BINARY_DIR}/../../build/librrtmgp.a" "${CXX_LINK}" "${NETCDF_LIBRARY}")
endif()
if (RRTMGP_ENABLE_KOKKOS)
  find_package(Kokkos REQUIRED)
  # target_link_libraries(optical_props_pybind Kokkos::kokkos)
  target_compile_definitions(${TARGET_NAME} PUBLIC RRTMGP_ENABLE_KOKKOS)
  target_link_libraries(${TARGET_NAME} PRIVATE pybind11::module "${CMAKE_CURRENT_BINARY_DIR}/../../build/librrtmgp.a" "${CXX_LINK}" "${NETCDF_LIBRARY}" Kokkos::kokkos)
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../../python)
