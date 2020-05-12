# IP is created in Vivado project this script is sourced from


create_ip -name vio -vendor xilinx.com -library ip -version 3.0 -module_name vio_reset_n
set_property -dict [list CONFIG.C_PROBE_OUT0_INIT_VAL {0x1} CONFIG.C_NUM_PROBE_IN {2}] [get_ips vio_reset_n]
generate_target all [get_ips vio_reset_n]

