# Writes a hidden config file, so package_bitstreams.pl knows how to 
# package the bitstream results

set origin_dir "."
if { [info exists ::env(OPENCAPI_BUILD_DIR)] } {
  set origin_dir $::env(OPENCAPI_BUILD_DIR)
} else {
  puts "Need to create environmental variable OPENCAPI_BUILD_DIR"
  exit 0
}


# AFU TYPE | PHY TYPE | Speed
# config_contents is a global variable created in the create_project* tcl script
set data $::config_contents;
set hidden_dir "$origin_dir/viv_proj/.my_settings";
set filename "$hidden_dir/config"

file mkdir $hidden_dir;

# Open FILE handle
set FILE [open $filename "w"]

puts $FILE $data
close $FILE

