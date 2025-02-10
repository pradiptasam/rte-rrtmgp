from cloud_optics import CloudOpticsCoeffsReader

def init_cloud_optics():

    cloud_optic_lw=CloudOpticsCoeffsReader("rrtmgp-cloud-optics-coeffs-lw.nc")
    cloud_optic_lw.load_coefficients()
    cloud_optic_lw.print_summary()

    cloud_optic_sw=CloudOpticsCoeffsReader("rrtmgp-cloud-optics-coeffs-sw.nc")
    cloud_optic_sw.load_coefficients()
    cloud_optic_sw.print_summary()

    return cloud_optic_lw, cloud_optic_sw
