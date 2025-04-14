# cmake/FindnetCDF.cmake
find_path(netCDF_INCLUDE_DIR netcdf.h
  HINTS
    ENV NETCDF_DIR
  PATH_SUFFIXES include
)

find_library(netCDF_LIBRARY
  NAMES netcdf
  HINTS
    ENV NETCDF_DIR
  PATH_SUFFIXES lib lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(netCDF
  REQUIRED_VARS netCDF_LIBRARY netCDF_INCLUDE_DIR
)

if(netCDF_FOUND)
  set(netCDF_LIBRARIES ${netCDF_LIBRARY})
  set(netCDF_INCLUDE_DIRS ${netCDF_INCLUDE_DIR})
endif()

mark_as_advanced(netCDF_INCLUDE_DIR netCDF_LIBRARY)

