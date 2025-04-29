MODULE rte_rrtmgp_interface_mod
  USE, INTRINSIC :: ISO_C_BINDING
  IMPLICIT NONE
  
  INTERFACE
    SUBROUTINE rte_rrtmgp_interface_onblock_cpp( &
        lclrsky_lw, lclrsky_sw, inhom_lts, inhom_lts_max, &
        ncol, klev, psctm, ssi_factor, &
        laland, laglac, pcos_mu0, daylght_frc, &
        alb_vis_dir, alb_nir_dir, alb_vis_dif, alb_nir_dif, &
        emissivity, zf, zh, dz, &
        pp_sfc, pp_fl, pp_hl, tk_sfc, tk_fl, tk_hl, &
        rad_2d, xvmr_vap, xm_liq, xm_ice, reff_ice, tau_ice, &
        reff_snow, tau_snow, cdnc, cld_frc, xm_snw, &
        xvmr_co2, xvmr_ch4, xvmr_n2o, xvmr_cfc, xvmr_o3, xvmr_o2, &
        aer_tau_lw, aer_tau_sw, aer_ssa_sw, aer_asy_sw, &
        flx_uplw, flx_uplw_clr, flx_dnlw, flx_dnlw_clr, &
        flx_upsw, flx_upsw_clr, flx_dnsw, flx_dnsw_clr, &
        vis_dn_dir_sfc, par_dn_dir_sfc, nir_dn_dir_sfc, &
        vis_dn_dff_sfc, par_dn_dff_sfc, nir_dn_dff_sfc, &
        vis_up_sfc, par_up_sfc, nir_up_sfc) &
        BIND(C, NAME='rte_rrtmgp_interface_onblock_cpp')
      
      IMPORT :: C_BOOL, C_INT, C_DOUBLE
      
      ! Input parameters
      LOGICAL(KIND=C_BOOL), INTENT(IN) :: lclrsky_lw, lclrsky_sw
      LOGICAL(KIND=C_BOOL), INTENT(IN) :: inhom_lts
      REAL(KIND=C_DOUBLE), INTENT(IN) :: inhom_lts_max
      INTEGER(KIND=C_INT), INTENT(IN) :: ncol, klev
      REAL(KIND=C_DOUBLE), INTENT(IN) :: psctm
      REAL(KIND=C_DOUBLE), INTENT(IN) :: ssi_factor(*)
      LOGICAL(KIND=C_BOOL), INTENT(IN) :: laland(*), laglac(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: pcos_mu0(*), daylght_frc(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: alb_vis_dir(*), alb_nir_dir(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: alb_vis_dif(*), alb_nir_dif(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: emissivity(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: zf(*), zh(*), dz(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: pp_sfc(*), pp_fl(*), pp_hl(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: tk_sfc(*), tk_fl(*), tk_hl(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: rad_2d(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: xvmr_vap(*), xm_liq(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: xm_ice(*), reff_ice(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: tau_ice(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: reff_snow(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: tau_snow(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: cdnc(*), cld_frc(*), xm_snw(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: xvmr_co2(*), xvmr_ch4(*), xvmr_n2o(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: xvmr_cfc(*), xvmr_o3(*), xvmr_o2(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: aer_tau_lw(*), aer_tau_sw(*)
      REAL(KIND=C_DOUBLE), INTENT(IN) :: aer_ssa_sw(*), aer_asy_sw(*)
      
      ! Output flux parameters
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: flx_uplw(*), flx_uplw_clr(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: flx_dnlw(*), flx_dnlw_clr(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: flx_upsw(*), flx_upsw_clr(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: flx_dnsw(*), flx_dnsw_clr(*)
      
      ! Output surface flux parameters
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: vis_dn_dir_sfc(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: par_dn_dir_sfc(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: nir_dn_dir_sfc(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: vis_dn_dff_sfc(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: par_dn_dff_sfc(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: nir_dn_dff_sfc(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: vis_up_sfc(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: par_up_sfc(*)
      REAL(KIND=C_DOUBLE), INTENT(INOUT) :: nir_up_sfc(*)
    END SUBROUTINE rte_rrtmgp_interface_onblock_cpp
  END INTERFACE

CONTAINS

  SUBROUTINE rte_rrtmgp_interface_onBlock( &
      lclrsky_lw, lclrsky_sw, inhom_lts, inhom_lts_max, &
      ncol, klev, psctm, ssi_factor, &
      laland, laglac, pcos_mu0, daylght_frc, &
      alb_vis_dir, alb_nir_dir, alb_vis_dif, alb_nir_dif, &
      emissivity, zf, zh, dz, &
      pp_sfc, pp_fl, pp_hl, tk_sfc, tk_fl, tk_hl, &
      rad_2d, xvmr_vap, xm_liq, xm_ice, reff_ice, tau_ice, &
      reff_snow, tau_snow, cdnc, cld_frc, xm_snw, &
      xvmr_co2, xvmr_ch4, xvmr_n2o, xvmr_cfc, xvmr_o3, xvmr_o2, &
      aer_tau_lw, aer_tau_sw, aer_ssa_sw, aer_asy_sw, &
      flx_uplw, flx_uplw_clr, flx_dnlw, flx_dnlw_clr, &
      flx_upsw, flx_upsw_clr, flx_dnsw, flx_dnsw_clr, &
      vis_dn_dir_sfc, par_dn_dir_sfc, nir_dn_dir_sfc, &
      vis_dn_dff_sfc, par_dn_dff_sfc, nir_dn_dff_sfc, &
      vis_up_sfc, par_up_sfc, nir_up_sfc)
    
    ! Parameters as defined in the original interface
    LOGICAL, INTENT(IN) :: lclrsky_lw, lclrsky_sw
    LOGICAL, INTENT(IN) :: inhom_lts
    REAL(C_DOUBLE), INTENT(IN) :: inhom_lts_max
    INTEGER, INTENT(IN) :: ncol, klev
    REAL(C_DOUBLE), INTENT(IN) :: psctm
    REAL(C_DOUBLE), INTENT(IN) :: ssi_factor(:)
    LOGICAL, INTENT(IN) :: laland(:), laglac(:)
    REAL(C_DOUBLE), INTENT(IN) :: pcos_mu0(:), daylght_frc(:)
    REAL(C_DOUBLE), INTENT(IN) :: alb_vis_dir(:), alb_nir_dir(:)
    REAL(C_DOUBLE), INTENT(IN) :: alb_vis_dif(:), alb_nir_dif(:)
    REAL(C_DOUBLE), INTENT(IN) :: emissivity(:)
    REAL(C_DOUBLE), INTENT(IN) :: zf(:,:), zh(:,:), dz(:,:)
    REAL(C_DOUBLE), INTENT(IN) :: pp_sfc(:), pp_fl(:,:), pp_hl(:,:)
    REAL(C_DOUBLE), INTENT(IN) :: tk_sfc(:), tk_fl(:,:), tk_hl(:,:)
    REAL(C_DOUBLE), INTENT(INOUT) :: rad_2d(:)
    REAL(C_DOUBLE), INTENT(IN) :: xvmr_vap(:,:), xm_liq(:,:)
    REAL(C_DOUBLE), INTENT(IN) :: xm_ice(:,:), reff_ice(:,:)
    REAL(C_DOUBLE), INTENT(INOUT) :: tau_ice(:,:)
    REAL(C_DOUBLE), INTENT(IN) :: reff_snow(:,:)
    REAL(C_DOUBLE), INTENT(INOUT) :: tau_snow(:,:)
    REAL(C_DOUBLE), INTENT(IN) :: cdnc(:,:), cld_frc(:,:), xm_snw(:,:)
    REAL(C_DOUBLE), INTENT(IN) :: xvmr_co2(:,:), xvmr_ch4(:,:), xvmr_n2o(:,:)
    REAL(C_DOUBLE), INTENT(IN) :: xvmr_cfc(:,:,:), xvmr_o3(:,:), xvmr_o2(:,:)
    REAL(C_DOUBLE), INTENT(IN) :: aer_tau_lw(:,:,:), aer_tau_sw(:,:,:)
    REAL(C_DOUBLE), INTENT(IN) :: aer_ssa_sw(:,:,:), aer_asy_sw(:,:,:)
    
    REAL(C_DOUBLE), INTENT(INOUT) :: flx_uplw(:,:), flx_uplw_clr(:,:)
    REAL(C_DOUBLE), INTENT(INOUT) :: flx_dnlw(:,:), flx_dnlw_clr(:,:)
    REAL(C_DOUBLE), INTENT(INOUT) :: flx_upsw(:,:), flx_upsw_clr(:,:)
    REAL(C_DOUBLE), INTENT(INOUT) :: flx_dnsw(:,:), flx_dnsw_clr(:,:)
    
    REAL(C_DOUBLE), INTENT(INOUT) :: vis_dn_dir_sfc(:), par_dn_dir_sfc(:), nir_dn_dir_sfc(:)
    REAL(C_DOUBLE), INTENT(INOUT) :: vis_dn_dff_sfc(:), par_dn_dff_sfc(:), nir_dn_dff_sfc(:)
    REAL(C_DOUBLE), INTENT(INOUT) :: vis_up_sfc(:), par_up_sfc(:), nir_up_sfc(:)
    
    ! Local C-compatible variables
    LOGICAL(KIND=C_BOOL) :: c_lclrsky_lw, c_lclrsky_sw, c_inhom_lts
    INTEGER(KIND=C_INT) :: c_ncol, c_klev
    
    ! Create temporary 1D arrays for multidimensional arrays
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: zf_1d(:), zh_1d(:), dz_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: pp_fl_1d(:), pp_hl_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: tk_fl_1d(:), tk_hl_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: xvmr_vap_1d(:), xm_liq_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: xm_ice_1d(:), reff_ice_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: tau_ice_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: reff_snow_1d(:), tau_snow_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: cdnc_1d(:), cld_frc_1d(:), xm_snw_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: xvmr_co2_1d(:), xvmr_ch4_1d(:), xvmr_n2o_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: xvmr_o3_1d(:), xvmr_o2_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: xvmr_cfc_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: aer_tau_lw_1d(:), aer_tau_sw_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: aer_ssa_sw_1d(:), aer_asy_sw_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: flx_uplw_1d(:), flx_uplw_clr_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: flx_dnlw_1d(:), flx_dnlw_clr_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: flx_upsw_1d(:), flx_upsw_clr_1d(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: flx_dnsw_1d(:), flx_dnsw_clr_1d(:)
    
    ! Local arrays for boolean conversions
    LOGICAL(KIND=C_BOOL), ALLOCATABLE :: c_laland(:), c_laglac(:)
    
    ! Temporary arrays for surface fluxes
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: c_vis_dn_dir_sfc(:), c_par_dn_dir_sfc(:), c_nir_dn_dir_sfc(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: c_vis_dn_dff_sfc(:), c_par_dn_dff_sfc(:), c_nir_dn_dff_sfc(:)
    REAL(KIND=C_DOUBLE), ALLOCATABLE :: c_vis_up_sfc(:), c_par_up_sfc(:), c_nir_up_sfc(:)
    
    INTEGER :: i, j, k, idx
    
    ! Convert Fortran logical to C_BOOL
    c_lclrsky_lw = lclrsky_lw
    c_lclrsky_sw = lclrsky_sw
    c_inhom_lts = inhom_lts
    
    ! Convert INTEGER to C_INT
    c_ncol = ncol
    c_klev = klev
    
    ! Allocate temporary 1D arrays for conversion
    ALLOCATE(zf_1d(ncol*klev), zh_1d(ncol*(klev+1)), dz_1d(ncol*klev))
    ALLOCATE(pp_fl_1d(ncol*klev), pp_hl_1d(ncol*(klev+1)))
    ALLOCATE(tk_fl_1d(ncol*klev), tk_hl_1d(ncol*(klev+1)))
    ALLOCATE(xvmr_vap_1d(ncol*klev), xm_liq_1d(ncol*klev))
    ALLOCATE(xm_ice_1d(ncol*klev), reff_ice_1d(ncol*klev))
    ALLOCATE(tau_ice_1d(ncol*klev))
    ALLOCATE(reff_snow_1d(ncol*klev), tau_snow_1d(ncol*klev))
    ALLOCATE(cdnc_1d(ncol*klev), cld_frc_1d(ncol*klev), xm_snw_1d(ncol*klev))
    ALLOCATE(xvmr_co2_1d(ncol*klev), xvmr_ch4_1d(ncol*klev), xvmr_n2o_1d(ncol*klev))
    ALLOCATE(xvmr_o3_1d(ncol*klev), xvmr_o2_1d(ncol*klev))
    ALLOCATE(xvmr_cfc_1d(ncol*klev*2))
    ALLOCATE(aer_tau_lw_1d(ncol*klev*SIZE(aer_tau_lw, 3)))
    ALLOCATE(aer_tau_sw_1d(ncol*klev*SIZE(aer_tau_sw, 3)))
    ALLOCATE(aer_ssa_sw_1d(ncol*klev*SIZE(aer_ssa_sw, 3)))
    ALLOCATE(aer_asy_sw_1d(ncol*klev*SIZE(aer_asy_sw, 3)))
    
    ALLOCATE(flx_uplw_1d(ncol*(klev+1)), flx_uplw_clr_1d(ncol*(klev+1)))
    ALLOCATE(flx_dnlw_1d(ncol*(klev+1)), flx_dnlw_clr_1d(ncol*(klev+1)))
    ALLOCATE(flx_upsw_1d(ncol*(klev+1)), flx_upsw_clr_1d(ncol*(klev+1)))
    ALLOCATE(flx_dnsw_1d(ncol*(klev+1)), flx_dnsw_clr_1d(ncol*(klev+1)))
    
    ! Allocate boolean arrays
    ALLOCATE(c_laland(ncol), c_laglac(ncol))
    
    ! Allocate surface flux arrays
    ALLOCATE(c_vis_dn_dir_sfc(ncol), c_par_dn_dir_sfc(ncol), c_nir_dn_dir_sfc(ncol))
    ALLOCATE(c_vis_dn_dff_sfc(ncol), c_par_dn_dff_sfc(ncol), c_nir_dn_dff_sfc(ncol))
    ALLOCATE(c_vis_up_sfc(ncol), c_par_up_sfc(ncol), c_nir_up_sfc(ncol))
    
    ! Convert boolean arrays
    DO i = 1, ncol
        c_laland(i) = laland(i)
        c_laglac(i) = laglac(i)
    END DO
    
    ! Convert 2D arrays to 1D (column-major flattening)
    DO j = 1, klev
        DO i = 1, ncol
            idx = i + (j-1)*ncol
            zf_1d(idx) = zf(i,j)
            dz_1d(idx) = dz(i,j)
            pp_fl_1d(idx) = pp_fl(i,j)
            tk_fl_1d(idx) = tk_fl(i,j)
            xvmr_vap_1d(idx) = xvmr_vap(i,j)
            xm_liq_1d(idx) = xm_liq(i,j)
            xm_ice_1d(idx) = xm_ice(i,j)
            reff_ice_1d(idx) = reff_ice(i,j)
            tau_ice_1d(idx) = tau_ice(i,j)
            reff_snow_1d(idx) = reff_snow(i,j)
            tau_snow_1d(idx) = tau_snow(i,j)
            cdnc_1d(idx) = cdnc(i,j)
            cld_frc_1d(idx) = cld_frc(i,j)
            xm_snw_1d(idx) = xm_snw(i,j)
            xvmr_co2_1d(idx) = xvmr_co2(i,j)
            xvmr_ch4_1d(idx) = xvmr_ch4(i,j)
            xvmr_n2o_1d(idx) = xvmr_n2o(i,j)
            xvmr_o3_1d(idx) = xvmr_o3(i,j)
            xvmr_o2_1d(idx) = xvmr_o2(i,j)
        END DO
    END DO
    
    ! Convert half-level arrays (klev+1)
    DO j = 1, klev+1
        DO i = 1, ncol
            idx = i + (j-1)*ncol
            zh_1d(idx) = zh(i,j)
            pp_hl_1d(idx) = pp_hl(i,j)
            tk_hl_1d(idx) = tk_hl(i,j)
        END DO
    END DO
    
    ! Convert 3D arrays (xvmr_cfc)
    DO k = 1, 2
        DO j = 1, klev
            DO i = 1, ncol
                idx = i + (j-1)*ncol + (k-1)*ncol*klev
                xvmr_cfc_1d(idx) = xvmr_cfc(i,j,k)
            END DO
        END DO
    END DO
    
    ! Convert 3D aerosol arrays
    DO k = 1, SIZE(aer_tau_lw, 3)
        DO j = 1, klev
            DO i = 1, ncol
                idx = i + (j-1)*ncol + (k-1)*ncol*klev
                aer_tau_lw_1d(idx) = aer_tau_lw(i,j,k)
            END DO
        END DO
    END DO
    
    DO k = 1, SIZE(aer_tau_sw, 3)
        DO j = 1, klev
            DO i = 1, ncol
                idx = i + (j-1)*ncol + (k-1)*ncol*klev
                aer_tau_sw_1d(idx) = aer_tau_sw(i,j,k)
                aer_ssa_sw_1d(idx) = aer_ssa_sw(i,j,k)
                aer_asy_sw_1d(idx) = aer_asy_sw(i,j,k)
            END DO
        END DO
    END DO
    
    ! Prepare flux arrays (initialize with current values)
    DO j = 1, klev+1
        DO i = 1, ncol
            idx = i + (j-1)*ncol
            flx_uplw_1d(idx) = flx_uplw(i,j)
            flx_uplw_clr_1d(idx) = flx_uplw_clr(i,j)
            flx_dnlw_1d(idx) = flx_dnlw(i,j)
            flx_dnlw_clr_1d(idx) = flx_dnlw_clr(i,j)
            flx_upsw_1d(idx) = flx_upsw(i,j)
            flx_upsw_clr_1d(idx) = flx_upsw_clr(i,j)
            flx_dnsw_1d(idx) = flx_dnsw(i,j)
            flx_dnsw_clr_1d(idx) = flx_dnsw_clr(i,j)
        END DO
    END DO
    
    ! Copy surface flux arrays
    c_vis_dn_dir_sfc = vis_dn_dir_sfc
    c_par_dn_dir_sfc = par_dn_dir_sfc
    c_nir_dn_dir_sfc = nir_dn_dir_sfc
    c_vis_dn_dff_sfc = vis_dn_dff_sfc
    c_par_dn_dff_sfc = par_dn_dff_sfc
    c_nir_dn_dff_sfc = nir_dn_dff_sfc
    c_vis_up_sfc = vis_up_sfc
    c_par_up_sfc = par_up_sfc
    c_nir_up_sfc = nir_up_sfc
    
    ! Call the C++ wrapper function with flattened arrays
    CALL rte_rrtmgp_interface_onblock_cpp( &
        c_lclrsky_lw, c_lclrsky_sw, c_inhom_lts, inhom_lts_max, &
        c_ncol, c_klev, psctm, ssi_factor, &
        c_laland, c_laglac, pcos_mu0, daylght_frc, &
        alb_vis_dir, alb_nir_dir, alb_vis_dif, alb_nir_dif, &
        emissivity, zf_1d, zh_1d, dz_1d, &
        pp_sfc, pp_fl_1d, pp_hl_1d, tk_sfc, tk_fl_1d, tk_hl_1d, &
        rad_2d, xvmr_vap_1d, xm_liq_1d, xm_ice_1d, reff_ice_1d, tau_ice_1d, &
        reff_snow_1d, tau_snow_1d, cdnc_1d, cld_frc_1d, xm_snw_1d, &
        xvmr_co2_1d, xvmr_ch4_1d, xvmr_n2o_1d, xvmr_cfc_1d, xvmr_o3_1d, xvmr_o2_1d, &
        aer_tau_lw_1d, aer_tau_sw_1d, aer_ssa_sw_1d, aer_asy_sw_1d, &
        flx_uplw_1d, flx_uplw_clr_1d, flx_dnlw_1d, flx_dnlw_clr_1d, &
        flx_upsw_1d, flx_upsw_clr_1d, flx_dnsw_1d, flx_dnsw_clr_1d, &
        c_vis_dn_dir_sfc, c_par_dn_dir_sfc, c_nir_dn_dir_sfc, &
        c_vis_dn_dff_sfc, c_par_dn_dff_sfc, c_nir_dn_dff_sfc, &
        c_vis_up_sfc, c_par_up_sfc, c_nir_up_sfc)
    
    ! Copy data from 1D arrays back to original arrays
    
    ! Copy optical depths back to 2D arrays
    DO j = 1, klev
        DO i = 1, ncol
            idx = i + (j-1)*ncol
            tau_ice(i,j) = tau_ice_1d(idx)
            tau_snow(i,j) = tau_snow_1d(idx)
        END DO
    END DO
    
    ! Copy fluxes back to 2D arrays
    DO j = 1, klev+1
        DO i = 1, ncol
            idx = i + (j-1)*ncol
            flx_uplw(i,j) = flx_uplw_1d(idx)
            flx_uplw_clr(i,j) = flx_uplw_clr_1d(idx)
            flx_dnlw(i,j) = flx_dnlw_1d(idx)
            flx_dnlw_clr(i,j) = flx_dnlw_clr_1d(idx)
            flx_upsw(i,j) = flx_upsw_1d(idx)
            flx_upsw_clr(i,j) = flx_upsw_clr_1d(idx)
            flx_dnsw(i,j) = flx_dnsw_1d(idx)
            flx_dnsw_clr(i,j) = flx_dnsw_clr_1d(idx)
        END DO
    END DO
    
    ! Copy surface fluxes back to original arrays
    vis_dn_dir_sfc = c_vis_dn_dir_sfc
    par_dn_dir_sfc = c_par_dn_dir_sfc
    nir_dn_dir_sfc = c_nir_dn_dir_sfc
    vis_dn_dff_sfc = c_vis_dn_dff_sfc
    par_dn_dff_sfc = c_par_dn_dff_sfc
    nir_dn_dff_sfc = c_nir_dn_dff_sfc
    vis_up_sfc = c_vis_up_sfc
    par_up_sfc = c_par_up_sfc
    nir_up_sfc = c_nir_up_sfc
    
    ! Deallocate temporary arrays
    DEALLOCATE(zf_1d, zh_1d, dz_1d)
    DEALLOCATE(pp_fl_1d, pp_hl_1d)
    DEALLOCATE(tk_fl_1d, tk_hl_1d)
    DEALLOCATE(xvmr_vap_1d, xm_liq_1d)
    DEALLOCATE(xm_ice_1d, reff_ice_1d)
    DEALLOCATE(tau_ice_1d)
    DEALLOCATE(reff_snow_1d, tau_snow_1d)
    DEALLOCATE(cdnc_1d, cld_frc_1d, xm_snw_1d)
    DEALLOCATE(xvmr_co2_1d, xvmr_ch4_1d, xvmr_n2o_1d)
    DEALLOCATE(xvmr_o3_1d, xvmr_o2_1d)
    DEALLOCATE(xvmr_cfc_1d)
    DEALLOCATE(aer_tau_lw_1d, aer_tau_sw_1d)
    DEALLOCATE(aer_ssa_sw_1d, aer_asy_sw_1d)
    DEALLOCATE(flx_uplw_1d, flx_uplw_clr_1d)
    DEALLOCATE(flx_dnlw_1d, flx_dnlw_clr_1d)
    DEALLOCATE(flx_upsw_1d, flx_upsw_clr_1d)
    DEALLOCATE(flx_dnsw_1d, flx_dnsw_clr_1d)
    DEALLOCATE(c_laland, c_laglac)
    DEALLOCATE(c_vis_dn_dir_sfc, c_par_dn_dir_sfc, c_nir_dn_dir_sfc)
    DEALLOCATE(c_vis_dn_dff_sfc, c_par_dn_dff_sfc, c_nir_dn_dff_sfc)
    DEALLOCATE(c_vis_up_sfc, c_par_up_sfc, c_nir_up_sfc)
    
  END SUBROUTINE rte_rrtmgp_interface_onBlock
  
END MODULE rte_rrtmgp_interface_mod
