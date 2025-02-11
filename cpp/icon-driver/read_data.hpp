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

//---------------------------------------------------------------------
// A helper function to read a 2D variable from the netCDF file.
// It reads the variable as a flat vector and reshapes it to a 2D vector.
std::vector<std::vector<double>> read2DVar(NcFile &file,
                                            const std::string &varName,
                                            int ncol, int klev)
{
    NcVar var = file.getVar(varName);
    if(var.isNull()) {
        throw std::runtime_error("Variable " + varName + " not found in the file.");
    }
    std::vector<double> flat(ncol * klev);
    var.getVar(flat.data());
    
    std::vector<std::vector<double>> result(ncol, std::vector<double>(klev, 0.0));
    for (int i = 0; i < ncol; ++i) {
        for (int j = 0; j < klev; ++j) {
            result[i][j] = flat[i * klev + j];
        }
    }
    return result;
}

//---------------------------------------------------------------------
// A helper function to read a 1D variable from the netCDF file.
std::vector<double> read1DVar(NcFile &file,
                              const std::string &varName,
                              int ncol)
{
    NcVar var = file.getVar(varName);
    if(var.isNull()) {
        throw std::runtime_error("Variable " + varName + " not found in the file.");
    }
    std::vector<double> vec(ncol);
    var.getVar(vec.data());
    return vec;
}

//---------------------------------------------------------------------
// A helper function to read a 3D variable from the netCDF file.
// Here we assume the third dimension is known (e.g. size 2 for xvmr_cfc).
std::vector<std::vector<std::vector<double>>>
read3DVar(NcFile &file,
          const std::string &varName,
          int ncol, int klev, int thirdDimSize)
{
    NcVar var = file.getVar(varName);
    if(var.isNull()) {
        throw std::runtime_error("Variable " + varName + " not found in the file.");
    }
    std::vector<double> flat(ncol * klev * thirdDimSize);
    var.getVar(flat.data());
    
    std::vector<std::vector<std::vector<double>>> result(
        ncol, std::vector<std::vector<double>>(klev, std::vector<double>(thirdDimSize, 0.0)));
    
    for (int i = 0; i < ncol; ++i) {
        for (int j = 0; j < klev; ++j) {
            for (int k = 0; k < thirdDimSize; ++k) {
                result[i][j][k] = flat[i * klev * thirdDimSize + j * thirdDimSize + k];
            }
        }
    }
    return result;
}

//---------------------------------------------------------------------
// The read_netcdf_to_dict function using the netCDF C++ API.
// It opens the file, reads dimensions, then reads each variable into
// the InputData structure.
//---------------------------------------------------------------------
InputData read_netcdf_to_dict(const std::string &nc_file)
{
    InputData data;
    try {
        // Open the netCDF file in read-only mode
        NcFile file(nc_file, NcFile::read);
        // print the name of the file
        std::cout << "File name: " << nc_file << std::endl;

        // Get dimensions
        NcDim ncolDim = file.getDim("ncol");
        NcDim nlayDim = file.getDim("nlay");  // "nlay" corresponds to klev in our code
        data.ncol = ncolDim.getSize();
        data.klev = nlayDim.getSize();
        printf("ncol: %d, klev: %d\n", data.ncol, data.klev);

        // Read 2D variables
        data.dz         = read2DVar(file, "dz", data.ncol, data.klev);
        data.zh         = read2DVar(file, "zh", data.ncol, data.klev+1);
        data.zf         = read2DVar(file, "zf", data.ncol, data.klev);
        data.pp_hl      = read2DVar(file, "pp_hl", data.ncol, data.klev+1);
        data.pp_fl      = read2DVar(file, "pp_fl", data.ncol, data.klev);
        data.tk_hl      = read2DVar(file, "tk_hl", data.ncol, data.klev+1);
        data.tk_fl      = read2DVar(file, "tk_fl", data.ncol, data.klev);
        data.xvmr_vap   = read2DVar(file, "xvmr_vap", data.ncol, data.klev);
        data.xm_liq     = read2DVar(file, "xm_liq", data.ncol, data.klev);
        data.xm_ice     = read2DVar(file, "xm_ice", data.ncol, data.klev);
        data.xm_snw     = read2DVar(file, "xm_snw", data.ncol, data.klev);
        data.cdnc       = read2DVar(file, "cdnc", data.ncol, data.klev);
        data.cld_frc    = read2DVar(file, "cld_frc", data.ncol, data.klev);
        data.reff_ice   = read2DVar(file, "reff_ice", data.ncol, data.klev);
        data.reff_snow  = read2DVar(file, "reff_snow", data.ncol, data.klev);

        // Read 1D variables
        data.pp_sfc       = read1DVar(file, "pp_sfc", data.ncol);
        data.tk_sfc       = read1DVar(file, "tk_sfc", data.ncol);
        data.pcos_mu0     = read1DVar(file, "pcos_mu0", data.ncol);
        data.daylight_frc = read1DVar(file, "daylght_frc", data.ncol);
        data.alb_vis_dir  = read1DVar(file, "alb_vis_dir", data.ncol);
        data.alb_nir_dir  = read1DVar(file, "alb_nir_dir", data.ncol);
        data.alb_vis_dif  = read1DVar(file, "alb_vis_dif", data.ncol);
        data.alb_nir_dif  = read1DVar(file, "alb_nir_dif", data.ncol);
        data.emissivity   = read1DVar(file, "emissivity", data.ncol);

        // Read optional 1D variables (land and glacier masks)
        try {
            data.laland = read1DVar(file, "laland", data.ncol);
        } catch (std::runtime_error &e) {
            // if not found, fill with zeros
            data.laland = std::vector<double>(data.ncol, 0.0);
        }
        try {
            data.laglac = read1DVar(file, "laglac", data.ncol);
        } catch (std::runtime_error &e) {
            data.laglac = std::vector<double>(data.ncol, 0.0);
        }

        // Read additional gas mixing ratios (assumed 2D)
        data.xvmr_co2 = read2DVar(file, "xvmr_co2", data.ncol, data.klev);
        data.xvmr_ch4 = read2DVar(file, "xvmr_ch4", data.ncol, data.klev);
        data.xvmr_n2o = read2DVar(file, "xvmr_n2o", data.ncol, data.klev);
        data.xvmr_o3  = read2DVar(file, "xvmr_o3", data.ncol, data.klev);
        data.xvmr_o2  = read2DVar(file, "xvmr_o2", data.ncol, data.klev);

        // Read the 3D variable xvmr_cfc (with third dimension size = 2)
        data.xvmr_cfc = read3DVar(file, "xvmr_cfc", data.ncol, data.klev, 2);
    }
    catch (NcException &e) {
        std::cerr << "NetCDF error: " << e.what() << std::endl;
        throw;
    }
    return data;
}
