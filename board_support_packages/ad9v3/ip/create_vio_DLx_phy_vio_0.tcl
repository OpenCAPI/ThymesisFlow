# Create DLx_phy VIO
create_ip -name vio -vendor xilinx.com -library ip -version 3.0 -module_name DLx_phy_vio_0
set_property -dict [list CONFIG.C_PROBE_OUT1_INIT_VAL {0x1} CONFIG.C_PROBE_IN6_WIDTH {8} CONFIG.C_PROBE_IN5_WIDTH {8} CONFIG.C_PROBE_IN4_WIDTH {8} CONFIG.C_PROBE_IN3_WIDTH {4} CONFIG.C_NUM_PROBE_OUT {7} CONFIG.C_NUM_PROBE_IN {13}] [get_ips DLx_phy_vio_0]
generate_target all [get_ips DLx_phy_vio_0]
