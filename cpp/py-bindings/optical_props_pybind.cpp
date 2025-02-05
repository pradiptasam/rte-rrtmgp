// optical_props_pybind.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>       // for automatic STL conversions
#include <pybind11/functional.h>
#include <pybind11/numpy.h>     // if you later want to pass NumPy arrays
#include "mo_optical_props.h"    // header with all the template classes

namespace py = pybind11;

void init_optical_props(py::module_ &m) {

  // Create type aliases for the concrete instantiations (using default template parameters)
  using OpticalPropsK_t      = OpticalPropsK<double, Kokkos::LayoutLeft, DefaultDevice>;
  using OpticalPropsArryK_t  = OpticalPropsArryK<double, Kokkos::LayoutLeft, DefaultDevice>;
  using OpticalProps1sclK_t  = OpticalProps1sclK<double, Kokkos::LayoutLeft, DefaultDevice>;
  using OpticalProps2strK_t  = OpticalProps2strK<double, Kokkos::LayoutLeft, DefaultDevice>;


  if (!Kokkos::is_initialized()) {
    Kokkos::initialize();
  }

  // m.doc() = "Python bindings for optical properties classes using pybind11";

  // --- Bind OpticalPropsK (the base class) ---
  py::class_<OpticalPropsK_t>(m, "OpticalPropsK")
    .def(py::init<>())
    .def("is_initialized", &OpticalPropsK_t::is_initialized, "Check if the object is initialized")
    .def("finalize", &OpticalPropsK_t::finalize, "Finalize (deallocate) the object")
    .def("get_nband", &OpticalPropsK_t::get_nband, "Return the number of bands")
    .def("get_ngpt", &OpticalPropsK_t::get_ngpt, "Return the number of g-points")
    .def("get_gpoint_bands", &OpticalPropsK_t::get_gpoint_bands, "Get mapping from g-points to bands")
    .def("get_band_lims_gpoint", &OpticalPropsK_t::get_band_lims_gpoint, "Get band limits in g-points")
    .def("get_band_lims_wavenumber", &OpticalPropsK_t::get_band_lims_wavenumber, "Get band limits in wavenumber")
    // .def("get_band_lims_wavelength", &OpticalPropsK_t::get_band_lims_wavelength, "Get band limits in wavelength")
    .def("bands_are_equal", &OpticalPropsK_t::bands_are_equal, "Compare band structures")
    .def("gpoints_are_equal", &OpticalPropsK_t::gpoints_are_equal, "Compare g-point mappings")
    .def("set_name", &OpticalPropsK_t::set_name, "Set the object name")
    .def("get_name", &OpticalPropsK_t::get_name, "Get the object name")
    ;

  // --- Bind OpticalPropsArryK (inherits from OpticalPropsK) ---
  py::class_<OpticalPropsArryK_t, OpticalPropsK_t>(m, "OpticalPropsArryK")
    .def(py::init<>())
    .def("get_ncol", &OpticalPropsArryK_t::get_ncol, "Return the number of columns")
    .def("get_nlay", &OpticalPropsArryK_t::get_nlay, "Return the number of layers")
    ;

  // --- Bind OpticalProps1sclK (inherits from OpticalPropsArryK) ---
  py::class_<OpticalProps1sclK_t, OpticalPropsArryK_t>(m, "OpticalProps1sclK")
    .def(py::init<>())
    .def("validate", &OpticalProps1sclK_t::validate, "Validate the one-scalar optical properties")
    // Bind the simplest alloc method taking ncol and nlay:
    .def("alloc_1scl", 
        (void (OpticalProps1sclK_t::*)(int, int)) &OpticalProps1sclK_t::alloc_1scl,
        "Allocate tau (optical depth) with given ncol and nlay",
        py::arg("ncol"), py::arg("nlay"))
    // Bind other methods as needed...
    // Bind the new overload that accepts normal arrays (NumPy arrays)
    .def("alloc_1scl_from_array",
        [](OpticalProps1sclK_t &self, int ncol, int nlay,
          py::array_t<double> band_lims_wvn,
          py::array_t<int> band_lims_gpt,
          int nbands, std::string name) {
        // Request buffer information for both arrays.
        py::buffer_info buf_wvn = band_lims_wvn.request();
        py::buffer_info buf_gpt = band_lims_gpt.request();

        // Check that the arrays have the expected dimensions.
        if (buf_wvn.ndim != 2 || buf_wvn.shape[0] != 2 || buf_wvn.shape[1] != static_cast<ssize_t>(nbands))
        throw std::runtime_error("band_lims_wvn must be a 2 x nbands array of doubles");
        if (buf_gpt.ndim != 2 || buf_gpt.shape[0] != 2 || buf_gpt.shape[1] != static_cast<ssize_t>(nbands))
        throw std::runtime_error("band_lims_gpt must be a 2 x nbands array of ints");

        // **Important Change:** Create non-const views by casting the raw pointer appropriately.
        using ViewDouble2D = Kokkos::View<double**, Kokkos::LayoutLeft, Kokkos::HostSpace>;
        using ViewInt2D    = Kokkos::View<int**, Kokkos::LayoutLeft, Kokkos::HostSpace>;

        double* ptr_wvn = static_cast<double*>(buf_wvn.ptr);
        int*    ptr_gpt = static_cast<int*>(buf_gpt.ptr);

        // Construct non-const views.
        ViewDouble2D band_lims_wvn_view(ptr_wvn, 2, nbands);
        ViewInt2D    band_lims_gpt_view(ptr_gpt, 2, nbands);

        // Optionally, if your original function requires the views to be in the device space,
        // create mirror views and copy data to the device.
        auto device_band_lims_wvn = Kokkos::create_mirror_view_and_copy(DefaultDevice(), band_lims_wvn_view);
        auto device_band_lims_gpt = Kokkos::create_mirror_view_and_copy(DefaultDevice(), band_lims_gpt_view);

        // Call the overload that takes Kokkos views.
        self.alloc_1scl(ncol, nlay, device_band_lims_wvn, device_band_lims_gpt, name);
        },
          "Allocate tau using raw arrays for spectral discretization",
          py::arg("ncol"),
          py::arg("nlay"),
          py::arg("band_lims_wvn"),
          py::arg("band_lims_gpt"),
          py::arg("nbands"),
          py::arg("name") = ""
            )
            // Bind the overload that also accepts spectral discretization info.
            // Note: Converting Python objects (e.g. NumPy arrays) to Kokkos views requires custom wrappers.
            // .def("alloc_1scl_with_bands",
            //      [](OpticalProps1sclK_t &self, int ncol, int nlay, py::object band_lims_wvn, 
            //          py::object band_lims_gpt, std::string name) {
            //          // In a full binding, convert band_lims_wvn/gbpt (likely NumPy arrays)
            //          // to the corresponding Kokkos::View types.
            //          // For demonstration we assume these conversions are defined.
            //          self.alloc_1scl(ncol, nlay,
            //              band_lims_wvn.cast<decltype(band_lims_wvn)&>(),
            //              band_lims_gpt.cast<decltype(band_lims_gpt)&>(), name);
            //      },
            //      "Allocate tau with spectral discretization (band limits) info",
            //      py::arg("ncol"), py::arg("nlay"), py::arg("band_lims_wvn"), py::arg("band_lims_gpt") = py::none(), py::arg("name") = "")
            // // Bind the version that uses another OpticalPropsK object.
            // .def("alloc_1scl_from_op",
            //      (void (OpticalProps1sclK_t::*)(int, int, OpticalPropsK_t const&, std::string))
            //      &OpticalProps1sclK_t::alloc_1scl,
            //      "Allocate tau using spectral discretization from an existing OpticalPropsK",
            //      py::arg("ncol"), py::arg("nlay"), py::arg("opIn"), py::arg("name") = "")
            .def("increment",
                (void (OpticalProps1sclK_t::*)(OpticalProps1sclK_t&)) &OpticalProps1sclK_t::increment,
                "Increment (accumulate) optical depth from another OpticalProps1sclK object")
            // .def("increment", &OpticalProps1sclK_t::increment, "Increment (accumulate) optical depth from another object")
            .def("print_norms", &OpticalProps1sclK_t::print_norms, "Print summary norms", py::arg("print_prefix") = false)
            .def("set_name", &OpticalProps1sclK_t::set_name, "Set the object name")
            .def("get_name", &OpticalProps1sclK_t::get_name, "Get the object name")
            ;
  //
  // --- Bind OpticalProps2strK (inherits from OpticalPropsArryK) ---
  py::class_<OpticalProps2strK_t, OpticalPropsArryK_t>(m, "OpticalProps2strK")
    .def(py::init<>())
    .def("validate", &OpticalProps2strK_t::validate, "Validate two-stream optical properties")
    // .def("delta_scale", &OpticalProps2strK_t::delta_scale, "Apply delta scaling", py::arg("forward") = py::none())
    // Bind the simplest allocation (ncol and nlay)
    .def("alloc_2str",
        (void (OpticalProps2strK_t::*)(int, int)) &OpticalProps2strK_t::alloc_2str,
        "Allocate two-stream arrays (tau, ssa, g) with given ncol and nlay",
        py::arg("ncol"), py::arg("nlay"))
    // Bind the overload with spectral discretization info.
    // .def("alloc_2str_with_bands",
    //      [](OpticalProps2strK_t &self, int ncol, int nlay, py::object band_lims_wvn,
    //         py::object band_lims_gpt, std::string name) {
    //          self.alloc_2str(ncol, nlay,
    //              band_lims_wvn.cast<decltype(band_lims_wvn)&>(),
    //              band_lims_gpt.cast<decltype(band_lims_gpt)&>(), name);
    //      },
    //      "Allocate two-stream arrays with spectral discretization info",
    //      py::arg("ncol"), py::arg("nlay"), py::arg("band_lims_wvn"), py::arg("band_lims_gpt") = py::none(), py::arg("name") = "")
    // Bind the allocation version that uses an existing OpticalPropsK object.
    // .def("alloc_2str_from_op",
    //      (void (OpticalProps2strK_t::*)(int, int, OpticalPropsK_t const&, std::string))
    //      &OpticalProps2strK_t::alloc_2str,
    //      "Allocate two-stream arrays using spectral discretization from an existing OpticalPropsK",
    //      py::arg("ncol"), py::arg("nlay"), py::arg("opIn"), py::arg("name") = "")
    .def("alloc_2str_from_array",
        [](OpticalProps2strK_t &self, int ncol, int nlay,
          py::array_t<double> band_lims_wvn,
          py::array_t<int> band_lims_gpt,
          int nbands, std::string name) {
        // Request buffer info for each NumPy array.
        py::buffer_info buf_wvn = band_lims_wvn.request();
        py::buffer_info buf_gpt = band_lims_gpt.request();

        // Check that the arrays have exactly 2 dimensions, with shape [2, nbands].
        if (buf_wvn.ndim != 2 || buf_wvn.shape[0] != 2 || buf_wvn.shape[1] != static_cast<ssize_t>(nbands))
        throw std::runtime_error("band_lims_wvn must be a 2 x nbands array of doubles");
        if (buf_gpt.ndim != 2 || buf_gpt.shape[0] != 2 || buf_gpt.shape[1] != static_cast<ssize_t>(nbands))
        throw std::runtime_error("band_lims_gpt must be a 2 x nbands array of ints");

        // Create non-const pointers from the buffers.
        double* ptr_wvn = static_cast<double*>(buf_wvn.ptr);
        int*    ptr_gpt = static_cast<int*>(buf_gpt.ptr);

        // Create non-const Kokkos views wrapping the raw arrays.
        using ViewDouble2D = Kokkos::View<double**, Kokkos::LayoutLeft, Kokkos::HostSpace>;
        using ViewInt2D    = Kokkos::View<int**,    Kokkos::LayoutLeft, Kokkos::HostSpace>;
        ViewDouble2D band_lims_wvn_view(ptr_wvn, 2, nbands);
        ViewInt2D    band_lims_gpt_view(ptr_gpt, 2, nbands);

        // Create device mirror views (copying the data to the device space).
        auto device_band_lims_wvn = Kokkos::create_mirror_view_and_copy(DefaultDevice(), band_lims_wvn_view);
        auto device_band_lims_gpt = Kokkos::create_mirror_view_and_copy(DefaultDevice(), band_lims_gpt_view);

        // Call the member function that accepts Kokkos views.
        self.alloc_2str(ncol, nlay, device_band_lims_wvn, device_band_lims_gpt, name);
        },
          "Allocate two-stream arrays (tau, ssa, g) using raw arrays for spectral discretization",
          py::arg("ncol"), py::arg("nlay"),
          py::arg("band_lims_wvn"),
          py::arg("band_lims_gpt"),
          py::arg("nbands"),
          py::arg("name") = ""
            )
            .def("increment_from_2str",
                (void (OpticalProps2strK_t::*)(OpticalProps2strK_t&)) &OpticalProps2strK_t::increment,
                "Increment two-stream optical properties from another two-stream object")
            // .def("increment_from_1scl",
            //      (void (OpticalProps2strK_t::*)(OpticalProps1sclK_t&)) &OpticalProps2strK_t::increment,
            //      "Increment two-stream optical properties from a one-scalar object")
            .def("print_norms", &OpticalProps2strK_t::print_norms, "Print summary norms", py::arg("print_prefix") = false)
            .def("set_name", &OpticalProps2strK_t::set_name, "Set the object name")
            .def("get_name", &OpticalProps2strK_t::get_name, "Get the object name")
            ;
}
