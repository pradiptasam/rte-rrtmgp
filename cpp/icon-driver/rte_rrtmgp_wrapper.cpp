#include <Kokkos_Core.hpp>
#include "rte_rrtmgp_interface.hpp"
extern "C" {
    void rte_rrtmgp_interface_onblock_cpp(
        // Input flags
        const bool* lclrsky_lw, const bool* lclrsky_sw,
        const bool* inhom_lts, const double* inhom_lts_max,
        // Dimensions
        const int* ncol, const int* klev,
        // Solar properties
        const double* psctm, const double* ssi_factor,
        // Land/glacier masks
        const bool* laland, const bool* laglac,
        // Surface and atmospheric properties
        const double* pcos_mu0, const double* daylght_frc,
        const double* alb_vis_dir, const double* alb_nir_dir,
        const double* alb_vis_dif, const double* alb_nir_dif,
        const double* emissivity,
        const double* zf, const double* zh, const double* dz,
        const double* pp_sfc, const double* pp_fl, const double* pp_hl,
        const double* tk_sfc, const double* tk_fl, const double* tk_hl,
        double* rad_2d, 
        const double* xvmr_vap, const double* xm_liq,
        const double* xm_ice, const double* reff_ice, double* tau_ice,
        const double* reff_snow, double* tau_snow,
        const double* cdnc, const double* cld_frc, const double* xm_snw,
        const double* xvmr_co2, const double* xvmr_ch4, const double* xvmr_n2o,
        const double* xvmr_cfc, const double* xvmr_o3, const double* xvmr_o2,
        const double* aer_tau_lw,
        const double* aer_tau_sw, const double* aer_ssa_sw, const double* aer_asy_sw,
        // Output fluxes
        double* flx_uplw, double* flx_uplw_clr,
        double* flx_dnlw, double* flx_dnlw_clr,
        double* flx_upsw, double* flx_upsw_clr,
        double* flx_dnsw, double* flx_dnsw_clr,
        // Surface fluxes
        double* vis_dn_dir_sfc, double* par_dn_dir_sfc, double* nir_dn_dir_sfc,
        double* vis_dn_dff_sfc, double* par_dn_dff_sfc, double* nir_dn_dff_sfc,
        double* vis_up_sfc, double* par_up_sfc, double* nir_up_sfc) 
    {
        // Set dimensions
        int ncol_val = *ncol;
        int klev_val = *klev;
        
        // Create InputData struct
        InputData input_data;
        input_data.ncol = ncol_val;
        input_data.klev = klev_val;
        
        // Initialize Kokkos Views for all data fields
        
        // 1D arrays
        input_data.pp_sfc = real1d_t("pp_sfc", ncol_val);
        input_data.tk_sfc = real1d_t("tk_sfc", ncol_val);
        input_data.pcos_mu0 = real1d_t("pcos_mu0", ncol_val);
        input_data.daylight_frc = real1d_t("daylight_frc", ncol_val);
        input_data.alb_vis_dir = real1d_t("alb_vis_dir", ncol_val);
        input_data.alb_nir_dir = real1d_t("alb_nir_dir", ncol_val);
        input_data.alb_vis_dif = real1d_t("alb_vis_dif", ncol_val);
        input_data.alb_nir_dif = real1d_t("alb_nir_dif", ncol_val);
        input_data.emissivity = real1d_t("emissivity", ncol_val);
        input_data.laland = real1d_t("laland", ncol_val);
        input_data.laglac = real1d_t("laglac", ncol_val);
        
        // 2D arrays
        input_data.zf = real2d_t("zf", ncol_val, klev_val);
        input_data.zh = real2d_t("zh", ncol_val, klev_val + 1);
        input_data.dz = real2d_t("dz", ncol_val, klev_val);
        input_data.pp_fl = real2d_t("pp_fl", ncol_val, klev_val);
        input_data.pp_hl = real2d_t("pp_hl", ncol_val, klev_val + 1);
        input_data.tk_fl = real2d_t("tk_fl", ncol_val, klev_val);
        input_data.tk_hl = real2d_t("tk_hl", ncol_val, klev_val + 1);
        input_data.xvmr_vap = real2d_t("xvmr_vap", ncol_val, klev_val);
        input_data.xm_liq = real2d_t("xm_liq", ncol_val, klev_val);
        input_data.xm_ice = real2d_t("xm_ice", ncol_val, klev_val);
        input_data.xm_snw = real2d_t("xm_snw", ncol_val, klev_val);
        input_data.cdnc = real2d_t("cdnc", ncol_val, klev_val);
        input_data.cld_frc = real2d_t("cld_frc", ncol_val, klev_val);
        input_data.reff_ice = real2d_t("reff_ice", ncol_val, klev_val);
        input_data.reff_snow = real2d_t("reff_snow", ncol_val, klev_val);
        input_data.xvmr_co2 = real2d_t("xvmr_co2", ncol_val, klev_val);
        input_data.xvmr_ch4 = real2d_t("xvmr_ch4", ncol_val, klev_val);
        input_data.xvmr_n2o = real2d_t("xvmr_n2o", ncol_val, klev_val);
        input_data.xvmr_o3 = real2d_t("xvmr_o3", ncol_val, klev_val);
        input_data.xvmr_o2 = real2d_t("xvmr_o2", ncol_val, klev_val);
        
        // 3D arrays
        input_data.xvmr_cfc = real3d_t("xvmr_cfc", ncol_val, klev_val, 2);
        
        // Create host mirrors for the Kokkos Views
        auto pp_sfc_h = Kokkos::create_mirror_view(input_data.pp_sfc);
        auto tk_sfc_h = Kokkos::create_mirror_view(input_data.tk_sfc);
        auto pcos_mu0_h = Kokkos::create_mirror_view(input_data.pcos_mu0);
        auto daylight_frc_h = Kokkos::create_mirror_view(input_data.daylight_frc);
        auto alb_vis_dir_h = Kokkos::create_mirror_view(input_data.alb_vis_dir);
        auto alb_nir_dir_h = Kokkos::create_mirror_view(input_data.alb_nir_dir);
        auto alb_vis_dif_h = Kokkos::create_mirror_view(input_data.alb_vis_dif);
        auto alb_nir_dif_h = Kokkos::create_mirror_view(input_data.alb_nir_dif);
        auto emissivity_h = Kokkos::create_mirror_view(input_data.emissivity);
        auto laland_h = Kokkos::create_mirror_view(input_data.laland);
        auto laglac_h = Kokkos::create_mirror_view(input_data.laglac);
        
        auto zf_h = Kokkos::create_mirror_view(input_data.zf);
        auto zh_h = Kokkos::create_mirror_view(input_data.zh);
        auto dz_h = Kokkos::create_mirror_view(input_data.dz);
        auto pp_fl_h = Kokkos::create_mirror_view(input_data.pp_fl);
        auto pp_hl_h = Kokkos::create_mirror_view(input_data.pp_hl);
        auto tk_fl_h = Kokkos::create_mirror_view(input_data.tk_fl);
        auto tk_hl_h = Kokkos::create_mirror_view(input_data.tk_hl);
        auto xvmr_vap_h = Kokkos::create_mirror_view(input_data.xvmr_vap);
        auto xm_liq_h = Kokkos::create_mirror_view(input_data.xm_liq);
        auto xm_ice_h = Kokkos::create_mirror_view(input_data.xm_ice);
        auto xm_snw_h = Kokkos::create_mirror_view(input_data.xm_snw);
        auto cdnc_h = Kokkos::create_mirror_view(input_data.cdnc);
        auto cld_frc_h = Kokkos::create_mirror_view(input_data.cld_frc);
        auto reff_ice_h = Kokkos::create_mirror_view(input_data.reff_ice);
        auto reff_snow_h = Kokkos::create_mirror_view(input_data.reff_snow);
        auto xvmr_co2_h = Kokkos::create_mirror_view(input_data.xvmr_co2);
        auto xvmr_ch4_h = Kokkos::create_mirror_view(input_data.xvmr_ch4);
        auto xvmr_n2o_h = Kokkos::create_mirror_view(input_data.xvmr_n2o);
        auto xvmr_o3_h = Kokkos::create_mirror_view(input_data.xvmr_o3);
        auto xvmr_o2_h = Kokkos::create_mirror_view(input_data.xvmr_o2);
        
        auto xvmr_cfc_h = Kokkos::create_mirror_view(input_data.xvmr_cfc);
        
        // Copy data from Fortran arrays to host mirrors
        // 1D arrays
        for (int i = 0; i < ncol_val; i++) {
            pp_sfc_h(i) = pp_sfc[i];
            tk_sfc_h(i) = tk_sfc[i];
            pcos_mu0_h(i) = pcos_mu0[i];
            daylight_frc_h(i) = daylght_frc[i];
            alb_vis_dir_h(i) = alb_vis_dir[i];
            alb_nir_dir_h(i) = alb_nir_dir[i];
            alb_vis_dif_h(i) = alb_vis_dif[i];
            alb_nir_dif_h(i) = alb_nir_dif[i];
            emissivity_h(i) = emissivity[i];
            laland_h(i) = laland[i] ? 1.0 : 0.0;  // Convert boolean to real
            laglac_h(i) = laglac[i] ? 1.0 : 0.0;  // Convert boolean to real
        }
        
        // 2D arrays - convert from Fortran to C++ array layout
        for (int i = 0; i < ncol_val; i++) {
            for (int j = 0; j < klev_val; j++) {
                int idx = i + j * ncol_val;  // Fortran column-major layout
                
                zf_h(i, j) = zf[idx];
                dz_h(i, j) = dz[idx];
                pp_fl_h(i, j) = pp_fl[idx];
                tk_fl_h(i, j) = tk_fl[idx];
                xvmr_vap_h(i, j) = xvmr_vap[idx];
                xm_liq_h(i, j) = xm_liq[idx];
                xm_ice_h(i, j) = xm_ice[idx];
                xm_snw_h(i, j) = xm_snw[idx];
                cdnc_h(i, j) = cdnc[idx];
                cld_frc_h(i, j) = cld_frc[idx];
                reff_ice_h(i, j) = reff_ice[idx];
                reff_snow_h(i, j) = reff_snow[idx];
                xvmr_co2_h(i, j) = xvmr_co2[idx];
                xvmr_ch4_h(i, j) = xvmr_ch4[idx];
                xvmr_n2o_h(i, j) = xvmr_n2o[idx];
                xvmr_o3_h(i, j) = xvmr_o3[idx];
                xvmr_o2_h(i, j) = xvmr_o2[idx];
            }
            
            // Handle arrays with klev+1 dimension
            for (int j = 0; j < klev_val + 1; j++) {
                int idx = i + j * ncol_val;
                zh_h(i, j) = zh[idx];
                pp_hl_h(i, j) = pp_hl[idx];
                tk_hl_h(i, j) = tk_hl[idx];
            }
        }
        
        // 3D arrays
        for (int i = 0; i < ncol_val; i++) {
            for (int j = 0; j < klev_val; j++) {
                for (int k = 0; k < 2; k++) {
                    int idx = i + j * ncol_val + k * ncol_val * klev_val;
                    xvmr_cfc_h(i, j, k) = xvmr_cfc[idx];
                }
            }
        }
        
        // Deep copy from host mirrors to device views
        Kokkos::deep_copy(input_data.pp_sfc, pp_sfc_h);
        Kokkos::deep_copy(input_data.tk_sfc, tk_sfc_h);
        Kokkos::deep_copy(input_data.pcos_mu0, pcos_mu0_h);
        Kokkos::deep_copy(input_data.daylight_frc, daylight_frc_h);
        Kokkos::deep_copy(input_data.alb_vis_dir, alb_vis_dir_h);
        Kokkos::deep_copy(input_data.alb_nir_dir, alb_nir_dir_h);
        Kokkos::deep_copy(input_data.alb_vis_dif, alb_vis_dif_h);
        Kokkos::deep_copy(input_data.alb_nir_dif, alb_nir_dif_h);
        Kokkos::deep_copy(input_data.emissivity, emissivity_h);
        Kokkos::deep_copy(input_data.laland, laland_h);
        Kokkos::deep_copy(input_data.laglac, laglac_h);
        
        Kokkos::deep_copy(input_data.zf, zf_h);
        Kokkos::deep_copy(input_data.zh, zh_h);
        Kokkos::deep_copy(input_data.dz, dz_h);
        Kokkos::deep_copy(input_data.pp_fl, pp_fl_h);
        Kokkos::deep_copy(input_data.pp_hl, pp_hl_h);
        Kokkos::deep_copy(input_data.tk_fl, tk_fl_h);
        Kokkos::deep_copy(input_data.tk_hl, tk_hl_h);
        Kokkos::deep_copy(input_data.xvmr_vap, xvmr_vap_h);
        Kokkos::deep_copy(input_data.xm_liq, xm_liq_h);
        Kokkos::deep_copy(input_data.xm_ice, xm_ice_h);
        Kokkos::deep_copy(input_data.xm_snw, xm_snw_h);
        Kokkos::deep_copy(input_data.cdnc, cdnc_h);
        Kokkos::deep_copy(input_data.cld_frc, cld_frc_h);
        Kokkos::deep_copy(input_data.reff_ice, reff_ice_h);
        Kokkos::deep_copy(input_data.reff_snow, reff_snow_h);
        Kokkos::deep_copy(input_data.xvmr_co2, xvmr_co2_h);
        Kokkos::deep_copy(input_data.xvmr_ch4, xvmr_ch4_h);
        Kokkos::deep_copy(input_data.xvmr_n2o, xvmr_n2o_h);
        Kokkos::deep_copy(input_data.xvmr_o3, xvmr_o3_h);
        Kokkos::deep_copy(input_data.xvmr_o2, xvmr_o2_h);
        
        Kokkos::deep_copy(input_data.xvmr_cfc, xvmr_cfc_h);
        
        // Create RteRrtmgpInterface instance and call on_block
        RteRrtmgpInterface interface(input_data);
        interface.on_block();
        
        // Create Kokkos Views for the outputs
        real2d_t tau_ice_view("tau_ice", ncol_val, klev_val);
        real2d_t tau_snow_view("tau_snow", ncol_val, klev_val);
        
        // The interface has already calculated the fluxes in interface.fluxes_lw and interface.fluxes_sw
        
        // Create host mirrors for the output views
        auto tau_ice_h = Kokkos::create_mirror_view(tau_ice_view);
        auto tau_snow_h = Kokkos::create_mirror_view(tau_snow_view);
        auto lw_flux_up_h = Kokkos::create_mirror_view(interface.fluxes_lw.flux_up);
        auto lw_flux_dn_h = Kokkos::create_mirror_view(interface.fluxes_lw.flux_dn);
        auto sw_flux_up_h = Kokkos::create_mirror_view(interface.fluxes_sw.flux_up);
        auto sw_flux_dn_h = Kokkos::create_mirror_view(interface.fluxes_sw.flux_dn);
        
        // Copy data from device to host
        Kokkos::deep_copy(tau_ice_h, tau_ice_view);
        Kokkos::deep_copy(tau_snow_h, tau_snow_view);
        Kokkos::deep_copy(lw_flux_up_h, interface.fluxes_lw.flux_up);
        Kokkos::deep_copy(lw_flux_dn_h, interface.fluxes_lw.flux_dn);
        Kokkos::deep_copy(sw_flux_up_h, interface.fluxes_sw.flux_up);
        Kokkos::deep_copy(sw_flux_dn_h, interface.fluxes_sw.flux_dn);
        
        // Copy data from host mirrors back to Fortran arrays
        
        // Copy optical depths back to Fortran arrays
        for (int i = 0; i < ncol_val; i++) {
            for (int j = 0; j < klev_val; j++) {
                int idx = i + j * ncol_val;
                tau_ice[idx] = tau_ice_h(i, j);
                tau_snow[idx] = tau_snow_h(i, j);
            }
        }
        
        // Copy flux profiles back to Fortran arrays
        for (int i = 0; i < ncol_val; i++) {
            for (int j = 0; j < klev_val + 1; j++) {
                int idx = i + j * ncol_val;
                
                flx_uplw[idx] = lw_flux_up_h(i, j);
                flx_dnlw[idx] = lw_flux_dn_h(i, j);
                flx_upsw[idx] = sw_flux_up_h(i, j);
                flx_dnsw[idx] = sw_flux_dn_h(i, j);
                
                // For clear-sky fluxes, we would need additional fields in the interface
                // For now, just use all-sky values as placeholders
                flx_uplw_clr[idx] = lw_flux_up_h(i, j);
                flx_dnlw_clr[idx] = lw_flux_dn_h(i, j);
                flx_upsw_clr[idx] = sw_flux_up_h(i, j);
                flx_dnsw_clr[idx] = sw_flux_dn_h(i, j);
            }
        }
        
        // For surface fluxes, we would need additional code in the interface
        // For now, set placeholders
        for (int i = 0; i < ncol_val; i++) {
            vis_dn_dir_sfc[i] = 0.0;
            par_dn_dir_sfc[i] = 0.0;
            nir_dn_dir_sfc[i] = 0.0;
            vis_dn_dff_sfc[i] = 0.0;
            par_dn_dff_sfc[i] = 0.0;
            nir_dn_dff_sfc[i] = 0.0;
            vis_up_sfc[i] = 0.0;
            par_up_sfc[i] = 0.0;
            nir_up_sfc[i] = 0.0;
        }
    }
}
