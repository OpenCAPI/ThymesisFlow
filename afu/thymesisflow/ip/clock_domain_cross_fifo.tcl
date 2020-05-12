## clock domain cross fifo that bridges dataflow between  OpenCAPI DLx and  Aurora transceiver clock domains 

create_ip -name axis_data_fifo -vendor xilinx.com -library ip -version 1.1 -module_name clock_domain_cross_fifo	

set_property -dict [list CONFIG.TDATA_NUM_BYTES {32} CONFIG.FIFO_DEPTH {256} CONFIG.IS_ACLK_ASYNC {1} CONFIG.SYNCHRONIZATION_STAGES {8}] [get_ips clock_domain_cross_fifo]

generate_target {all} [get_ips clock_domain_cross_fifo]

