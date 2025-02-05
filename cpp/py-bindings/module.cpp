// module.cpp
#include <pybind11/pybind11.h>
namespace py = pybind11;

// Forward declarations of the initialization functions.
void init_optical_props(py::module_ &m);
void init_gas_concs(py::module_ &m);

PYBIND11_MODULE(rte_rrtmgp, m) {
    m.doc() = "Combined Python bindings for optical properties and gas concentrations";
    
    // Call initialization functions from other translation units.
    init_optical_props(m);
    init_gas_concs(m);
}
