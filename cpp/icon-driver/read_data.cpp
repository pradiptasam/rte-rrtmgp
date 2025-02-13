#include "read_data.hpp"

//---------------------------------------------------------------------
// A helper function to read a 1D variable from the netCDF file.
real1d_t read1DVar(netCDF::NcFile &file,
                   const std::string &varName,
                   int ncol)
{
    netCDF::NcVar var = file.getVar(varName);
    if(var.isNull()) {
        throw std::runtime_error("Variable " + varName + " not found in the file.");
    }
    // Read data into a temporary host vector.
    std::vector<real> flat(ncol);
    var.getVar(flat.data());

    // Allocate a 1D Kokkos view.
    real1d_t view("var1d", ncol);
    // Create a host mirror.
    auto host_view = Kokkos::create_mirror_view(view);

    // Copy data from the flat vector into the host view.
    for (int i = 0; i < ncol; ++i) {
        host_view(i) = flat[i];
    }
    // Deep copy the host view to the device view.
    Kokkos::deep_copy(view, host_view);
    return view;
}

//---------------------------------------------------------------------
// A helper function to read a 2D variable from the netCDF file.
// It reads the variable as a flat vector and reshapes it to a 2D vector.
real2d_t read2DVar(netCDF::NcFile &file,
                   const std::string &varName,
                   int ncol, int klev)
{
    netCDF::NcVar var = file.getVar(varName);
    if(var.isNull()) {
        throw std::runtime_error("Variable " + varName + " not found in the file.");
    }
    // Read data into a temporary flat vector.
    std::vector<real> flat(ncol * klev);
    var.getVar(flat.data());

    // Allocate a 2D Kokkos view.
    real2d_t view("var2d", ncol, klev);
    auto host_view = Kokkos::create_mirror_view(view);

    // Fill the host view with the flattened data.
    for (int i = 0; i < ncol; ++i) {
        for (int j = 0; j < klev; ++j) {
            host_view(i,j) = flat[i * klev + j];
        }
    }
    Kokkos::deep_copy(view, host_view);
    return view;
}

//---------------------------------------------------------------------
// A helper function to read a 3D variable from the netCDF file.
// Here we assume the third dimension is known (e.g. size 2 for xvmr_cfc).
real3d_t read3DVar(netCDF::NcFile &file,
                   const std::string &varName,
                   int ncol, int klev, int thirdDimSize)
{
    netCDF::NcVar var = file.getVar(varName);
    if(var.isNull()) {
        throw std::runtime_error("Variable " + varName + " not found in the file.");
    }
    // Read the entire 3D dataset into a flat vector.
    std::vector<real> flat(ncol * klev * thirdDimSize);
    var.getVar(flat.data());

    // Allocate a 3D Kokkos view.
    real3d_t view("var3d", ncol, klev, thirdDimSize);
    auto host_view = Kokkos::create_mirror_view(view);

    // Fill the host mirror with the data.
    for (int i = 0; i < ncol; ++i) {
        for (int j = 0; j < klev; ++j) {
            for (int k = 0; k < thirdDimSize; ++k) {
                host_view(i,j,k) = flat[i * klev * thirdDimSize + j * thirdDimSize + k];
            }
        }
    }
    Kokkos::deep_copy(view, host_view);
    return view;
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
        // Open the netCDF file in read-only mode.
        netCDF::NcFile file(nc_file, netCDF::NcFile::read);
        std::cout << "File name: " << nc_file << std::endl;

        // Get dimensions.
        netCDF::NcDim ncolDim = file.getDim("ncol");
        netCDF::NcDim nlayDim = file.getDim("nlay");  // "nlay" corresponds to klev in our code.
        data.ncol = ncolDim.getSize();
        data.klev = nlayDim.getSize();
        printf("ncol: %d, klev: %d\n", data.ncol, data.klev);

        // Read 2D variables. (Note: Some variables such as zh and tk_hl may have dimension (ncol, klev+1))
        data.dz         = read2DVar(file, "dz", data.ncol, data.klev);
        data.zh         = read2DVar(file, "zh", data.ncol, data.klev + 1);
        data.zf         = read2DVar(file, "zf", data.ncol, data.klev);
        data.pp_hl      = read2DVar(file, "pp_hl", data.ncol, data.klev + 1);
        data.pp_fl      = read2DVar(file, "pp_fl", data.ncol, data.klev);
        data.tk_hl      = read2DVar(file, "tk_hl", data.ncol, data.klev + 1);
        data.tk_fl      = read2DVar(file, "tk_fl", data.ncol, data.klev);
        data.xvmr_vap   = read2DVar(file, "xvmr_vap", data.ncol, data.klev);
        data.xm_liq     = read2DVar(file, "xm_liq", data.ncol, data.klev);
        data.xm_ice     = read2DVar(file, "xm_ice", data.ncol, data.klev);
        data.xm_snw     = read2DVar(file, "xm_snw", data.ncol, data.klev);
        data.cdnc       = read2DVar(file, "cdnc", data.ncol, data.klev);
        data.cld_frc    = read2DVar(file, "cld_frc", data.ncol, data.klev);
        data.reff_ice   = read2DVar(file, "reff_ice", data.ncol, data.klev);
        data.reff_snow  = read2DVar(file, "reff_snow", data.ncol, data.klev);

        // Read 1D variables.
        data.pp_sfc       = read1DVar(file, "pp_sfc", data.ncol);
        data.tk_sfc       = read1DVar(file, "tk_sfc", data.ncol);
        data.pcos_mu0     = read1DVar(file, "pcos_mu0", data.ncol);
        data.daylight_frc = read1DVar(file, "daylght_frc", data.ncol);
        data.alb_vis_dir  = read1DVar(file, "alb_vis_dir", data.ncol);
        data.alb_nir_dir  = read1DVar(file, "alb_nir_dir", data.ncol);
        data.alb_vis_dif  = read1DVar(file, "alb_vis_dif", data.ncol);
        data.alb_nir_dif  = read1DVar(file, "alb_nir_dif", data.ncol);
        data.emissivity   = read1DVar(file, "emissivity", data.ncol);

        // Read optional 1D variables (land and glacier masks).
        try {
            data.laland = read1DVar(file, "laland", data.ncol);
        } catch (std::runtime_error &e) {
            data.laland = real1d_t("laland", data.ncol);
            // Optionally, initialize to zero.
        }
        try {
            data.laglac = read1DVar(file, "laglac", data.ncol);
        } catch (std::runtime_error &e) {
            data.laglac = real1d_t("laglac", data.ncol);
        }

        // Read additional gas mixing ratios (assumed 2D).
        data.xvmr_co2 = read2DVar(file, "xvmr_co2", data.ncol, data.klev);
        data.xvmr_ch4 = read2DVar(file, "xvmr_ch4", data.ncol, data.klev);
        data.xvmr_n2o = read2DVar(file, "xvmr_n2o", data.ncol, data.klev);
        data.xvmr_o3  = read2DVar(file, "xvmr_o3", data.ncol, data.klev);
        data.xvmr_o2  = read2DVar(file, "xvmr_o2", data.ncol, data.klev);

        // Read the 3D variable xvmr_cfc (with third dimension size = 2).
        data.xvmr_cfc = read3DVar(file, "xvmr_cfc", data.ncol, data.klev, 2);
    }
    catch (netCDF::exceptions::NcException &e) {
      std::cerr << "NetCDF error: " << e.what() << std::endl;
      throw;
    }
    return data;
}
