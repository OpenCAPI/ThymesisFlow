## fifo that stores command and dataflits that need to be retried.

create_ip -name axis_data_fifo -vendor xilinx.com -library ip -version 1.1 -module_name rty_data_fifo

set_property -dict [list CONFIG.TDATA_NUM_BYTES {64} CONFIG.FIFO_DEPTH {128}] [get_ips rty_data_fifo]

generate_target {all} [get_ips rty_data_fifo]

