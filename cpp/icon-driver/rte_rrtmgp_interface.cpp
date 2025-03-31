#include <algorithm>
// #include "CloudOptics.hpp"  // Ensure that this header defines CloudOptics and init_cloud_optics()
#include "rte_rrtmgp_interface.hpp"
#include "read_data.hpp"
#include "mo_gas_optics_rrtmgp.h"
#include "mo_cloud_optics.h"
#include "mo_gas_concentrations.h"
#include "mo_optical_props.h"
#include "mo_load_cloud_coefficients.h"
#include "mo_load_coefficients.h"
#include "mo_fluxes.h"
#include "mo_rte_lw.h"
#include "mo_rte_sw.h"

// Constructor definition.
RteRrtmgpInterface::RteRrtmgpInterface(const InputData &data)
    : input(data)
{
    // Set constant parameters.
    ccwmin = 1e-7;
    zkap_cont = 1.143;
    zkap_mrtm = 1.077;
    del1 = 2.0;
    del2 = 20.0;
    cld_frc_thresh = 4.0 * std::numeric_limits<double>::epsilon();
    droplet_scale = 1.0e2;
    rhoh2o = 1.0e3;
    effective_radius = 1.0e6 * droplet_scale * pow((3.0e-9 / (4.0 * M_PI * rhoh2o)), 1.0/3.0);
    nir_vis_boundary = 14500.0;

    fluxes_lw = FluxesBroadbandK<real, LayoutT>();
    fluxes_sw = FluxesBroadbandK<real, LayoutT>();
}

/// Clamp the pressure values in the source view and write the results to the target view.
/// The clamping bounds vary with the level (second index). For each column j (0-indexed):
///   tgt_min(j) = low  + j * epsilon
///   tgt_max(j) = high - ((n-1)-j) * epsilon
/// and then, for each element (i,j):
///   tgt(i,j) = min( tgt_max(j), max( tgt_min(j), src(i,j) ) )
void clamp_pressure(const real2d_t& src,
                    real2d_t& tgt,
                    real low,
                    real high)
{
  // Get dimensions. In Fortran, m = number of rows, n = number of columns.
  const int m = src.extent(0);
  const int n = src.extent(1);

  // Allocate temporary 1D views for the level-dependent clamping bounds.
  real1d_t tgt_min("tgt_min", n);
  real1d_t tgt_max("tgt_max", n);

  // Get machine epsilon for real
  real eps = std::numeric_limits<real>::epsilon();

  // Compute clamping bounds for each level (j = 0 ... n-1).
  Kokkos::parallel_for("ComputeTgtBounds", n, KOKKOS_LAMBDA (const int j) {
    tgt_min(j) = low + j * eps;
    tgt_max(j) = high - ( (n - 1) - j ) * eps;
  });
  Kokkos::fence();

  // Apply clamping to each element of the source array.
  // Here, we use an MDRangePolicy to iterate over the 2D index space.
  Kokkos::MDRangePolicy<Kokkos::Rank<2>> policy({0, 0}, {m, n});
  Kokkos::parallel_for("ClampPressure", policy, KOKKOS_LAMBDA (const int i, const int j) {
    // Compute the clamped value.
    real value = src(i,j);
    // Clamp value between tgt_min(j) and tgt_max(j)
    value = (value < tgt_min(j)) ? tgt_min(j) : value;
    value = (value > tgt_max(j)) ? tgt_max(j) : value;
    tgt(i,j) = value;
  });
  Kokkos::fence();
}

void clamp_temperature(const real2d_t& src, real2d_t& tgt, real low, real high) {
    // Get the dimensions.
    const int m = src.extent(0);
    const int n = src.extent(1);
    
    // Launch a 2D parallel kernel that performs the clamping.
    Kokkos::parallel_for("ClampTemperature",
        Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0,0}, {m,n}),
        KOKKOS_LAMBDA (const int i, const int j) {
            // Compute the clamped value.
            // This is equivalent to: min(high, max(low, src(i,j)))
            tgt(i,j) = Kokkos::min(high, Kokkos::max(low, src(i,j)));
        }
    );
    Kokkos::fence();
}

// on_block() definition.
void RteRrtmgpInterface::on_block() {
    int ncol = input.ncol;
    int klev = input.klev;

    // Obtain cloud optical properties (dummy values provided by init_cloud_optics).
    // auto cloudOptics = init_cloud_optics();
    // CloudOptics cloud_optics_lw = cloudOptics.first;
    // CloudOptics cloud_optics_sw = cloudOptics.second;
    CloudOpticsK<real, LayoutT> cloud_optics_lw;
    CloudOpticsK<real, LayoutT> cloud_optics_sw;
    
    GasConcsK<real, LayoutT> gas_concs;

    std::vector<std::string> gas_names = {
      "h2o", "co2", "ch4", "o2", "o3", "n2o", "cfc11", "cfc12"
    };

    gas_concs.init(gas_names,ncol,klev);
    gas_concs.set_vmr("h2o", input.xvmr_vap);
    gas_concs.set_vmr("co2", input.xvmr_co2);
    gas_concs.set_vmr("ch4", input.xvmr_ch4);
    gas_concs.set_vmr("o2", input.xvmr_o2);
    gas_concs.set_vmr("o3", input.xvmr_o3);
    gas_concs.set_vmr("n2o", input.xvmr_n2o);
    // gas_concs.set_vmr("cfc11", input.xvmr_cfc(0));
    // gas_concs.set_vmr("cfc12", input.xvmr_cfc(1));
    real2d_t xvmr_vap("xvmr_vap", ncol, klev);
    gas_concs.get_vmr("h2o", xvmr_vap);

    // print out the whole xvmr_vap array
    for (int i = 0; i < ncol; ++i) {
      for (int j = 0; j < klev; ++j) {
        // print the indices and the value only if not equal to zero
        double value = xvmr_vap(i, j) - input.xvmr_vap(i,j) ;
        if (value != 0.0) {
          std::cout << i << " " << j << " " << value << std::endl;
        }
      }
      std::cout << std::endl;
    }

    GasOpticsRRTMGPK<real, LayoutT> k_dist_lw;
    GasOpticsRRTMGPK<real, LayoutT> k_dist_sw;

    OpticalProps2strK<real, LayoutT> atmos;
    OpticalProps2strK<real, LayoutT> clouds;

    std::string k_dist_file_lw = "coefficients_lw.nc";
    load_and_init(k_dist_lw, k_dist_file_lw, gas_concs);
    std::string k_dist_file_sw = "coefficients_sw.nc";
    load_and_init(k_dist_sw, k_dist_file_sw, gas_concs);

    int nbndlw = k_dist_lw.get_nband();
    int nbndsw = k_dist_sw.get_nband();

    std::string cloud_optics_file_lw = "rrtmgp-cloud-optics-coeffs-lw.nc";
    load_cld_lutcoeff(cloud_optics_lw, cloud_optics_file_lw);
    std::string cloud_optics_file_sw = "rrtmgp-cloud-optics-coeffs-sw.nc";
    load_cld_lutcoeff(cloud_optics_sw, cloud_optics_file_sw);

    cloud_optics_lw.set_ice_roughness(2);
    cloud_optics_sw.set_ice_roughness(2);

    clouds.init(k_dist_lw.get_band_lims_wavenumber());

    double reimin = std::max(cloud_optics_lw.radice_lwr, cloud_optics_sw.radice_lwr);
    double reimax = std::min(cloud_optics_lw.radice_upr, cloud_optics_sw.radice_upr);
    double relmin = std::max(cloud_optics_lw.radliq_lwr, cloud_optics_sw.radliq_lwr);
    double relmax = std::min(cloud_optics_lw.radliq_upr, cloud_optics_sw.radliq_upr);
    
    real2d_t ziwp("ziwp", ncol, klev);
    real2d_t zlwp("zlwp", ncol, klev);
    real2d_t zswp("zswp", ncol, klev);
    real2d_t tau_ice("tau_ice", ncol, klev);
    real2d_t tau_snow("tau_snow", ncol, klev);
    real2d_t zdwp("zdwp", ncol, klev);

    // Initialize the views to zero:
    Kokkos::deep_copy(ziwp, 0.0);
    Kokkos::deep_copy(zlwp, 0.0);
    Kokkos::deep_copy(zswp, 0.0);
    Kokkos::deep_copy(tau_ice, 0.0);
    Kokkos::deep_copy(tau_snow, 0.0);
    Kokkos::deep_copy(zdwp, 0.0);

    real2d_t re_cryst("re_cryst", ncol, klev);
    real2d_t re_drop("re_drop", ncol, klev);
    real2d_t re_snow("re_snow", ncol, klev);

    Kokkos::parallel_for(
        "ProcessLevels", 
        Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0,0}, {ncol, klev}),
        KOKKOS_LAMBDA (const int jl, const int jk) {
        // Optional: Remove printing from device code (or use a device-safe logging mechanism)

        // Compute a local cloud fraction ensuring a minimum epsilon.
        double cld_frc_loc = input.cld_frc(jl, jk);
        cld_frc_loc = (cld_frc_loc < std::numeric_limits<double>::epsilon()) ?
        std::numeric_limits<double>::epsilon() : cld_frc_loc;

        // Compute water paths.
        ziwp(jl, jk) = input.xm_ice(jl, jk) * 1000.0 / cld_frc_loc;
        zlwp(jl, jk) = input.xm_liq(jl, jk) * 1000.0 / cld_frc_loc;
        zswp(jl, jk) = input.xm_snw(jl, jk) * 1000.0;

        // Determine if the level is cloudy.
        bool lcldlyr = (input.cld_frc(jl, jk) > cld_frc_thresh);
        if (!lcldlyr) {
        ziwp(jl, jk) = 0.0;
        zlwp(jl, jk) = 0.0;
        }

        // Calculate the liquid water content.
        double dz_val = input.dz(jl, jk);
        double zlwc = (dz_val > 0.0) ? (zlwp(jl, jk) / dz_val) : 0.0;

        // Compute crystal and droplet effective radii.
        if (lcldlyr && ((zlwp(jl, jk) + ziwp(jl, jk)) > ccwmin)) {
          double zkap = zkap_mrtm;
          if ((input.laland(jl) != 0.0) && (input.laglac(jl) == 0.0)) {
            zkap = zkap_cont;
          }
          re_cryst(jl, jk) = fmax(reimin, fmin(reimax, 1.0e6 * input.reff_ice(jl, jk)));
          if (input.cdnc(jl, jk) > 0.0) {
            re_drop(jl, jk) = fmax(relmin,
                fmin(relmax,
                  effective_radius * zkap * pow(zlwc / input.cdnc(jl, jk), 1.0/3.0)));
          } else {
            re_drop(jl, jk) = 0.0;
          }
        } else {
          re_cryst(jl, jk) = reimin;
          re_drop(jl, jk) = relmin;
        }

        // Compute snow effective radius.
        if (zswp(jl, jk) > ccwmin)
          re_snow(jl, jk) = fmax(reimin, fmin(reimax, 1.0e6 * input.reff_snow(jl, jk)));
        else
          re_snow(jl, jk) = reimin;
        }
    );

    // Restrict out-of-bounds temperatures and pressures
    //
    // The air pressure on levels plev, on the upper and lower boundaries of a layer,
    // is used to determine the air mass transferred by radiation. For safety
    // reasons plev is limited to values >= 0 Pa (and <=10**6 Pa so that high is defined).

    real low = 0.0;
    real high = 1.0e6;

    real2d_t plev("plev", ncol, klev+1);
    clamp_pressure(input.pp_hl, plev, low, high);

    // The pressure in the layer play is used for optical properties and
    // is limited here to the range for which tables are defined in the
    // RRTMGP data file and stored in k_dist_lw.
    // Thus radiation can be computed for pressure values lower or higher
    // than the defined range, albeit at lower precision.

    low =  k_dist_lw.get_press_min();
    high = k_dist_lw.get_press_max();

    real2d_t play("play", ncol, klev);
    clamp_pressure(input.pp_fl, play, low, high);

    low =  k_dist_lw.get_temp_min();
    high = k_dist_lw.get_temp_max();

    real2d_t tlev("tlev", ncol, klev+1);
    real2d_t tlay("tlay", ncol, klev);

    clamp_temperature(input.tk_hl, tlev, low, high);
    clamp_temperature(input.tk_fl, tlay, low, high);

    //--------------------------------
    //
    // Boundary conditions
    //
    //--------------------------------

    real1d_t mu0("mu0", ncol);

    const real low_bound  = 1.e-10;
    const real high_bound = 1.0;

    Kokkos::parallel_for("ClampMu0", ncol, KOKKOS_LAMBDA (const int j) {
      mu0(j) = Kokkos::min(high_bound, Kokkos::max(low_bound, input.pcos_mu0(j)));
    });
    Kokkos::fence();

    // 2.0 Surface Properties
    // --------------------------------
    real2d_t zsemiss("zsemiss", nbndlw, ncol);
    Kokkos::MDRangePolicy<Kokkos::Rank<2>> policy_emiss({0,0}, {nbndlw, ncol});
    Kokkos::parallel_for("SetSurfaceEmissivity", policy_emiss, KOKKOS_LAMBDA (const int i, const int j) {
      zsemiss(i,j) = input.emissivity(j);
    });
    Kokkos::fence();

    //
    // 4.1 Longwave radiative Transfer

    //
    // 4.1.2 Gas optics
    //
    SourceFuncLWK<real, LayoutT> source_lw;
    source_lw.alloc(ncol, klev, k_dist_lw);

    OpticalProps1sclK<real, LayoutT> atmos_lw;
    OpticalProps1sclK<real, LayoutT> clouds_bnd_lw;
    OpticalProps1sclK<real, LayoutT> snow_bnd_lw;

    atmos_lw.alloc_1scl(ncol, klev, k_dist_lw);
    clouds_bnd_lw.alloc_1scl(ncol, klev, k_dist_lw.get_band_lims_wavenumber());

    const size_t base_ref = 18000;
    const size_t my_size_ref = ncol * klev * (klev+1);

    using pool_t = typename decltype(k_dist_lw)::pool_t;

    pool_t::init(2e6 * (float(my_size_ref) / base_ref));

    pool_t::print_stats();

    real3d_t col_gas("col_gas", ncol, klev, k_dist_lw.get_ngas()+1);
    bool top_at_1 = false; // Do not know the actual value yet
    k_dist_lw.gas_optics(ncol, klev, top_at_1, play, plev, tlay, input.tk_sfc, gas_concs, col_gas, atmos_lw, source_lw, real2d_t(), tlev);

    cloud_optics_lw.cloud_optics(ncol, klev, zdwp, ziwp, re_drop, re_cryst, clouds_bnd_lw);

    for (int band = 0; band < nbndlw; ++band) {
      Kokkos::parallel_for(
        "AccumulateTauIce",
        Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0, 0}, {klev, ncol}),
        KOKKOS_LAMBDA(const int j, const int i) {
          tau_ice(i, j) += clouds_bnd_lw.tau(i, j, band);
        }
      );
      Kokkos::fence();
    }

    cloud_optics_lw.cloud_optics(ncol, klev, zlwp, ziwp, re_drop, re_cryst, clouds_bnd_lw);

    clouds_bnd_lw.increment(atmos_lw);
    clouds_bnd_lw.finalize();

    snow_bnd_lw.alloc_1scl(ncol, klev, k_dist_lw.get_band_lims_wavenumber());
    cloud_optics_lw.cloud_optics(ncol, klev, zdwp, zswp, re_snow, re_snow, snow_bnd_lw);

    for (int band = 0; band < nbndlw; ++band) {
      Kokkos::parallel_for(
        "ProcessSnowBand",
        Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0,0}, {klev, ncol}),
        KOKKOS_LAMBDA (const int j, const int i) {
          if (1.0e6 * input.reff_snow(i, j) > reimax) {
            snow_bnd_lw.tau(i, j, band) = snow_bnd_lw.tau(i, j, band) * reimax / (1.0e6 * input.reff_snow(i, j));
          }
          // Accumulate into tau_snow.
          tau_snow(i, j) += snow_bnd_lw.tau(i, j, band);
        }
      );
      Kokkos::fence();
    }

    snow_bnd_lw.increment(atmos_lw);
    snow_bnd_lw.finalize();

    real2d_t flux_up_lw ( "flux_up_lw", ncol, klev+1);
    real2d_t flux_dn_lw ( "flux_dn_lw", ncol, klev+1);
    real2d_t flux_net_lw("flux_net_lw", ncol, klev+1);

    fluxes_lw.flux_up = flux_up_lw;
    fluxes_lw.flux_dn = flux_dn_lw;
    fluxes_lw.flux_net = flux_net_lw;

    int constexpr max_gauss_pts = 4;

    hreal2d_t gauss_Ds_host_k ("gauss_Ds" ,max_gauss_pts,max_gauss_pts);
    gauss_Ds_host_k(0,0) = 1.66      ; gauss_Ds_host_k(1,0) =         0.; gauss_Ds_host_k(2,0) =         0.; gauss_Ds_host_k(3,0) =         0.;
    gauss_Ds_host_k(0,1) = 1.18350343; gauss_Ds_host_k(1,1) = 2.81649655; gauss_Ds_host_k(2,1) =         0.; gauss_Ds_host_k(3,1) =         0.;
    gauss_Ds_host_k(0,2) = 1.09719858; gauss_Ds_host_k(1,2) = 1.69338507; gauss_Ds_host_k(2,2) = 4.70941630; gauss_Ds_host_k(3,2) =         0.;
    gauss_Ds_host_k(0,3) = 1.06056257; gauss_Ds_host_k(1,3) = 1.38282560; gauss_Ds_host_k(2,3) = 2.40148179; gauss_Ds_host_k(3,3) = 7.15513024;

    hreal2d_t gauss_wts_host_k("gauss_wts",max_gauss_pts,max_gauss_pts);
    gauss_wts_host_k(0,0) = 0.5         ; gauss_wts_host_k(1,0) = 0.          ; gauss_wts_host_k(2,0) = 0.          ; gauss_wts_host_k(3,0) = 0.          ;
    gauss_wts_host_k(0,1) = 0.3180413817; gauss_wts_host_k(1,1) = 0.1819586183; gauss_wts_host_k(2,1) = 0.          ; gauss_wts_host_k(3,1) = 0.          ;
    gauss_wts_host_k(0,2) = 0.2009319137; gauss_wts_host_k(1,2) = 0.2292411064; gauss_wts_host_k(2,2) = 0.0698269799; gauss_wts_host_k(3,2) = 0.          ;
    gauss_wts_host_k(0,3) = 0.1355069134; gauss_wts_host_k(1,3) = 0.2034645680; gauss_wts_host_k(2,3) = 0.1298475476; gauss_wts_host_k(3,3) = 0.0311809710;

    real2d_t gauss_Ds_k ("gauss_Ds" ,max_gauss_pts,max_gauss_pts);
    real2d_t gauss_wts_k("gauss_wts",max_gauss_pts,max_gauss_pts);

    Kokkos::deep_copy(gauss_Ds_k, gauss_Ds_host_k);
    Kokkos::deep_copy(gauss_wts_k, gauss_wts_host_k);

    rte_lw(max_gauss_pts, gauss_Ds_k, gauss_wts_k, atmos_lw, top_at_1, source_lw, zsemiss, fluxes_lw);

    source_lw.finalize();
    atmos_lw.finalize();

    pool_t::finalize();

    using pool_t = typename decltype(k_dist_sw)::pool_t;

    pool_t::init(2e6 * (float(my_size_ref) / base_ref));

    pool_t::print_stats();

    OpticalProps2strK<real, LayoutT> atmos_sw;
    OpticalProps2strK<real, LayoutT> clouds_bnd_sw;
    OpticalProps2strK<real, LayoutT> snow_bnd_sw;

    atmos_sw.alloc_2str(ncol, klev, k_dist_sw);
    clouds_bnd_sw.alloc_2str(ncol, klev, k_dist_sw.get_band_lims_wavenumber());

    real2d_t toa_flux("toa_flux" , ncol, k_dist_sw.get_ngpt());

    k_dist_sw.gas_optics(ncol, klev, top_at_1, play, plev, tlay, gas_concs, col_gas, atmos_sw, toa_flux);

    cloud_optics_sw.cloud_optics(ncol, klev, zdwp, ziwp, re_drop, re_cryst, clouds_bnd_sw);

    cloud_optics_sw.cloud_optics(ncol, klev, zdwp, ziwp, re_drop, re_cryst, clouds_bnd_sw);

    // cloud_optics_sw.cloud_optics(ncol, klev, zlwp, ziwp, re_drop, re_cryst, clouds_bnd_sw);

    clouds_bnd_sw.delta_scale();
    clouds_bnd_sw.increment(atmos_sw);
    clouds_bnd_sw.finalize();

    snow_bnd_sw.alloc_2str(ncol, klev, k_dist_sw.get_band_lims_wavenumber());

    cloud_optics_sw.cloud_optics(ncol, klev, zdwp, zswp, re_snow, re_snow, snow_bnd_sw);

    snow_bnd_sw.delta_scale();
    snow_bnd_lw.increment(atmos_lw);
    snow_bnd_lw.finalize();

    //  Boundary conditions depending on whether the k-distribution being supplied
    real2d_t sfc_alb_dir("sfc_alb_dir",nbndsw,ncol);
    real2d_t sfc_alb_dif("sfc_alb_dif",nbndsw,ncol);
    real1d_t mu0_k        ("mu0"        ,ncol);

    // Ocean-ish values for no particular reason
    Kokkos::deep_copy(sfc_alb_dir, 0.06);
    Kokkos::deep_copy(sfc_alb_dif, 0.06);
    Kokkos::deep_copy(mu0_k        , 0.86);

    //
    // Surface albedo interpolation
    //
    real2d_t band_lims("band_lims", 2, nbndlw);

    band_lims = k_dist_lw.get_band_lims_wavenumber();

    real2d_t albdif("albdif", nbndsw, ncol);
    real2d_t albdir("albdir", nbndsw, ncol);

    Kokkos::MDRangePolicy<Kokkos::Rank<2>> policy_alb({0,0}, {nbndsw, ncol});
    Kokkos::parallel_for("AlbedoInterpolation", policy_alb, KOKKOS_LAMBDA (const int band, const int j) {
      real delwave = band_lims(1, band) - band_lims(0, band);
      real frc_vis = Kokkos::min(1.0, Kokkos::max(0.0, (band_lims(1, band) - nir_vis_boundary) / delwave));
      albdif(band,j) = input.alb_vis_dif(j) * frc_vis + input.alb_nir_dif(j) * (1.0 - frc_vis);
      albdir(band,j) = input.alb_vis_dir(j) * frc_vis + input.alb_nir_dir(j) * (1.0 - frc_vis);
    });

    Kokkos::fence();

    // FluxesBybandK<real, LayoutT> fluxes_sw;

    real2d_t flux_up_sw ( "flux_up_sw", ncol, klev+1);
    real2d_t flux_dn_sw ( "flux_dn_sw", ncol, klev+1);
    real2d_t flux_net_sw("flux_net_sw", ncol, klev+1);

    fluxes_sw.flux_up = flux_up_sw;
    fluxes_sw.flux_dn = flux_dn_sw;
    fluxes_sw.flux_net = flux_net_sw;

    rte_sw(atmos_sw, top_at_1, mu0_k, toa_flux, albdir, albdif, fluxes_sw);

    // print out full fluxes_sw.flux_up, fluxes_sw.flux_dn, fluxes_sw.flux_net on separate netcdf files
    // declare create_netcdf_file function
    // create_netcdf_file("flux_up_sw.nc", fluxes_sw.flux_up);
    // create_netcdf_file("flux_dn_sw.nc", fluxes_sw.flux_dn);
    // create_netcdf_file("flux_net_sw.nc", fluxes_sw.flux_net);

    std::cout << "on_block() processing complete.\n";
};

void RteRrtmgpInterface::create_netcdf_file(std::string filename) {
  try {
    netCDF::NcFile nc(filename, netCDF::NcFile::replace);

    // Define the dimensions.
    NcDim ncol_dim = nc.addDim("ncol", input.ncol);
    NcDim klev_dim = nc.addDim("klev", input.klev+1);

    // Define the variables.
    NcVar flux_up_lw_var = nc.addVar("flux_up_lw", ncFloat, {ncol_dim, klev_dim});
    NcVar flux_dn_lw_var = nc.addVar("flux_dn_lw", ncFloat, {ncol_dim, klev_dim});
    NcVar flux_net_lw_var = nc.addVar("flux_net_lw", ncFloat, {ncol_dim, klev_dim});
    NcVar flux_up_sw_var = nc.addVar("flux_up_sw", ncFloat, {ncol_dim, klev_dim});
    NcVar flux_dn_sw_var = nc.addVar("flux_dn_sw", ncFloat, {ncol_dim, klev_dim});
    NcVar flux_net_sw_var = nc.addVar("flux_net_sw", ncFloat, {ncol_dim, klev_dim});

    auto flux_up_lw_host = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), fluxes_lw.flux_up);
    auto flux_dn_lw_host = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), fluxes_lw.flux_dn);
    auto flux_net_lw_host = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), fluxes_lw.flux_net);
    auto flux_up_sw_host = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), fluxes_sw.flux_up);
    auto flux_dn_sw_host = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), fluxes_sw.flux_dn);
    auto flux_net_sw_host = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), fluxes_sw.flux_net);

    // Write the data.
    flux_up_lw_var.putVar(flux_up_lw_host.data());
    flux_dn_lw_var.putVar(flux_dn_lw_host.data());
    flux_net_lw_var.putVar(flux_net_lw_host.data());

    flux_up_sw_var.putVar(flux_up_sw_host.data());
    flux_dn_sw_var.putVar(flux_dn_sw_host.data());
    flux_net_sw_var.putVar(flux_net_sw_host.data());

    // Close the file.
    nc.close();

  } catch (netCDF::exceptions::NcException &e) {
    std::cerr << "Error creating netCDF file '" << filename << "': " << e.what() << std::endl;
  }
}
