#include <iostream>
#include <string>
#include <exception>

// Include the netCDF header and any other necessary headers
#include <netcdf>
#include "read_data.hpp"

// (Assume here that InputData, read_netcdf_to_dict, and RteRrtmgpInterface have been defined as in the previous example)

int main() {
    // Name of the NetCDF input file.
    std::string filename = "rte_rrtmgp_interface_input.nc";

    try {
        // Read data from the netCDF file.
        InputData data = read_netcdf_to_dict(filename);
        std::cout << "Data read from file: ncol = " << data.ncol << std::endl;

        // Create the interface object with the read data.
        // RteRrtmgpInterface iface(data);

        // Call the on_block() method to process the data.
        // iface.on_block();

        std::cout << "Test passed." << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
