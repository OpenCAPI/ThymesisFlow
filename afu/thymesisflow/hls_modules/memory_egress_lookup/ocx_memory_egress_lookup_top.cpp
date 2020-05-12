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
#include "ocx_cmd_fifo_load.hpp"



void ocx_memory_egress_lookup_top(stream<tlrflit_t> &tl_resp, \
			   /*External BRAM Input Interfaces */
			   tlxscflit_t cmdlookup_in[LOOKUPSIZE],\
                           /*outputs*/
                           count_t &valex_memegress_cmddata_ld_cnt,\
			   stream<resp_combo_t> &tl_fifo_resp_out)
{


#pragma HLS INTERFACE bram register port=cmdlookup_in
#pragma HLS RESOURCE variable=cmdlookup_in core=RAM_1P_BRAM latency=1


#pragma HLS INTERFACE ap_none register port=valex_memegress_cmddata_ld_cnt
#pragma HLS INTERFACE axis register port=tl_fifo_resp_out
#pragma HLS INTERFACE axis register port=tl_resp
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
CmdFifoLoad CmdFifold;


/*
   Fifo load for TL cmd
   Input:
     tl_resp comming from TL
   Output: 
     tl_fifo_resp a fifo that feeds Resp egine.
*/

CmdFifold.CmdFifoLoadSM(tl_resp,\
		        cmdlookup_in,\
                valex_memegress_cmddata_ld_cnt,\
		        tl_fifo_resp_out);


}
