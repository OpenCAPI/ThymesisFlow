## LPC axi_quad_spi
create_ip -name axi_quad_spi -vendor xilinx.com -library ip -version 3.2 -module_name axi_quad_spi_0
set_property -dict [list CONFIG.C_SPI_MEMORY {2} CONFIG.C_SPI_MODE {2} CONFIG.C_DUAL_QUAD_MODE {1} CONFIG.C_NUM_SS_BITS {2} CONFIG.C_FIFO_DEPTH {256} CONFIG.Async_Clk {1} CONFIG.C_USE_STARTUP {1} CONFIG.C_SCK_RATIO {2}] [get_ips axi_quad_spi_0]
generate_target {all} [get_ips axi_quad_spi_0]
