## tl_resp_fifo stores response flits

create_ip -name axis_data_fifo -vendor xilinx.com -library ip -version 1.1 -module_name tl_resp_fifo

set_property -dict [list CONFIG.TDATA_NUM_BYTES {32} CONFIG.FIFO_DEPTH {32}] [get_ips tl_resp_fifo]

generate_target {all} [get_ips tl_resp_fifo]

