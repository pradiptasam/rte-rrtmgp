# Instruction to compile and run standalone radiation driver with ICON data

## Compilation

* Make sure that both `netcdf-c` and `netcdf-cxx4` packages are installed and loaded (in case installed using Spack). For MacBook, these packages could be installed using `brew`
* Create a build directory:
```bash
mkdir build
cd build
```
* Configure the code using `cmake`:
```bash
cmake -DRRTMGP_ICON_DRIVER=ON ..
```
* Compile the code using `make`
```bash
make -j 4
```

## Running the code
* The main executable is `icon_radiation` and is located inside `build/icon-driver` directory
* Following data files are needed to run the code:
  * `rrtmgp-cloud-optics-coeffs-lw.nc`: can be copied from `icon/data/rrtmgp-cloud-optics-coeffs-lw.nc`
  * `rrtmgp-cloud-optics-coeffs-sw.nc`: can be copied from `icon/data/rrtmgp-cloud-optics-coeffs-sw.nc`
  * `coefficients_lw.nc`: can be copied from `icon/data/rrtmgp-gas-lw-g128.nc`
  * `coefficients_sw.nc`: can be copied from `icon/data/rrtmgp-gas-sw-g112.nc`
  * `rte_rrtmgp_interface_input_dropsondes.nc`: This file is created by ICON and contains the input data for the radiation driver. This file can be copied from `/scratch/m/m300575/rte_rrtmgp_interface_input_dropsondes.nc`
* Copy the above files to the `build/icon-driver` directory and run the code there:
```bash
./icon_radiation 
```
