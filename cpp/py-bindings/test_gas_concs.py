import numpy as np
import sys

sys.path.insert(0, "/home/k/k202170/programs/rte-rrtmgp/cpp/py-bindings/build")
import rte_rrtmgp

# Create a GasConcsK object.
gas_obj = rte_rrtmgp.GasConcsK()

# Initialize with gas names and grid dimensions.
gas_names = ["CO2", "O3", "H2O"]
# gas_names = "CO2"
gas_obj.init(gas_names, 10, 5)

# Set VMR for a gas using a scalar.
gas_obj.set_vmr_scalar("CO2", 0.15)

# Set VMR using a 1-D numpy array.
w_1d = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
gas_obj.set_vmr_1d("O3", w_1d)

# Set VMR using a 2-D numpy array.
w_2d = np.random.rand(10, 5)
gas_obj.set_vmr_2d("H2O", w_2d)

# Get VMR as a 2-D numpy array.
vmr = gas_obj.get_vmr("CO2")
print("VMR for CO2:", vmr)
vmr = gas_obj.get_vmr("O3")
print("VMR for O3:", vmr)
vmr = gas_obj.get_vmr("H2O")
print("VMR for H2O:", vmr)
