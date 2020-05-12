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


#include "ocx_rty_eng.hpp"


RtyEng::RtyEng()
{
 return;
}


void RtyEng::RtyEngSM(cutimer_t timeout, stream<ocxflit_t> &input_fifo,\
    		       stream<ocxflit_t> &rty_jail,\
                       stream<ocxflit_t> &output_fifo,\
                       count_t &valex_memingress_cmdf_rty_in,\
                       count_t &valex_memingress_cmdf_jail_in,\
                       count_t &valex_memingress_cmdf_rty_out,\
                       count_t &valex_memingress_cmdf_jail_out,\
                       count_t &valex_memingress_data_rty_in,\
                       count_t &valex_memingress_data_rty_out,\
                       count_t &valex_memingress_data_jail_in,\
                       count_t &valex_memingress_data_jail_out)

{


  #pragma HLS pipeline II=1 enable_flush

  static enum State {START = 0,\
	                 DATA,\
                    JAIL_DATA,\
                    } smstate;

  
  static OCxFlit rtycmd;
  static dlsize_t rdata = 0x0;
  ocxflit_t dflt;  
  dl_t fdlt;
  static valid_t xlate_lock = 0x0;
  static cutimer_t timeout_hack = 0x2800;
  static cutimer_t counter = 0x0;
  static valid_t jaildata = 0x0;
  static count_t  valex_memingress_cmdf_rty_inin = 0x0;
  static count_t  valex_memingress_cmdf_jail_inin = 0x0;
  static count_t  valex_memingress_cmdf_rty_outin = 0x0;
  static count_t  valex_memingress_cmdf_jail_outin = 0x0;
  static count_t  valex_memingress_data_rty_inin = 0x0;
  static count_t  valex_memingress_data_rty_outin = 0x0;
  static count_t  valex_memingress_data_jail_inin = 0x0;
  static count_t  valex_memingress_data_jail_outin = 0x0;

  switch(smstate) {

          case START:{
              if (!input_fifo.empty()) {

                  rtycmd.fromOCxFlit(input_fifo.read());
                  valex_memingress_cmdf_rty_in = valex_memingress_cmdf_rty_inin;
                  valex_memingress_cmdf_rty_inin = valex_memingress_cmdf_rty_inin + 1;
                  fdlt = rtycmd.getDl();
                  rdata.range(2,2) = fdlt.range(1,1) & fdlt.range(0,0); //dlt is 11 bit high
                  rdata.range(1,1) = fdlt.range(1,1) & !fdlt.range(0,0);//dlt is 10 bit high
                  rdata.range(0,0) = !fdlt.range(1,1); //It is 01 or 00 then bit high 
                  if ((rtycmd.getTLxCmdRespAnno() == TLRESP_CODE_RTY)) {
                      //Going to jail these flits -- set jail timer if this is the first
                	  //or set the lock for xlate_done
                	  if (xlate_lock == 0x0) {
                		  xlate_lock = 0x1;
                	  }
                	  //send flit to jail
                	  valex_memingress_cmdf_jail_in = valex_memingress_cmdf_jail_inin;
                          valex_memingress_cmdf_jail_inin = valex_memingress_cmdf_jail_inin + 1;
                	  rty_jail.write(rtycmd.toOCxFlit());
                	  if (rtycmd.TLxCmdIsWrite()) {
                		  jaildata = 0x1;
                	      smstate = DATA;
                	  }
                  } else {
                      if (rtycmd.TLxCmdIsWrite()) {
                    	  jaildata = 0x0;
                          smstate = DATA;
                      }//Xlate done arrived - lift lock after this command to re-issue them all.
                       xlate_lock=0x0;
                      //schedule command for re-issue
                       valex_memingress_cmdf_rty_out = valex_memingress_cmdf_rty_outin;
                      valex_memingress_cmdf_rty_outin = valex_memingress_cmdf_rty_outin + 1;
                      output_fifo.write(rtycmd.toOCxFlit());
                  }  //danger here if consequtive page faults some flits might need to wait for another follow up xlate done here.
               } else if (!rty_jail.empty() && /* (counter == 0)*/(xlate_lock == 0x0)) {
            	      //command to exit jail
            	      rtycmd.fromOCxFlit(rty_jail.read());
            	      valex_memingress_cmdf_jail_out = valex_memingress_cmdf_jail_outin;
            	      valex_memingress_cmdf_jail_outin = valex_memingress_cmdf_jail_outin + 1;
                      fdlt = rtycmd.getDl();
            	      rdata.range(2,2) = fdlt.range(1,1) & fdlt.range(0,0); //dlt is 11 bit high
            	      rdata.range(1,1) = fdlt.range(1,1) & !fdlt.range(0,0);//dlt is 10 bit high
            	      rdata.range(0,0) = !fdlt.range(1,1); //It is 01 or 00 then bit high
            	      if (rtycmd.TLxCmdIsWrite()) {
            	         smstate = JAIL_DATA;
            	      }
            	      output_fifo.write(rtycmd.toOCxFlit());
               } 
               break;
           }
           case DATA: {
               if (!input_fifo.empty()) {
                   dflt = input_fifo.read();       
                   if (rdata == 0x1) {
                       smstate = START;
                   } else {
                       rdata = rdata - 1;
                   }
                   valex_memingress_data_rty_in = valex_memingress_data_rty_inin;
                   valex_memingress_data_rty_inin = valex_memingress_data_rty_inin + 1;
                   if(jaildata == 0x1) {
                       valex_memingress_data_jail_in = valex_memingress_data_jail_inin;
                       valex_memingress_data_jail_inin = valex_memingress_data_jail_inin + 1;
                       rty_jail.write(dflt);
                   } else {
                       valex_memingress_data_rty_out = valex_memingress_data_rty_outin + 1;
                       valex_memingress_data_rty_outin = valex_memingress_data_rty_outin + 1;
                       output_fifo.write(dflt);
                   }
                } 
               break;
           }
           case JAIL_DATA: {
                if (!rty_jail.empty()) {
                	if (rdata == 0x1) {
                        smstate = START;
                    } else {
                        rdata = rdata - 1;
                    }
                    valex_memingress_data_jail_out = valex_memingress_data_jail_outin;
                    valex_memingress_data_jail_outin = valex_memingress_data_jail_outin + 1;
                    output_fifo.write(rty_jail.read());
                }
                break;
            }
           default : { break;} 
  }; //end of switch

}
