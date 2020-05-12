#!/bin/sh
# Creates MCS files for the following OpenCAPI cards
# Usage: ./write_mcs card_name
# card_name can be one of the following options: ad9v3, ad9h7, ad9h3

# tcl wrapper - next line treated as comment by vivado \
if [ "$#" -ne 1 ]; then echo "Need to pass 1 command line argument (card eg: ad9v3, ad9h7, ad9h3)"; exit 3; else exec vivado -notrace -nojournal -nolog -source "$0" -mode batch -tclargs "$1"; fi

set card ""

if {$::argc == 1} {
    set card [string tolower [lindex $::argv 0]]
    puts "$card"
} 

set myformat    "MCS"
set mysize      64
set myinterface "SPIx8"

if {$card eq "ad9v3"} {
    set mysize 64

} elseif {$card eq "ad9h7"} {
    set mysize 128
} elseif {$card eq "ad9h3"} {
    set mysize 64
} else {
    puts "Card not recognized"
    exit;
}

set bitstream_name [string trim [lindex [glob *.bit] 0]]

#remove .bit from name
set bitstream_name [string trimright $bitstream_name ".bit"]

# Write MCS file for Alphadata 9V3 card's flash memory
write_cfgmem -format $myformat -size $mysize -interface $myinterface -loadbit "up 0x0000000 ./$bitstream_name.bit" ./mcs_files/$bitstream_name.mcs
