#ifndef RTE_RRTMGP_INTERFACE_HPP
#define RTE_RRTMGP_INTERFACE_HPP

#include "read_data.hpp"    // Declares InputData
// #include "CloudOptics.hpp"   // Declares CloudOptics and init_cloud_optics()
#include <vector>
#include <iostream>
#include <limits>
#include <cmath>

// Declaration of the RteRrtmgpInterface class.
class RteRrtmgpInterface {
public:
    // Store a reference to the InputData.
    const InputData &input;
    
    // Constants used by the model.
    double ccwmin;
    double zkap_cont;
    double zkap_mrtm;
    double del1;
    double del2;
    double cld_frc_thresh;
    double droplet_scale;
    double rhoh2o;
    double effective_radius;
    double nir_vis_boundary;
    
    // Constructor that stores a reference to the external InputData.
    RteRrtmgpInterface(const InputData &data);
    
    // on_block() arranges inputs and computes output arrays.
    void on_block();
};

#endif // RTE_RRTMGP_INTERFACE_HPP
