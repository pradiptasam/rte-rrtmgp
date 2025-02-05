# test_optical_props.py
import numpy as np
import sys

sys.path.insert(0, "/home/k/k202170/programs/rte-rrtmgp/cpp/py-bindings/build")
import rte_rrtmgp

# --- Using OpticalPropsK ---
opk = rte_rrtmgp.OpticalPropsK()
print("OpticalPropsK is initialized:", opk.is_initialized())
opk.set_name("BaseProps")
print("Name (OpticalPropsK):", opk.get_name())

# --- Using OpticalPropsArryK ---
op_arry = rte_rrtmgp.OpticalPropsArryK()
# For demonstration, assume get_ncol and get_nlay return 0 before allocation
print("OpticalPropsArryK: ncol =", op_arry.get_ncol(), "nlay =", op_arry.get_nlay())

# Create an instance of OpticalProps1sclK.
op1scl = rte_rrtmgp.OpticalProps1sclK()
op1scl.set_name("OneScalarProps")

# Create dummy NumPy arrays for the spectral discretization.
# For example, for 2 rows and 3 bands:
band_lims_wvn = np.array([[1.0, 2.0, 3.0],
                            [1.0, 2.0, 3.0]])
band_lims_gpt = np.array([[0, 1, 2],
                            [0, 1, 2]])

# Allocate optical properties using the raw array overload.
# Here, ncol and nlay are grid dimensions (e.g., 10 columns and 5 layers), and nbands is 3.
op1scl.alloc_1scl_from_array(10, 5, band_lims_wvn, band_lims_gpt, 3, "OneScalarProps")

# Validate and print norms.
op1scl.validate()
op1scl.print_norms(True)
print("OpticalProps1sclK name:", op1scl.get_name())

# Increment optical properties by itself as a test.
op1scl.increment(op1scl)

# --- Using OpticalProps2strK ---
op_2str = rte_rrtmgp.OpticalProps2strK()
op_2str.set_name("TwoStreamProps")
# Allocate two-stream arrays (tau, ssa, g) with the same grid dimensions.
op_2str.alloc_2str_from_array(10, 5, band_lims_wvn, band_lims_gpt, 3, "TwoStreamProps")
op_2str.validate()
op_2str.print_norms(True)
print("OpticalProps2strK name:", op_2str.get_name())

# Increment two-stream properties using a one-scalar object.
# op_2str.increment_from_1scl(op_1scl)
