// gas_concs_pybind.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>       // for STL conversions (e.g. std::vector<std::string>)
#include <pybind11/numpy.h>     // for NumPy array bindings
#include "mo_gas_concentrations.h"   // Header file that defines GasConcsK

namespace py = pybind11;

void init_gas_concs(py::module_ &m) {
  // Fix the template parameters for our bindings.
  // For example, we assume: RealT = double, LayoutT = Kokkos::LayoutLeft, DeviceT = DefaultDevice.
  using GasConcsK_t = GasConcsK<double, Kokkos::LayoutLeft, DefaultDevice>;

  if (!Kokkos::is_initialized()) {
    Kokkos::initialize();
  }

  // m.doc() = "Python bindings for the GasConcsK class";

  // Bind the GasConcsK class.
  py::class_<GasConcsK_t>(m, "GasConcsK")
    .def(py::init<>(), "Construct an empty GasConcsK object")
    .def("reset", &GasConcsK_t::reset, "Reset gas concentrations")
    // .def("init", &GasConcsK_t::init,
    //      py::arg("gas_names"), py::arg("ncol"), py::arg("nlay"),
    //      "Initialize gas concentrations with a list of gas names and grid dimensions")
    .def("init", &GasConcsK_t::init,
        py::arg("gas_names"), py::arg("ncol"), py::arg("nlay"),
        "Initialize gas concentrations with a list of gas names and grid dimensions")
    // Bind the scalar version of set_vmr.
    .def("set_vmr_scalar",
        (void (GasConcsK_t::*)(std::string, const double)) &GasConcsK_t::set_vmr,
        py::arg("gas"), py::arg("w"),
        "Set the VMR for a gas to a scalar value (applied to all columns and levels)")
    .def("get_num_gases", &GasConcsK_t::get_num_gases,
        "Return the number of gases")
    .def("get_gas_names", &GasConcsK_t::get_gas_names,
        "Return the list of gas names")
    .def("find_gas", &GasConcsK_t::find_gas,
        py::arg("gas"),
        "Return the index of the given gas in the list (or GAS_NOT_IN_LIST if not found)")
    .def("print_norms", &GasConcsK_t::print_norms,
        py::arg("print_prefix") = false,
        "Print summary norms for the gas concentration arrays")
    // Bind the alternative set_vmr overload that accepts a 1-D numpy array.
    .def("set_vmr_1d", [](GasConcsK_t &self, const std::string &gas, py::array_t<double> w) {
        py::buffer_info buf = w.request();
        if (buf.ndim != 1)
        throw std::runtime_error("set_vmr_1d: Expected a 1-D numpy array");
        // Create a 1-D Kokkos view from the raw pointer.
        using View1D = Kokkos::View<double*, Kokkos::LayoutLeft, Kokkos::HostSpace>;
        double* ptr = static_cast<double*>(buf.ptr);
        View1D view(ptr, buf.shape[0]);
        self.set_vmr(gas, view);
        }, "Set VMR using a 1-D numpy array", py::arg("gas"), py::arg("w"))
  // Bind the alternative set_vmr overload that accepts a 2-D numpy array.
  .def("set_vmr_2d", [](GasConcsK_t &self, const std::string &gas, py::array_t<double> w) {
      py::buffer_info buf = w.request();
      if (buf.ndim != 2)
      throw std::runtime_error("set_vmr_2d: Expected a 2-D numpy array");
      using View2D = Kokkos::View<double**, Kokkos::LayoutLeft, Kokkos::HostSpace>;
      double* ptr = static_cast<double*>(buf.ptr);
      View2D view(ptr, buf.shape[0], buf.shape[1]);
      self.set_vmr(gas, view);
      }, "Set VMR using a 2-D numpy array", py::arg("gas"), py::arg("w"))
  // Bind get_vmr. Here we create a new 2-D numpy array and pass its data to get_vmr.
  .def("get_vmr", [](const GasConcsK_t &self, const std::string &gas) -> py::array {
      int ncol = self.ncol;
      int nlay = self.nlay;
      // Allocate a numpy array of shape [ncol, nlay].
      py::array_t<double> result({ncol, nlay});
      py::buffer_info buf = result.request();
      using View2D = Kokkos::View<double**, Kokkos::LayoutLeft, Kokkos::HostSpace>;
      double* ptr = static_cast<double*>(buf.ptr);
      View2D view(ptr, ncol, nlay);
      self.get_vmr(gas, view);
      return result;
      }, "Get the VMR for a gas as a 2-D numpy array", py::arg("gas"))
  // Bind the static helper set_concs_impl2.
  // .def("set_concs_impl2", [](py::array_t<double> w, int nlay, int ncol, int igas, typename GasConcsK_t::real3d_t concs) {
  // .def("set_concs_impl2", [](py::array_t<double> w, int nlay, int ncol, int igas, py::array_t<double> concs) {
  //      py::buffer_info buf = w.request();
  //      py::buffer_info buf_conc = concs.request();
  //      if (buf.ndim != 2)
  //          throw std::runtime_error("set_concs_impl2: Expected a 2-D numpy array");
  //      if (buf_conc.ndim != 3)
  //          throw std::runtime_error("set_concs_impl2: Expected a 3-D numpy array");
  //      using View2D = Kokkos::View<double**, Kokkos::LayoutLeft, Kokkos::HostSpace>;
  //      using View3D = Kokkos::View<double***, Kokkos::LayoutLeft, Kokkos::HostSpace>;
  //      double* ptr = static_cast<double*>(buf.ptr);
  //      double* ptr_conc = static_cast<double*>(buf_conc.ptr);
  //      View2D view(ptr, buf.shape[0], buf.shape[1]);
  //      View3D view_conc(ptr_conc, buf_conc.shape[0], buf_conc.shape[1], buf_conc.shape[2]);
  //      GasConcsK_t::set_concs_impl2(view, nlay, ncol, igas, view_conc);
  // }, "Set concentrations using a 2-D numpy array (static helper)",
  //    py::arg("w"), py::arg("nlay"), py::arg("ncol"), py::arg("igas"), py::arg("concs"))
  ;
}
