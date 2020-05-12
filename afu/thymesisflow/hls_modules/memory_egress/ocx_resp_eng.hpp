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


#ifndef OCX_RESP_ENG_HH
#define OCX_RESP_ENG_HH

#include "globals.hpp"
#include "types.hpp"

#include "ocx_flit.hpp"

class TLRespEng{

 public:
   TLRespEng();
                  //Cmd Response from TL
   void RespEngSM(valid_t &enable,\
		           stream<resp_combo_t> &tlresp_fifo_in, \
                  //Data Response from TL 
                  stream<ocxflitbdi_t> &tlresp_data_in, \
                  ocxflit_t  data0lookup[LOOKUPSIZE],\
                  ocxflit_t  data1lookup[LOOKUPSIZE],\
                  ocxflit_t  data2lookup[LOOKUPSIZE],\
                  ocxflit_t  data3lookup[LOOKUPSIZE],\
                  /*OUTPUTS*/
                  //capptag return..
                  stream<tag_t> &lookup_out,\
                  //rty command out
                  stream<ocxflit_t> &rty_do_cmd_out, \
                  //network command out - should be many
                  stream<ocxflit_t> &network_cmd_out0,\
                  count_t &valex_memegress_cmdf_cnt,\
                  count_t &valex_memegress_data_cnt,\
                  count_t &valex_memegress_cmdf_torty,\
                  count_t &valex_memegress_data_torty);

};



#endif
