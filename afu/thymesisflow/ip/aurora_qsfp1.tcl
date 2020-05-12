## Aurora 64B66B QSFP1 configuration for 9V3 cage QSFP1 - 4x25.78125 and 309.375 reference clock

create_ip -name aurora_64b66b -vendor xilinx.com -library ip -version 11.2 -module_name aurora_qsfp1

set_property -dict [list CONFIG.CHANNEL_ENABLE {X0Y12 X0Y13 X0Y14 X0Y15} CONFIG.C_AURORA_LANES {4} CONFIG.C_LINE_RATE {25.78125} CONFIG.C_REFCLK_FREQUENCY {309.375} \
CONFIG.C_INIT_CLK {100} CONFIG.C_UCOLUMN_USED {left} CONFIG.C_START_QUAD {Quad_X0Y3} CONFIG.C_START_LANE {X0Y12} CONFIG.C_REFCLK_SOURCE {MGTREFCLK0_of_Quad_X0Y3} \
CONFIG.C_GT_LOC_4 {4} CONFIG.C_GT_LOC_3 {3} CONFIG.C_GT_LOC_2 {2} CONFIG.crc_mode {true} CONFIG.SupportLevel {1} CONFIG.C_USE_BYTESWAP {true} CONFIG.drp_mode {Native}] [get_ips aurora_qsfp1]


generate_target {all} [get_ips aurora_qsfp1]
