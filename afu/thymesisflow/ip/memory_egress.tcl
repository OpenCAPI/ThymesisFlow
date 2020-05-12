## ocx_memory_egress ip creation

create_ip -name ocx_memory_egress_top -vendor ibm.com -library ThymesisFlow -version 1.0 -module_name ocx_memory_egress


generate_target {all} [get_ips ocx_memory_egress]
