import json
import numpy as np
from init_rad import init_cloud_optics
import rte_rrtmgp
from netCDF4 import Dataset

class rte_rrtmgp_interface:

    # def __init__(self, ncol, klev, psctm, lclrsky_lw, lclrsky_sw, inhom_lts, inhom_lts_max, laland, laglac, **kwargs):
    # def __init__(self, ncol, klev):
    def __init__(self, **kwargs):
        """
        Initialize the interface with required dimensions and input parameters.

        Parameters:
        - ncol: Number of columns
        - klev: Number of vertical levels
        - psctm: Orbit and time dependent solar constant for radiation time
        - lclrsky_lw: Flag for clear sky longwave radiation
        - lclrsky_sw: Flag for clear sky shortwave radiation
        - inhom_lts: Flag for inhomogeneous surface temperature
        - inhom_lts_max: Maximum value on inhoml
        - laland: Land sea mask
        - laglac: Glacier mask
        - kwargs: Other input parameters to match Fortran subroutine arguments
        """
        self.ncol = kwargs.get("ncol", 0)
        self.klev = kwargs.get("nlay", 0)

        # self.psctm = psctm # orbit and time dependent solar constant for radiation time
        # self.lclrsky_lw = lclrsky_lw # flag for clear sky longwave radiation
        # self.lclrsky_sw = lclrsky_sw # flag for clear sky shortwave radiation
        # self.inhom_lts = inhom_lts # flag for inhomogeneous surface temperature
        # self.inhom_lts_max = inhom_lts_max # maximum value on inhoml
        self.laland = np.array(kwargs.get("laland", np.zeros(self.ncol))) # land sea mask
        self.laglac = np.array(kwargs.get("laglac", np.zeros(self.ncol))) # glacier mask

        # Initialize input and output arrays (simplified structure)
        # self.tlay = np.zeros((ncol, klev))  # Temperature layer
        # self.play = np.zeros((ncol, klev))  # Pressure layer
        # self.albdir = np.zeros((nbndsw, ncol))  # Direct albedo
        # self.albdif = np.zeros((nbndsw, ncol))  # Diffuse albedo
        # self.flux_up_lw = np.zeros((ncol, klev + 1))  # Upward LW flux
        # self.flux_dn_lw = np.zeros((ncol, klev + 1))  # Downward LW flux
        # self.flux_up_sw = np.zeros((ncol, klev + 1))  # Upward SW flux
        # self.flux_dn_sw = np.zeros((ncol, klev + 1))  # Downward SW flux

        self.pcos_mu0 = kwargs.get("pcos_mu0", np.zeros(self.ncol)) # mu0 for solar zenith angle
        self.daylight_frc = kwargs.get("daylight_frc", np.zeros(self.ncol)) # daylight fraction; with diurnal cycle 0 or 1, with zonal mean in [0,1]
        self.alb_vis_dir = kwargs.get("alb_vis_dir", np.zeros(self.ncol)) # surface albedo for vis range and dir light
        self.alb_nir_dir = kwargs.get("alb_nir_dir", np.zeros(self.ncol)) # surface albedo for NIR range and dir light
        self.alb_vis_dif = kwargs.get("alb_vis_dif", np.zeros(self.ncol)) # surface albedo for vis range and dif light
        self.alb_nir_dif = kwargs.get("alb_nir_dif", np.zeros(self.ncol)) # surface albedo for NIR range and dif light
        self.emissivity = kwargs.get("emissivity", np.zeros(self.ncol)) # surface longwave emissivity

        self.zf = kwargs.get("zf", np.zeros((self.ncol, self.klev))) # geometric height at full level in m
        self.zh = kwargs.get("zh", np.zeros((self.ncol, self.klev + 1))) # geometric height at half level in m
        self.dz = np.array(kwargs.get("dz", np.zeros((self.ncol, self.klev)))) # geometric height thickness in m
        self.pp_sfc = kwargs.get("pp_sfc", np.zeros(self.ncol)) # surface pressure in Pa
        self.pp_fl = kwargs.get("pp_fl", np.zeros((self.ncol, self.klev))) # full level pressure in Pa
        self.pp_hl = kwargs.get("pp_hl", np.zeros((self.ncol, self.klev + 1))) # half level pressure in Pa
        self.tk_sfc = kwargs.get("tk_sfc", np.zeros(self.ncol)) # surface temperature in K
        self.tk_fl = kwargs.get("tk_fl", np.zeros((self.ncol, self.klev))) # full level temperature in K
        self.tk_hl = kwargs.get("tk_hl", np.zeros((self.ncol, self.klev + 1))) # half level temperature in K
        self.xvmr_vap = kwargs.get("xvmr_vap", np.zeros((self.ncol, self.klev))) # water vapor volume mixing ratio
        self.xm_liq = np.array(kwargs.get("xm_liq", np.zeros((self.ncol, self.klev)))) # cloud liquid mass in kg/m2
        self.xm_ice = np.array(kwargs.get("xm_ice", np.zeros((self.ncol, self.klev)))) # cloud ice   mass in kg/m2
        self.xm_snw = np.array(kwargs.get("xm_snw", np.zeros((self.ncol, self.klev)))) # snow mass in kg/m2
        # self.aer_tau_lw = kwargs.get("aer_tau_lw", np.zeros((self.ncol, self.klev, self.nbndlw))) # aerosol optical depth, longwave (ncol, nlay, nbndlw)
        # self.aer_tau_sw = kwargs.get("aer_tau_sw", np.zeros((self.ncol, self.klev, self.nbndlw))) # aerosol optical depth, shortwave (ncol, nlay, nbndlw)
        # self.aer_ssa_sw = kwargs.get("aer_ssa_sw", np.zeros((self.ncol, self.klev, self.nbndlw))) # aerosol single-scattering albedo, shortwave (ncol, nlay, nbndlw)
        # self.aer_asy_sw = kwargs.get("aer_asy_sw", np.zeros((self.ncol, self.klev, self.nbndlw))) # aerosol asymetry parameter, shortwave (ncol, nlay, nbndlw
        self.cdnc = np.array(kwargs.get("cdnc", np.zeros((self.ncol, self.klev)))) # cloud droplet number concentration
        self.cld_frc = np.array(kwargs.get("cld_frc", np.zeros((self.ncol, self.klev)))) # cloud fraction
        self.xvmr_co2 = kwargs.get("xvmr_co2", np.zeros((self.ncol, self.klev))) # co2 volume mixing ratio
        self.xvmr_ch4 = kwargs.get("xvmr_ch4", np.zeros((self.ncol, self.klev))) # ch4 volume mixing ratio
        self.xvmr_n2o = kwargs.get("xvmr_n2o", np.zeros((self.ncol, self.klev))) # n2o volume mixing ratio
        self.xvmr_cfc = kwargs.get("xvmr_cfc", np.zeros((self.ncol, self.klev, 2))) # cfc volume mixing ratio (kbdim,klev,2)
        self.xvmr_o3 = kwargs.get("xvmr_o3", np.zeros((self.ncol, self.klev))) # o3  volume mixing ratio
        self.xvmr_o2 = kwargs.get("xvmr_o2", np.zeros((self.ncol, self.klev))) # o2  volume mixing ratio
        self.reff_ice = np.array(kwargs.get("reff_ice", np.zeros((self.ncol, self.klev)))) # ice effectiv radius m
        self.reff_snow = np.array(kwargs.get("reff_snow", np.zeros((self.ncol, self.klev)))) # snow effectiv radius m

        # self.tau_ice = kwargs.get("tau_ice", np.zeros((self.ncol, self.klev))) # optical depth of cloud ice integrated over bands
        # self.tau_snow = kwargs.get("tau_snow", np.zeros((self.ncol, self.klev))) # optical depth of snow integrated over bands
        # self.rad_2d = kwargs.get("rad_2d", np.zeros(self.ncol)) # arbitrary 2d-field in radiation for output
        # self.flx_uplw = kwargs.get("flx_uplw", np.zeros((self.ncol, self.klev + 1))) # upward LW flux profile, all sky
        # self.flx_uplw_clr = kwargs.get("flx_uplw_clr", np.zeros((self.ncol, self.klev + 1))) # upward LW flux profile, clear sky
        # self.flx_dnlw = kwargs.get("flx_dnlw", np.zeros((self.ncol, self.klev + 1))) # downward LW flux profile, all sky
        # self.flx_dnlw_clr = kwargs.get("flx_dnlw_clr", np.zeros((self.ncol, self.klev + 1))) # downward LW flux profile, clear sky
        # self.flx_upsw = kwargs.get("flx_upsw", np.zeros((self.ncol, self.klev + 1))) # upward SW flux profile, all sky
        # self.flx_upsw_clr = kwargs.get("flx_upsw_clr", np.zeros((self.ncol, self.klev + 1))) # upward SW flux profile, clear sky
        # self.flx_dnsw = kwargs.get("flx_dnsw", np.zeros((self.ncol, self.klev + 1))) # downward SW flux profile, all sky
        # self.flx_dnsw_clr = kwargs.get("flx_dnsw_clr", np.zeros((self.ncol, self.klev + 1))) # downward SW flux profile, clear sky

        # self.vis_dn_dir_sfc = kwargs.get("vis_dn_dir_sfc", np.zeros(self.ncol)) # Diffuse downward flux surface visible radiation
        # self.par_dn_dir_sfc = kwargs.get("par_dn_dir_sfc", np.zeros(self.ncol)) # Diffuse downward flux surface PAR
        # self.nir_dn_dir_sfc = kwargs.get("nir_dn_dir_sfc", np.zeros(self.ncol)) # Diffuse downward flux surface near-infrared radiation
        # self.vis_dn_dff_sfc = kwargs.get("vis_dn_dff_sfc", np.zeros(self.ncol)) # Direct  downward flux surface visible radiation
        # self.par_dn_dff_sfc = kwargs.get("par_dn_dff_sfc", np.zeros(self.ncol)) # Direct  downward flux surface PAR
        # self.nir_dn_dff_sfc = kwargs.get("nir_dn_dff_sfc", np.zeros(self.ncol)) # Direct  downward flux surface near-infrared radiation
        # self.vis_up_sfc = kwargs.get("vis_up_sfc", np.zeros(self.ncol)) # Upward  flux surface visible radiation
        # self.par_up_sfc = kwargs.get("par_up_sfc", np.zeros(self.ncol)) # Upward  flux surface PAR
        # self.nir_up_sfc = kwargs.get("nir_up_sfc", np.zeros(self.ncol)) # Upward  flux surface near-infrared radiation

        # Constants
        self.ccwmin = 1e-7       # Minimum condensate for LW cloud opacity
        self.zkap_cont = 1.143   # Continental (Martin et al.) breadth parameter
        self.zkap_mrtm = 1.077   # Maritime (Martin et al.) breadth parameter
        self.del1 = 2.0          # Transition factor for inhomogeneity stability scaling
        self.del2 = 20.0         # Cut-over point for inhomogeneity stability scaling

        # Compute cld_frc_thresh using double precision
        self.cld_frc_thresh = 4.0 * np.spacing(1.0)

        self.droplet_scale = 1.0e2
        self.rhoh2o = 1.0e3
        self.effective_radius = 1.0e6 * self.droplet_scale * (3.0e-9 / (4.0 * np.pi * self.rhoh2o))**(1.0/3.0)

    def on_block(self):
        """
        Arranges input and calls RRTM SW and LW routines.

        Remarks:
            Because the RRTM indexes vertical levels differently than ECHAM,
            a chief function of this routine is to reorder the input in the vertical.
            In addition, some cloud physical properties are prescribed, which are
            required to derive cloud optical properties.
        """
        print("Number of columns: ", self.ncol)
        print("Number of vertical levels: ", self.klev)
        print("Land sea mask: ", self.laland)
        print("Glacier mask: ", self.laglac)
        print("Cloud water mass: ", self.xm_liq)
        print("Cloud ice mass: ", self.xm_ice)
        print("Snow mass: ", self.xm_snw)
        print("Cloud fraction: ", self.cld_frc)

        # Initializing the the cloud optical properties
        cloud_optics_lw, cloud_optics_sw = init_cloud_optics()

        reimin = max(cloud_optics_lw.radice_lwr, cloud_optics_sw.radice_lwr)
        reimax = min(cloud_optics_lw.radice_upr, cloud_optics_sw.radice_upr)

        relmin = max(cloud_optics_lw.radliq_lwr, cloud_optics_sw.radliq_lwr)
        relmax = min(cloud_optics_lw.radliq_upr, cloud_optics_sw.radliq_upr)

        # Initialize output arrays
        ziwp = np.zeros((self.ncol, self.klev))
        zlwp = np.zeros((self.ncol, self.klev))
        zswp = np.zeros((self.ncol, self.klev))
        tau_ice = np.zeros((self.ncol, self.klev))
        tau_snow = np.zeros((self.ncol, self.klev))
        zdwp = np.zeros((self.ncol, self.klev))
        re_cryst = np.full((self.ncol, self.klev), reimin)
        re_drop = np.full((self.ncol, self.klev), relmin)
        re_snow = np.full((self.ncol, self.klev), reimin)
 
        for jl in range(self.ncol):
            for jk in range(self.klev):
                print("Processing column ", jl, " and level ", jk, self.xm_liq[jl][jk], self.xm_ice[jl][jk], self.xm_snw[jl][jk])

        # Process each level and column
        for jk in range(self.klev):
            for jl in range(self.ncol):
                # Local calculations
                print("Processing level ", jk, " and column ", jl)
                cld_frc_loc = max(np.finfo(float).eps, self.cld_frc[jl, jk])
                ziwp[jl, jk] = self.xm_ice[jl, jk] * 1000.0 / cld_frc_loc
                zlwp[jl, jk] = self.xm_liq[jl, jk] * 1000.0 / cld_frc_loc
                zswp[jl, jk] = self.xm_snw[jl, jk] * 1000.0
 
                lcldlyr = self.cld_frc[jl, jk] > self.cld_frc_thresh
 
                if not lcldlyr:
                    ziwp[jl, jk] = 0.0
                    zlwp[jl, jk] = 0.0
 
                zlwc = zlwp[jl, jk] / self.dz[jl, jk] if self.dz[jl, jk] > 0 else 0.0
 
                if lcldlyr and (zlwp[jl, jk] + ziwp[jl, jk]) > self.ccwmin:
                    zkap = self.zkap_mrtm
                    if self.laland[jl] and not self.laglac[jl]:
                        zkap = self.zkap_cont
                    re_cryst[jl, jk] = max(reimin, min(reimax, 1.0e6 * self.reff_ice[jl, jk]))
                    re_drop[jl, jk] = max(relmin, min(relmax, self.effective_radius * zkap * (zlwc / self.cdnc[jl, jk])**(1.0 / 3.0) if self.cdnc[jl, jk] > 0 else 0.0))
                else:
                    re_cryst[jl, jk] = reimin
                    re_drop[jl, jk] = relmin
 
                if zswp[jl, jk] > self.ccwmin:
                    re_snow[jl, jk] = max(reimin, min(reimax, 1.0e6 * self.reff_snow[jl, jk]))
                else:
                    re_snow[jl, jk] = reimin
 
            pass
        
        gas_obj = rte_rrtmgp.GasConcsK()

        gas_names = ["h2o", "co2", "ch4", "o2", "o3", "n2o", "cfc11", "cfc12"]
        gas_obj.init(gas_names, self.ncol, self.klev)
        gas_obj.set_vmr_2d("h2o", self.xvmr_vap)
        gas_obj.set_vmr_2d("co2", self.xvmr_co2)
        gas_obj.set_vmr_2d("ch4", self.xvmr_co2)
        gas_obj.set_vmr_2d("o2", self.xvmr_co2)
        gas_obj.set_vmr_2d("o3", self.xvmr_co2)
        gas_obj.set_vmr_2d("n2o", self.xvmr_co2)
        gas_obj.set_vmr_2d("cfc11", self.xvmr_co2)
        gas_obj.set_vmr_2d("cfc12", self.xvmr_co2)

        # vmr = gas_obj.get_vmr("co2")
        # print("VMR for CO2:", vmr)

def read_netcdf_to_dict(nc_file):
    """
    Reads data from a NetCDF file and returns a dictionary suitable for passing as arguments.
    """
    with Dataset(nc_file, "r") as nc:
        # Read scalar values (stored as size-1 arrays, so extract the first element)
        ncol = nc.dimensions["ncol"].size
        klev = nc.dimensions["nlay"].size

        # Read matrix variables and convert them to NumPy arrays
        dz = np.array(nc.variables["dz"][:])  # Read full 2D array
        zh = np.array(nc.variables["zh"][:])  # Read full 2D array
        zf = np.array(nc.variables["zf"][:])  # Read full 2D array
        pp_hl = np.array(nc.variables["pp_hl"][:])  # Read full 2D array
        pp_fl = np.array(nc.variables["pp_fl"][:])  # Read full 2D array
        pp_sfc = np.array(nc.variables["pp_sfc"][:]) # Read full 1D array
        tk_hl = np.array(nc.variables["tk_hl"][:])  # Read full 2D array
        tk_fl = np.array(nc.variables["tk_fl"][:]) # Read full 2D array
        tk_sfc = np.array(nc.variables["tk_sfc"][:]) # Read full 1D array
        # xvmr_cfc = np.array(nc.variables["xvmr_cfc"][:])  # Read full 3D array
        # xvmr_vap = np.array(nc.variables["xvmr_vap"][:]) # Read full 2D array
        # xvmr_co2 = np.array(nc.variables["xvmr_co2"][:]) # Read full 2D array
        xm_liq = np.array(nc.variables["xm_liq"][:]) # Read full 2D array
        xm_ice = np.array(nc.variables["xm_ice"][:]) # Read full 2D array
        xm_snw = np.array(nc.variables["xm_snw"][:]) # Read full 2D array
        cdnc = np.array(nc.variables["cdnc"][:]) # Read full 2D array
        cld_frc = np.array(nc.variables["cld_frc"][:]) # Read full 2D array
        reff_ice = np.array(nc.variables["reff_ice"][:]) # Read full 2D array
        reff_snow = np.array(nc.variables["reff_snow"][:]) # Read full 2D array
        pcos_mu0 = np.array(nc.variables["pcos_mu0"][:]) # Read full 1D array
        daylight_frc = np.array(nc.variables["daylght_frc"][:]) # Read full 1D array
        alb_vis_dir = np.array(nc.variables["alb_vis_dir"][:]) # Read full 1D array
        alb_nir_dir = np.array(nc.variables["alb_nir_dir"][:]) # Read full 1D array
        alb_vis_dif = np.array(nc.variables["alb_vis_dif"][:]) # Read full 1D array
        alb_nir_dif = np.array(nc.variables["alb_nir_dif"][:]) # Read full 1D array
        emissivity = np.array(nc.variables["emissivity"][:]) # Read full 1D array

    return {
        "ncol": ncol,
        "klev": klev,
        "dz": dz,
        "zh": zh,
        "zf": zf,
        "pp_hl": pp_hl,
        "pp_fl": pp_fl,
        "pp_sfc": pp_sfc,
        "tk_hl": tk_hl,
        "tk_fl": tk_fl,
        "tk_sfc": tk_sfc,
        # "xvmr_cfc": xvmr_cfc,
        # "xvmr_vap": xvmr_vap,
        # "xvmr_co2": xvmr_co2,
        "xm_liq": xm_liq,
        "xm_ice": xm_ice,
        "xm_snw": xm_snw,
        "cdnc": cdnc,
        "cld_frc": cld_frc,
        "reff_ice": reff_ice,
        "reff_snow": reff_snow,
        "pcos_mu0": pcos_mu0,
        "daylight_frc": daylight_frc,
        "alb_vis_dir": alb_vis_dir,
        "alb_nir_dir": alb_nir_dir,
        "alb_vis_dif": alb_vis_dif,
        "alb_nir_dif": alb_nir_dif,
        "emissivity": emissivity
    }

if __name__ == "__main__":
    # Test the interface
    # open the icon_rad_driver.json file and read from there
    # with open("icon_rad_driver.json", "r") as f:
    #     data = json.load(f)

    # Read data from output.nc instead of JSON
    filename = "rte_rrtmgp_interface_input.nc"
    data = read_netcdf_to_dict(filename)

    print("Data read from file:")
    print(data["ncol"])

    # for key in data:
    #     values = data[key]

    interface = rte_rrtmgp_interface(**data)

    interface.on_block()

    print("Test passed.")
