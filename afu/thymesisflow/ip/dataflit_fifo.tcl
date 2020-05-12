## tlx_afu_data_fifo fifo that stores cmddata flits that are emmited by M1 (or compute side) stack towards the network

create_ip -name axis_data_fifo -vendor xilinx.com -library ip -version 1.1 -module_name dataflit_fifo

set_property -dict [list CONFIG.TDATA_NUM_BYTES {65} CONFIG.FIFO_DEPTH {128}] [get_ips dataflit_fifo]

generate_target {all} [get_ips dataflit_fifo]

