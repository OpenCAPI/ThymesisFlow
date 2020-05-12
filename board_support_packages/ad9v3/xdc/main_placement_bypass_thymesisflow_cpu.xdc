
create_pblock aurora_qsfp_pblock
add_cells_to_pblock [get_pblocks aurora_qsfp_pblock] [get_cells -quiet [list oc_func/AFU00/TF_TOP/AURORA_QSFP0_CORE oc_func/AFU00/TF_TOP/AURORA_QSFP1_CORE oc_func/AFU00/TF_TOP/TFLLC_32B_QSFP0/TFLLC_32B_FRAMER oc_func/AFU00/TF_TOP/TFLLC_32B_QSFP1/TFLLC_32B_FRAMER]]
resize_pblock [get_pblocks aurora_qsfp_pblock] -add {CLOCKREGION_X0Y3:CLOCKREGION_X0Y4}


create_pblock driver_pblock
add_cells_to_pblock [get_pblocks driver_pblock] [get_cells -quiet [list oc_func/AFU00/TF_TOP/TFLLC_32B_QSFP0/TF_32B_DRIVER_EGR  oc_func/AFU00/TF_TOP/TFLLC_32B_QSFP1/TF_32B_DRIVER_EGR ]]
resize_pblock [get_pblocks driver_pblock] -add {CLOCKREGION_X1Y4:CLOCKREGION_X1Y3}