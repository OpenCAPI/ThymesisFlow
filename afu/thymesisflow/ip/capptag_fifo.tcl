## capptag_fifo that stores capptags and enables their recycling.

create_ip -name axis_data_fifo -vendor xilinx.com -library ip -version 1.1 -module_name capptag_fifo

set_property -dict [list CONFIG.TDATA_NUM_BYTES {2} CONFIG.FIFO_DEPTH {512}] [get_ips capptag_fifo]

generate_target {all} [get_ips capptag_fifo]

