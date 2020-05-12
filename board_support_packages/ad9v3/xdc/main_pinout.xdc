###############################################################
## Placement Constraints: xcvu3p-ffvc1517
###############################################################

set_property PACKAGE_PIN A30 [get_ports ocde]
set_property IOSTANDARD LVCMOS18 [get_ports ocde]

########################
## Bottom Left Corner ##
########################

# DLX0 Clocks
set_property PACKAGE_PIN AG34 [get_ports mgtrefclk1_x0y0_n]
set_property PACKAGE_PIN AG33 [get_ports mgtrefclk1_x0y0_p]
set_property PACKAGE_PIN AC34 [get_ports mgtrefclk1_x0y1_n]
set_property PACKAGE_PIN AC33 [get_ports mgtrefclk1_x0y1_p]

# Free run clock constraint
# page 13 adm-pcie-9v3 user manual.pdf
# Signal: FABRIC_CLK
set_property PACKAGE_PIN AP27 [get_ports freerun_clk_n]
set_property PACKAGE_PIN AP26 [get_ports freerun_clk_p]
set_property IOSTANDARD  LVDS [get_ports freerun_clk_p]
set_property IOSTANDARD  LVDS [get_ports freerun_clk_n]

# DLX0 TRANSMIT/RECEIVE Channels
set_property PACKAGE_PIN AV36 [get_ports ch0_gtyrxp_in]
set_property PACKAGE_PIN AV37 [get_ports ch0_gtyrxn_in]
set_property PACKAGE_PIN AW33 [get_ports ch0_gtytxp_out]
set_property PACKAGE_PIN AW34 [get_ports ch0_gtytxn_out]
set_property PACKAGE_PIN AU38 [get_ports ch1_gtyrxp_in]
set_property PACKAGE_PIN AU39 [get_ports ch1_gtyrxn_in]
set_property PACKAGE_PIN AU33 [get_ports ch1_gtytxp_out]
set_property PACKAGE_PIN AU34 [get_ports ch1_gtytxn_out]
set_property PACKAGE_PIN AR38 [get_ports ch2_gtyrxp_in]
set_property PACKAGE_PIN AR39 [get_ports ch2_gtyrxn_in]
set_property PACKAGE_PIN AT35 [get_ports ch2_gtytxp_out]
set_property PACKAGE_PIN AT36 [get_ports ch2_gtytxn_out]
set_property PACKAGE_PIN AN38 [get_ports ch3_gtyrxp_in]
set_property PACKAGE_PIN AN39 [get_ports ch3_gtyrxn_in]
set_property PACKAGE_PIN AP35 [get_ports ch3_gtytxp_out]
set_property PACKAGE_PIN AP36 [get_ports ch3_gtytxn_out]
set_property PACKAGE_PIN AL38 [get_ports ch4_gtyrxp_in]
set_property PACKAGE_PIN AL39 [get_ports ch4_gtyrxn_in]
set_property PACKAGE_PIN AM35 [get_ports ch4_gtytxp_out]
set_property PACKAGE_PIN AM36 [get_ports ch4_gtytxn_out]
set_property PACKAGE_PIN AJ38 [get_ports ch5_gtyrxp_in]
set_property PACKAGE_PIN AJ39 [get_ports ch5_gtyrxn_in]
set_property PACKAGE_PIN AK35 [get_ports ch5_gtytxp_out]
set_property PACKAGE_PIN AK36 [get_ports ch5_gtytxn_out]
set_property PACKAGE_PIN AG38 [get_ports ch6_gtyrxp_in]
set_property PACKAGE_PIN AG39 [get_ports ch6_gtyrxn_in]
set_property PACKAGE_PIN AH35 [get_ports ch6_gtytxp_out]
set_property PACKAGE_PIN AH36 [get_ports ch6_gtytxn_out]
set_property PACKAGE_PIN AE38 [get_ports ch7_gtyrxp_in]
set_property PACKAGE_PIN AE39 [get_ports ch7_gtyrxn_in]
set_property PACKAGE_PIN AF35 [get_ports ch7_gtytxp_out]
set_property PACKAGE_PIN AF36 [get_ports ch7_gtytxn_out]
