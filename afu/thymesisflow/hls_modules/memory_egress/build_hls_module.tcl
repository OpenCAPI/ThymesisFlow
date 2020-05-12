# Create a project
open_project	-reset ocx_memory_egress

# The source files
add_files ocx_memory_egress_top.cpp
add_files ocx_flit.cpp
add_files ocx_resp_eng.cpp 
add_files ocx_rty_eng.cpp

# Specify the top-level function for synthesis
set_top		ocx_memory_egress_top

###########################
# Solution settings

# Create solution1
open_solution -reset solution1

set_part  {xcvu3p-ffvc1517-2-i}
create_clock -period 2.5
config_rtl -reset_level low

## Synthesize design
csynth_design
## Export for vivado IP catalog integration
export_design -rtl verilog -format ip_catalog -description "ThymesisFlow Memory Egress module" -vendor "ibm.com" -library "ThymesisFlow" -display_name "ocx_memory_egress"

exit

