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


#ifndef OCX_RTY_ENG_HH
#define OCX_RTY_ENG_HH

#include "globals.hpp"
#include "types.hpp"

#include "ocx_flit.hpp"

class RtyEng {

   public:
          
      RtyEng();
       //Get Input from Retry engine and accordingly forwards to command engine
      void  RtyEngSM(cutimer_t timeout,\
                       stream<ocxflit_t> &input_fifo, \
		       stream<ocxflit_t> &rty_jail,\
                       stream<ocxflit_t> &output_fifo,
                       count_t &valex_memingress_cmdf_rty_in,\
                       count_t &valex_memingress_cmdf_jail_in,\
                       count_t &valex_memingress_cmdf_rty_out,\
                       count_t &valex_memingress_cmdf_jail_out,\
                       count_t &valex_memingress_data_rty_in,\
                       count_t &valex_memingress_data_rty_out,\
                       count_t &valex_memingress_data_jail_in,\
                       count_t &valex_memingress_data_jail_out); 



};





#endif
