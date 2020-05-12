# FPGA-external clock definitions to help timing analysis
# probably not needed because we configure the reference clock frequency in the aurora core
# but adding anyway for completeness. 
 
##QSFP reference clocks 309.25Mhz
create_clock -period 3.232 -name QSFP0_REF_CLK_P [get_ports QSFP0_REF_CLK_P]


#set false path between aurora reset module (clock domain clock_100) and actual reset pins on aurora module.
set_false_path -from [get_clocks -of_objects [get_pins {bsp/dlx_phy/example_wrapper_inst/DLx_phy_inst/inst/gen_gtwizard_gtye4_top.DLx_phy_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[1].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[0].GTYE4_CHANNEL_PRIM_INST/TXOUTCLK}]] -to [get_clocks -of_objects [get_pins bsp/dlx_phy/BUFGCE_DIV_inst2/O]]
set_false_path -from [get_clocks -of_objects [get_pins bsp/dlx_phy/BUFGCE_DIV_inst2/O]] -to [get_clocks -of_objects [get_pins {oc_func/AFU00/TF_TOP/AURORA_QSFP0_CORE/inst/aurora_qsfp0_core_i/aurora_qsfp0_wrapper_i/aurora_qsfp0_multi_gt_i/aurora_qsfp0_gt_i/inst/gen_gtwizard_gtye4_top.aurora_qsfp0_gt_gtwizard_gtye4_inst/gen_gtwizard_gtye4.gen_channel_container[4].gen_enabled_channel.gtye4_channel_wrapper_inst/channel_inst/gtye4_channel_gen.gen_gtye4_channel_inst[2].GTYE4_CHANNEL_PRIM_INST/TXOUTCLK}]]

# false path constraints for example design paths on aurora
set_false_path -from [get_pins oc_func/AFU00/TF_TOP/AURORA_QSFP0_CORE/inst/aurora_qsfp0_core_i/aurora_qsfp0_wrapper_i/aurora_qsfp0_multi_gt_i/gtwiz_userclk_rx_reset_in_r_reg/C] -to [get_pins oc_func/AFU00/TF_TOP/AURORA_QSFP0_CORE/inst/aurora_qsfp0_core_i/aurora_qsfp0_wrapper_i/aurora_qsfp0_multi_gt_i/ultrascale_rx_userclk/gen_gtwiz_userclk_rx_main.rx_active_stg2_reg/CLR]
set_false_path -from [get_pins oc_func/AFU00/TF_TOP/AURORA_QSFP0_CORE/inst/aurora_qsfp0_core_i/aurora_qsfp0_wrapper_i/aurora_qsfp0_multi_gt_i/gtwiz_userclk_rx_reset_in_r_reg/C] -to [get_pins oc_func/AFU00/TF_TOP/AURORA_QSFP0_CORE/inst/aurora_qsfp0_core_i/aurora_qsfp0_wrapper_i/aurora_qsfp0_multi_gt_i/ultrascale_rx_userclk/gen_gtwiz_userclk_rx_main.gtwiz_userclk_rx_active_out_reg/CLR]
set_false_path -from [get_pins oc_func/AFU00/TF_TOP/AURORA_QSFP0_CORE/inst/aurora_qsfp0_core_i/aurora_qsfp0_wrapper_i/aurora_qsfp0_multi_gt_i/gtwiz_userclk_rx_reset_in_r_reg/C] -to [get_pins oc_func/AFU00/TF_TOP/AURORA_QSFP0_CORE/inst/aurora_qsfp0_core_i/aurora_qsfp0_wrapper_i/aurora_qsfp0_multi_gt_i/ultrascale_rx_userclk/gen_gtwiz_userclk_rx_main.rx_active_aurora_qsfp0_cdc_to_reg/CLR]


set_false_path -to [get_pins -hier *aurora_qsfp0_cdc_to*/D]

