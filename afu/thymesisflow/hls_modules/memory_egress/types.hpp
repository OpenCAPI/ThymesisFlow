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

#ifndef TYPES_HH
#define TYPES_HH

#include <ap_int.h>


typedef ap_uint<253> resp_combo_t; //tlrflit (252:215) + tlxscflit_t (214:0)
typedef ap_uint<32> count_t;
typedef ap_uint<513> ocxflitbdi_t; 
typedef ap_uint<512> ocxflit_t; 
typedef ap_uint<256> ocxflitH_t;
typedef ap_uint<1> valid_t;
typedef ap_uint<199> tlxcflit_t;
typedef ap_uint<38> tlrflit_t;
typedef ap_uint<215> tlxscflit_t;
typedef ap_uint<3> dlsize_t;
typedef ap_uint<36> cutimer_t;
typedef ap_uint<20> pasid_t;
typedef ap_uint<4> stream_t;
typedef ap_uint<12> actag_t;
typedef ap_uint<16> bdf_t;
typedef ap_uint<3> pl_t;
typedef ap_uint<64> addr_t;
typedef ap_uint<4> code_t;
typedef ap_uint<2> dp_t;
typedef ap_uint<1> pad_t;
typedef ap_uint<6> nid_t;
typedef ap_uint<8> opcode_t;
typedef ap_uint<2> dl_t;
typedef ap_uint<16> tag_t;
typedef ap_uint<1> signal_t;
typedef ap_uint<3> sel_t;
typedef ap_uint<10> combo_t;
typedef ap_uint<10> fifocnt_t;
typedef ap_uint<8> token_t;
typedef ap_uint<4> xlatecnt_t;

#endif
