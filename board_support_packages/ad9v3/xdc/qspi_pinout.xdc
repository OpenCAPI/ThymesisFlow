# Following ports should be added as outputs in top level, and assigned to data outputs of AXI QSPI Core passed through IOBUF
#set_property PACKAGE_PIN AB8  [get_ports FPGA_FLASH_DQ0]
#set_property PACKAGE_PIN AD8  [get_ports FPGA_FLASH_DQ1]
#set_property PACKAGE_PIN Y8   [get_ports FPGA_FLASH_DQ2]
#set_property PACKAGE_PIN AC8  [get_ports FPGA_FLASH_DQ3]
set_property PACKAGE_PIN AF30 [get_ports FPGA_FLASH_DQ4]
set_property PACKAGE_PIN AG30 [get_ports FPGA_FLASH_DQ5]
set_property PACKAGE_PIN AF28 [get_ports FPGA_FLASH_DQ6]
set_property PACKAGE_PIN AG28 [get_ports FPGA_FLASH_DQ7]

# Slave select output of AXI QSPI Core
set_property PACKAGE_PIN AV30 [get_ports FPGA_FLASH_CE2_L]

#set_property IOSTANDARD LVCMOS18 [get_ports FPGA_FLASH_DQ0]
#set_property IOSTANDARD LVCMOS18 [get_ports FPGA_FLASH_DQ1]
#set_property IOSTANDARD LVCMOS18 [get_ports FPGA_FLASH_DQ2]
#set_property IOSTANDARD LVCMOS18 [get_ports FPGA_FLASH_DQ3]
set_property IOSTANDARD LVCMOS18 [get_ports FPGA_FLASH_DQ4]
set_property IOSTANDARD LVCMOS18 [get_ports FPGA_FLASH_DQ5]
set_property IOSTANDARD LVCMOS18 [get_ports FPGA_FLASH_DQ6]
set_property IOSTANDARD LVCMOS18 [get_ports FPGA_FLASH_DQ7]
set_property IOSTANDARD LVCMOS18 [get_ports FPGA_FLASH_CE2_L]

