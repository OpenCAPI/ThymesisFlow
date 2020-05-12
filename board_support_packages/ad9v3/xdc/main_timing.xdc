###############################################################
## FPGA: xcvu3p-ffvc1517-2-i
###############################################################

###############################################################
## Synthesis Constraints
###############################################################
create_clock -period 6.400 -name mgtrefclk1_x0y0_p [get_ports mgtrefclk1_x0y0_p]
create_clock -period 6.400 -name mgtrefclk1_x0y1_p [get_ports mgtrefclk1_x0y1_p]

create_clock -period 3.333 -name freerun_clk_p [get_ports freerun_clk_p]

# Timing analysis tools are unable to calculate fabric clock correctly because of the fractional-N in the
# transceiver.  Therefore, overconstrain the design to get the correct clock calculation.
# create_clock -period 6.361 -name mgtrefclk1_x0y0_p [get_ports mgtrefclk1_x0y0_p]
# create_clock -period 6.361 -name mgtrefclk1_x0y1_p [get_ports mgtrefclk1_x0y1_p]


# False path constraints
# ----------------------------------------------------------------------------------------------------------------------
set_false_path -to [get_cells -hierarchical -filter {NAME =~ *bit_synchronizer*inst/i_in_meta_reg}]
set_false_path -to [get_cells -hierarchical -filter {NAME =~ *bit_synchronizer*inst/i_in_meta_reg}]
set_false_path -to [get_cells -hierarchical -filter {NAME =~ *reset_synchronizer*inst/rst_in_*_reg}]
set_false_path -to [get_cells -hierarchical -filter {NAME =~ *reset_synchronizer*inst/rst_in_*_reg}]
set_false_path -to [get_cells -hierarchical -filter {NAME =~ *gtwiz_userclk_tx_inst/*gtwiz_userclk_tx_active_*_reg}]
set_false_path -from [get_clocks mgtrefclk1_x0y0_p] -to [get_clocks -of_objects [get_pins {bsp/dlx_phy/example_wrapper_inst/DLx_phy_inst/inst/gen_gtwizard_gtye4_top.DLx_phy_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[1].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[0].GTYE4_CHANNEL_PRIM_INST/TXOUTCLK}]]
set_false_path -from [get_clocks -of_objects [get_pins {bsp/dlx_phy/example_wrapper_inst/DLx_phy_inst/inst/gen_gtwizard_gtye4_top.DLx_phy_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[1].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[0].GTYE4_CHANNEL_PRIM_INST/TXOUTCLK}]] -to [get_clocks -of_objects [get_pins bsp/dlx_phy/BUFGCE_DIV_inst/O]]
