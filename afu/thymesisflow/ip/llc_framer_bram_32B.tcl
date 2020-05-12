## LLC Framer  BRAM (native interface) that acts as the retransmission fifo buffer to relax timing closure

create_ip -name blk_mem_gen -vendor xilinx.com -library ip -version 8.4 -module_name thymesisflow_llc_framer_bram_32B

set_property -dict [list CONFIG.Memory_Type {Simple_Dual_Port_RAM} CONFIG.Enable_32bit_Address {false} CONFIG.Use_Byte_Write_Enable {false} CONFIG.Byte_Size {9} \
CONFIG.Assume_Synchronous_Clk {true} CONFIG.Write_Width_A {256} CONFIG.Write_Depth_A {256} CONFIG.Read_Width_A {256} CONFIG.Operating_Mode_A {READ_FIRST} \
CONFIG.Enable_A {Always_Enabled} CONFIG.Write_Width_B {256} CONFIG.Read_Width_B {256} CONFIG.Operating_Mode_B {READ_FIRST} CONFIG.Enable_B {Always_Enabled} \
CONFIG.Register_PortA_Output_of_Memory_Primitives {false} CONFIG.Register_PortB_Output_of_Memory_Primitives {false} CONFIG.Use_RSTA_Pin {false} \
CONFIG.Port_B_Clock {100} CONFIG.Port_B_Enable_Rate {100} CONFIG.use_bram_block {Stand_Alone} CONFIG.EN_SAFETY_CKT {false}] [get_ips thymesisflow_llc_framer_bram_32B]

generate_target {all} [get_ips thymesisflow_llc_framer_bram_32B]
