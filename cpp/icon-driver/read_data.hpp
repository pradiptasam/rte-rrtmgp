#ifndef READ_DATA_HPP
#define READ_DATA_HPP

#include <netcdf>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <Kokkos_Core.hpp>

using namespace netCDF;
using namespace netCDF::exceptions;
using real   = double;
using LayoutT = Kokkos::LayoutRight; // or LayoutLeft
using DeviceT = Kokkos::DefaultExecutionSpace; // or another device type
using real1d_t = Kokkos::View<real*,   LayoutT, DeviceT>;
using real2d_t = Kokkos::View<real**,  LayoutT, DeviceT>;
using real3d_t = Kokkos::View<real***, LayoutT, DeviceT>;

//---------------------------------------------------------------------
// Define a structure to hold the input arrays and scalar values
//---------------------------------------------------------------------
struct InputData {
    int ncol;
    int klev;   // In the Python code, klev comes from dimension "nlay"

    // 2D variables (dimensions: ncol x klev)
    real2d_t dz;
    real2d_t zh;
    real2d_t zf;
    real2d_t pp_hl;
    real2d_t pp_fl;
    real2d_t tk_hl;
    real2d_t tk_fl;
    real2d_t xvmr_vap;
    real2d_t xm_liq;
    real2d_t xm_ice;
    real2d_t xm_snw;
    real2d_t cdnc;
    real2d_t cld_frc;
    real2d_t xvmr_co2;
    real2d_t xvmr_ch4;
    real2d_t xvmr_n2o;
    real2d_t xvmr_o3;
    real2d_t xvmr_o2;
    real2d_t reff_ice;
    real2d_t reff_snow;

    // 1D variables (length: ncol)
    real1d_t pp_sfc;
    real1d_t tk_sfc;
    real1d_t pcos_mu0;
    real1d_t daylight_frc;
    real1d_t alb_vis_dir;
    real1d_t alb_nir_dir;
    real1d_t alb_vis_dif;
    real1d_t alb_nir_dif;
    real1d_t emissivity;
    real1d_t laland;  // land-sea mask
    real1d_t laglac;  // glacier mask

    // 3D variable (dimensions: ncol x klev x 2)
    real3d_t xvmr_cfc;
};

// A helper function to read a 2D variable from the netCDF file.
// It reads the variable as a flat vector and reshapes it to a 2D vector.
// std::vector<std::vector<double>> read2DVar(NcFile &file,
//                                             const std::string &varName,
//                                             int ncol, int klev);
real2d_t read2DVar(netCDF::NcFile &file,
                   const std::string &varName,
                   int ncol, int klev);
//---------------------------------------------------------------------
// A helper function to read a 1D variable from the netCDF file.
// std::vector<double> read1DVar(NcFile &file,
//                               const std::string &varName,
//                               int ncol);
real1d_t read1DVar(netCDF::NcFile &file,
                   const std::string &varName,
                   int ncol);
//---------------------------------------------------------------------
// A helper function to read a 3D variable from the netCDF file.
// Here we assume the third dimension is known (e.g. size 2 for xvmr_cfc).
// std::vector<std::vector<std::vector<double>>>
// read3DVar(NcFile &file,
//           const std::string &varName,
//           int ncol, int klev, int thirdDimSize);
real3d_t read3DVar(netCDF::NcFile &file,
                   const std::string &varName,
                   int ncol, int klev, int thirdDimSize);
//---------------------------------------------------------------------
// The read_netcdf_to_dict function using the netCDF C++ API.
// It opens the file, reads dimensions, then reads each variable into
// the InputData structure.
//---------------------------------------------------------------------
InputData read_netcdf_to_dict(const std::string &nc_file);

#endif
