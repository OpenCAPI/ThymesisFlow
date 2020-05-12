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


#include "ocx_cmd_fifo_load.hpp"
#include "globals.hpp"

CmdFifoLoad::CmdFifoLoad()
{
  return;
}

void CmdFifoLoad::CmdFifoLoadSM(stream<tlrflit_t> &input_fifo,\
		                        tlxscflit_t   cmdlookup[LOOKUPSIZE], \
                                        count_t &valex_memegress_cmddata_ld_cnt,\
					stream<resp_combo_t> &output_fifo)
{
    #pragma HLS pipeline II=1 enable_flush

    OCxFlit tlresp;
    OCxFlit cmdf;
    static count_t valex_memegress_cmddata_ld_cntin = 0x0;
    static resp_combo_t to_send;

    if (!input_fifo.empty()) {
    	      tlresp.fromTLRespData(input_fifo.read());
    	      cmdf.fromTLxCStore(cmdlookup[tlresp.getRespCappTag()]);
              valex_memegress_cmddata_ld_cnt = valex_memegress_cmddata_ld_cntin;
              valex_memegress_cmddata_ld_cntin = valex_memegress_cmddata_ld_cntin + 1;
              to_send.range(252,215) = tlresp.toTLRespData().range(37,0);
              to_send.range(214,0) = cmdf.toTLxCStore().range(214,0);
              output_fifo.write(to_send);
    }

    return;
}


