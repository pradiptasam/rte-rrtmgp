#ifndef READ_DATA_HPP
#define READ_DATA_HPP

#include <netcdf>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

using namespace netCDF;
using namespace netCDF::exceptions;

//---------------------------------------------------------------------
// Define a structure to hold the input arrays and scalar values
//---------------------------------------------------------------------
struct InputData {
    int ncol;
    int klev;   // In the Python code, klev comes from dimension "nlay"

    // 2D variables (dimensions: ncol x klev)
    std::vector<std::vector<double>> dz;
    std::vector<std::vector<double>> zh;
    std::vector<std::vector<double>> zf;
    std::vector<std::vector<double>> pp_hl;
    std::vector<std::vector<double>> pp_fl;
    std::vector<std::vector<double>> tk_hl;
    std::vector<std::vector<double>> tk_fl;
    std::vector<std::vector<double>> xvmr_vap;
    std::vector<std::vector<double>> xm_liq;
    std::vector<std::vector<double>> xm_ice;
    std::vector<std::vector<double>> xm_snw;
    std::vector<std::vector<double>> cdnc;
    std::vector<std::vector<double>> cld_frc;
    std::vector<std::vector<double>> xvmr_co2;
    std::vector<std::vector<double>> xvmr_ch4;
    std::vector<std::vector<double>> xvmr_n2o;
    std::vector<std::vector<double>> xvmr_o3;
    std::vector<std::vector<double>> xvmr_o2;
    std::vector<std::vector<double>> reff_ice;
    std::vector<std::vector<double>> reff_snow;

    // 1D variables (length: ncol)
    std::vector<double> pp_sfc;
    std::vector<double> tk_sfc;
    std::vector<double> pcos_mu0;
    std::vector<double> daylight_frc;
    std::vector<double> alb_vis_dir;
    std::vector<double> alb_nir_dir;
    std::vector<double> alb_vis_dif;
    std::vector<double> alb_nir_dif;
    std::vector<double> emissivity;
    std::vector<double> laland;  // land sea mask
    std::vector<double> laglac;  // glacier mask

    // 3D variable (dimensions: ncol x klev x 2)
    std::vector<std::vector<std::vector<double>>> xvmr_cfc;
};

// A helper function to read a 2D variable from the netCDF file.
// It reads the variable as a flat vector and reshapes it to a 2D vector.
std::vector<std::vector<double>> read2DVar(NcFile &file,
                                            const std::string &varName,
                                            int ncol, int klev);
//---------------------------------------------------------------------
// A helper function to read a 1D variable from the netCDF file.
std::vector<double> read1DVar(NcFile &file,
                              const std::string &varName,
                              int ncol);
//---------------------------------------------------------------------
// A helper function to read a 3D variable from the netCDF file.
// Here we assume the third dimension is known (e.g. size 2 for xvmr_cfc).
std::vector<std::vector<std::vector<double>>>
read3DVar(NcFile &file,
          const std::string &varName,
          int ncol, int klev, int thirdDimSize);
//---------------------------------------------------------------------
// The read_netcdf_to_dict function using the netCDF C++ API.
// It opens the file, reads dimensions, then reads each variable into
// the InputData structure.
//---------------------------------------------------------------------
InputData read_netcdf_to_dict(const std::string &nc_file);

#endif
