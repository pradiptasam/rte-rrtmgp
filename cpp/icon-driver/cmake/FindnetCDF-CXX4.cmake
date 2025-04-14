# cmake/FindnetCDF-CXX4.cmake
find_path(netCDF-CXX4_INCLUDE_DIR netcdf
  HINTS
    ENV NETCDFCXX_DIR
  PATH_SUFFIXES include
)

find_library(netCDF-CXX4_LIBRARY
  NAMES netcdf-cxx4
  HINTS
    ENV NETCDFCXX_DIR
  PATH_SUFFIXES lib lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(netCDF-CXX4
  REQUIRED_VARS netCDF-CXX4_LIBRARY netCDF-CXX4_INCLUDE_DIR
)

if(netCDF-CXX4_FOUND)
  set(netCDF-CXX4_LIBRARIES ${netCDF-CXX4_LIBRARY})
  set(netCDF-CXX4_INCLUDE_DIRS ${netCDF-CXX4_INCLUDE_DIR})
endif()

mark_as_advanced(netCDF-CXX4_INCLUDE_DIR netCDF-CXX4_LIBRARY)
