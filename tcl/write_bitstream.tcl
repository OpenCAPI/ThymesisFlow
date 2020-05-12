# Creates a bitstream for the current Vivado project run

set bitstream_dir [get_property DIRECTORY [current_run]]
set bitstream_name [get_property TOP [current_fileset]]
set bitstream_name ${bitstream_name}.bit

# Sometimes the current_fileset TOP property is empty, so give the bitstream a generic name if this happens
if { $bitstream_name eq ".bit" } {
    set bitstream_name "fpga_top${bitstream_name}"
}

puts "HELP-----"
puts "bitstream_dir:  |$bitstream_dir|"
puts "bitstream_name: |$bitstream_name|"
report_property [current_fileset] -all
report_property [current_project] -all
report_property [current_run] -all

write_bitstream -force ./${bitstream_name}
write_debug_probes -force ./debug_nets.ltx