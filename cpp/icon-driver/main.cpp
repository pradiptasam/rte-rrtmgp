#include <iostream>
#include <string>
#include <exception>
#include <Kokkos_Core.hpp>

// Include the netCDF header and any other necessary headers
#include <netcdf>
#include "read_data.hpp"
#include "rte_rrtmgp_interface.hpp"

// (Assume here that InputData, read_netcdf_to_dict, and RteRrtmgpInterface have been defined as in the previous example)

int main() {
  Kokkos::initialize();

  using DeviceT = Kokkos::DefaultExecutionSpace;
  using LayoutT = Kokkos::LayoutLeft;
  // using MDRP = conv::MDRP<LayoutT>;
  using real = double;
  using real1d_t = Kokkos::View<real*,   LayoutT, DeviceT>;
  using real2d_t = Kokkos::View<real**,  LayoutT, DeviceT>;
  using real3d_t = Kokkos::View<real***, LayoutT, DeviceT>;
  using bool2d_t = Kokkos::View<bool**,  LayoutT, DeviceT>;
  using hreal2d_t = Kokkos::View<real**, LayoutT, Kokkos::DefaultHostExecutionSpace>;
  {
    // Name of the NetCDF input file.
    // std::string filename = "rte_rrtmgp_interface_input.nc";
    std::string filename = "rte_rrtmgp_interface_input_dropsondes.nc";
    printf("filename: %s\n", filename.c_str());

    try {
      // Read data from the netCDF file.
      InputData data = read_netcdf_to_dict(filename);
      std::cout << "Data read from file: ncol = " << data.ncol << std::endl;

      // Create the interface object with the read data.
      RteRrtmgpInterface iface(data);

      // Call the on_block() method to process the data.
      iface.on_block();

      std::cout << "Test passed." << std::endl;
    } catch (std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      return 1;
    }

    return 0;
  }
  Kokkos::finalize();
}
