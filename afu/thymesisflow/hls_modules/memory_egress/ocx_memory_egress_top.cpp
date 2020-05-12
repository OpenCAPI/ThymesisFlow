// *!***************************************************************************
// *! Copyright 2019 International Business Machines
// *!
// *! Licensed under the Apache License, Version 2.0 (the "License");
// *! you may not use this file except in compliance with the License.
// *! You may obtain a copy of the License at
// *! http://www.apache.org/licenses/LICENSE-2.0 
// *!
// *! The patent license granted to you in Section 3 of the License, as applied
// *! to the "Work," hereby includes implementations of the Work in physical form.  
// *!
// *! Unless required by applicable law or agreed to in writing, the reference design
// *! distributed under the License is distributed on an "AS IS" BASIS,
// *! WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// *! See the License for the specific language governing permissions and
// *! limitations under the License.
// *! 
// *! The background Specification upon which this is based is managed by and available from
// *! the OpenCAPI Consortium.  More information can be found at https://opencapi.org. 
// *!***************************************************************************
// Module designer: Dimitris Syrivelis
// Backup: Christian Pinto, Michele Gazzetti

#include "globals.hpp"

#include "types.hpp"
#include "ocx_flit.hpp"
#include "ocx_resp_eng.hpp"
#include "ocx_rty_eng.hpp"


                         /*Inputs*/
void ocx_memory_egress_top(cutimer_t &rtry_timeout,\
		                    valid_t &enable,\
                           stream<resp_combo_t> tl_fifo_resp,\
                           stream<ocxflitbdi_t> &tl_fifo_data,\
                           /*External BRAM Input Interfaces */
			   ocxflit_t data0lookup_in[LOOKUPSIZE],\
			   ocxflit_t data1lookup_in[LOOKUPSIZE],\
			   ocxflit_t data2lookup_in[LOOKUPSIZE],\
			   ocxflit_t data3lookup_in[LOOKUPSIZE],\
                           /*outputs*/
                           stream<tag_t> &capptag_fifo_out,\
                           stream<ocxflit_t> &rty_issue_fifo_out,\
			   stream<ocxflit_t> &net_resp_out,\
                           count_t &valex_memegress_cmdf_cnt,\
                           count_t &valex_memegress_data_cnt,\
                           count_t &valex_memegress_cmdf_torty,\
                           count_t &valex_memegress_data_torty,\
                           count_t &valex_memingress_cmdf_rty_in,\
                           count_t &valex_memingress_cmdf_jail_in,\
                           count_t &valex_memingress_cmdf_rty_out,\
                           count_t &valex_memingress_cmdf_jail_out,\
                           count_t &valex_memingress_data_rty_in,\
                           count_t &valex_memingress_data_rty_out,\
                           count_t &valex_memingress_data_jail_in,\
                           count_t &valex_memingress_data_jail_out)
{


#pragma HLS INTERFACE ap_none register port=rtry_timeout
#pragma HLS INTERFACE ap_none register port=enable

#pragma HLS INTERFACE bram register port=data0lookup_in
#pragma HLS RESOURCE variable=data0lookup_in core=RAM_1P_BRAM latency=1

#pragma HLS INTERFACE bram register port=data1lookup_in
#pragma HLS RESOURCE variable=data1lookup_in core=RAM_1P_BRAM latency=1

#pragma HLS INTERFACE bram register port=data2lookup_in
#pragma HLS RESOURCE variable=data2lookup_in core=RAM_1P_BRAM latency=1

#pragma HLS INTERFACE bram register port=data3lookup_in
#pragma HLS RESOURCE variable=data3lookup_in core=RAM_1P_BRAM latency=1

#pragma HLS INTERFACE axis register port=tl_fifo_resp

#pragma HLS INTERFACE axis register port=tl_fifo_data
#pragma HLS INTERFACE axis register port=capptag_fifo_out
#pragma HLS INTERFACE axis register port=rty_issue_fifo_out
#pragma HLS INTERFACE axis register port=net_resp_out

#pragma HLS INTERFACE ap_none register port=valex_memegress_cmddata_ld_cnt
#pragma HLS INTERFACE ap_none register port=valex_memegress_cmdf_cnt
#pragma HLS INTERFACE ap_none register port=valex_memegress_data_cnt
#pragma HLS INTERFACE ap_none register port=valex_memegress_cmdf_torty
#pragma HLS INTERFACE ap_none register port=&valex_memegress_data_torty
#pragma HLS INTERFACE ap_none register port=valex_memingress_cmdf_rty_in
#pragma HLS INTERFACE ap_none register port=valex_memingress_cmdf_jail_in
#pragma HLS INTERFACE ap_none register port=valex_memingress_cmdf_rty_out
#pragma HLS INTERFACE ap_none register port=valex_memingress_cmdf_jail_out
#pragma HLS INTERFACE ap_none register port=valex_memingress_data_rty_in
#pragma HLS INTERFACE ap_none register port=valex_memingress_data_rty_out
#pragma HLS INTERFACE ap_none register port=valex_memingress_data_jail_in
#pragma HLS INTERFACE ap_none register port=valex_memingress_data_jail_out

#pragma HLS INTERFACE ap_ctrl_none port=return

#pragma HLS dataflow interval=1

/*
 This Fifo Holds Responses coming from the local TL
 We need this Buffer as the Resp State machine will
 might not be ready to pull, spending cycles on 
 rertry CAM updates or data push. We put a generous 8
 depth here, but i think we will not need more than 4.
*/


/*Various State machines Declarations*/


TLRespEng TlResp;
RtyEng Rtyeng;


static stream<ocxflit_t> rty_unit_fifo_in("rty_unit_fifo_in");
#pragma HLS STREAM variable=rty_unit_fifo_in depth=128

static stream<ocxflit_t> rty_jail("rty_jail");
#pragma HLS STREAM variable=rty_jail depth=512

/*Response Issue Engine
  Inputs:
   resp fifo comes from TL
   resp data comes from TL
   rty_cmd_fifo comes from Cmd Issue Eng,
  Outputs:
   capptag fifo
   rty_cmd_fifo_retry goes to rtry timeout unit
   resp to network
*/

TlResp.RespEngSM(enable,\
		 tl_fifo_resp, \
                 tl_fifo_data, \
                 data0lookup_in,\
                 data1lookup_in,\
                 data2lookup_in,\
                 data3lookup_in,\
                 capptag_fifo_out, \
                 rty_unit_fifo_in, \
                 net_resp_out,\
                 valex_memegress_cmdf_cnt,\
                 valex_memegress_data_cnt,\
                 valex_memegress_cmdf_torty,\
                 valex_memegress_data_torty);


Rtyeng.RtyEngSM(rtry_timeout, \
                rty_unit_fifo_in,\
                rty_jail,\
                rty_issue_fifo_out,\
                valex_memingress_cmdf_rty_in,\
                valex_memingress_cmdf_jail_in,\
                valex_memingress_cmdf_rty_out,\
                valex_memingress_cmdf_jail_out,\
                valex_memingress_data_rty_in,\
                valex_memingress_data_rty_out,\
                valex_memingress_data_jail_in,\
                valex_memingress_data_jail_out);


}
