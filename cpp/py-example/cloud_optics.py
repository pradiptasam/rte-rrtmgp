# import numpy as np
from netCDF4 import Dataset

class CloudOpticsCoeffsReader:
    def __init__(self, cld_coeff_file):
        self.cld_coeff_file = cld_coeff_file
        self.nband = None
        self.nrghice = None
        self.nsize_liq = None
        self.nsize_ice = None

        self.band_lims_wvn = None
        self.radliq_lwr = None
        self.radliq_upr = None
        self.radice_lwr = None
        self.radice_upr = None

        self.lut_extliq = None
        self.lut_ssaliq = None
        self.lut_asyliq = None
        self.lut_extice = None
        self.lut_ssaice = None
        self.lut_asyice = None

    def load_coefficients(self):
        try:
            # Open the NetCDF file
            with Dataset(self.cld_coeff_file, mode='r') as nc:
                # Read LUT coefficient dimensions
                self.nband = nc.dimensions['nband'].size
                self.nrghice = nc.dimensions['nrghice'].size
                self.nsize_liq = nc.dimensions['nsize_liq'].size
                self.nsize_ice = nc.dimensions['nsize_ice'].size

                # Read band limits
                self.band_lims_wvn = nc.variables['bnd_limits_wavenumber'][:]

                # Read LUT constants
                self.radliq_lwr = nc.variables['radliq_lwr'][:].item()
                self.radliq_upr = nc.variables['radliq_upr'][:].item()
                self.radice_lwr = nc.variables['radice_lwr'][:].item()
                self.radice_upr = nc.variables['radice_upr'][:].item()

                # Allocate and read LUT coefficients
                self.lut_extliq = nc.variables['lut_extliq'][:]
                self.lut_ssaliq = nc.variables['lut_ssaliq'][:]
                self.lut_asyliq = nc.variables['lut_asyliq'][:]
                self.lut_extice = nc.variables['lut_extice'][:]
                self.lut_ssaice = nc.variables['lut_ssaice'][:]
                self.lut_asyice = nc.variables['lut_asyice'][:]

        except FileNotFoundError:
            raise RuntimeError(f"Cannot open file: {self.cld_coeff_file}")
        except KeyError as e:
            raise RuntimeError(f"Missing expected variable or dimension in the NetCDF file: {e}")
        except Exception as e:
            raise RuntimeError(f"Error reading NetCDF file: {e}")

    def print_summary(self):
        print(f"Number of bands (nband): {self.nband}")
        print(f"Number of ice radiative grid points (nrghice): {self.nrghice}")
        print(f"Number of liquid particle sizes (nsize_liq): {self.nsize_liq}")
        print(f"Number of ice particle sizes (nsize_ice): {self.nsize_ice}")
        print(f"Band limits (band_lims_wvn):\n{self.band_lims_wvn}")
        print(f"Liquid particle size lower bound: {self.radliq_lwr}")
        print(f"Liquid particle size upper bound: {self.radliq_upr}")
        print(f"Ice particle size lower bound: {self.radice_lwr}")
        print(f"Ice particle size upper bound: {self.radice_upr}")
        print("LUT coefficients loaded successfully.")
