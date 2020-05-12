## data lookup bram of depth 512 for storing command flits that have to be reissued

create_ip -name blk_mem_gen -vendor xilinx.com -library ip -version 8.4 -module_name data_lookup_bram

set_property -dict [list CONFIG.Memory_Type {True_Dual_Port_RAM} CONFIG.Enable_32bit_Address {true} CONFIG.Use_Byte_Write_Enable {true} CONFIG.Byte_Size {8} CONFIG.Write_Width_A {512} CONFIG.Write_Depth_A {512} CONFIG.Read_Width_A {512} CONFIG.Write_Width_B {512} CONFIG.Read_Width_B {512} CONFIG.Enable_B {Use_ENB_Pin} CONFIG.Register_PortA_Output_of_Memory_Primitives {false} CONFIG.Register_PortB_Output_of_Memory_Primitives {false} CONFIG.Use_RSTA_Pin {true} CONFIG.Use_RSTB_Pin {true} CONFIG.Port_B_Clock {100} CONFIG.Port_B_Write_Rate {50} CONFIG.Port_B_Enable_Rate {100} CONFIG.use_bram_block {BRAM_Controller} CONFIG.EN_SAFETY_CKT {true}] [get_ips data_lookup_bram]

generate_target {all} [get_ips data_lookup_bram]
