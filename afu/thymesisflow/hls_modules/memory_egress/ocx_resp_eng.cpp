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
// Backup: Christian Pinto christian.pinto@ibm.com, Michele Gazzetti



#include "ocx_resp_eng.hpp"

TLRespEng::TLRespEng()
{
  return;
}


void TLRespEng::RespEngSM( valid_t &enable,\
		           stream<resp_combo_t> &tlresp_fifo_in,\
                           stream<ocxflitbdi_t> &tlresp_data_in,\
			   ocxflit_t  data0lookup[LOOKUPSIZE],\
			   ocxflit_t  data1lookup[LOOKUPSIZE],\
			   ocxflit_t  data2lookup[LOOKUPSIZE],\
			   ocxflit_t  data3lookup[LOOKUPSIZE],\
                           /*OUTPUTS*/
                           stream<tag_t> &lookup_out,\
                           stream<ocxflit_t> &rtry_do_cmd_out,\
                           //1 interfaces for now - that should be preprocessor scalable
                           stream<ocxflit_t> &network_cmd_out0,\
                           count_t &valex_memegress_cmdf_cnt,\
                           count_t &valex_memegress_data_cnt,\
                           count_t &valex_memegress_cmdf_torty,\
                           count_t &valex_memegress_data_torty)
{

  #pragma HLS pipeline II=1 enable_flush


  static enum State { INIT = 0, GET_CMD , GET_DATA} smstate;



  OCxFlit cmdf;
  OCxFlit dataf;
  ocxflit_t datar;
  static OCxFlit tlresp;
  static OCxFlit tlrespw;
  static OCxFlit tlrespw1;
  static tag_t capptag_count = 0x1;
  dl_t cdlt;
  dl_t fdlt;
  dl_t respdl;
  dl_t origdl;
  dp_t respdp;
  static valid_t keeplookupentry = 0x0;
  static valid_t respdataflag = 0x0;
  static tag_t localtag = 0x0;
  static dlsize_t cdata = 0x0;
  static dlsize_t cdata_count = 0x0;
  nid_t outnid;
  tag_t origtag;
  resp_combo_t tlresp_combo;
  tlrflit_t tlrespflit_received;
  tlxscflit_t tlxcmd_store_received;

  static count_t valex_memegress_cmdf_cntin = 0x0;
  static count_t valex_memegress_data_cntin = 0x0;
  static count_t valex_memegress_cmdf_tortyin = 0x0;
  static count_t valex_memegress_data_tortyin = 0x0;

  switch(smstate) {
        /*INIT loads CAPPTAG FIFO with tokens at initialization */
        case INIT: {
           if (enable == 0x1) {
              if (capptag_count == 0x200) {
                 smstate = GET_CMD;
              }
              lookup_out.write(capptag_count);
              capptag_count = capptag_count + 1;
           }
           break;
        }
        case GET_CMD: {
           if(!tlresp_fifo_in.empty() /*&& (enable == 0x1)*/) {
               //pull new response flit from fifo
        	   valex_memegress_cmdf_cnt  = valex_memegress_cmdf_cntin;
               valex_memegress_cmdf_cntin = valex_memegress_cmdf_cntin + 1;
               //Get Combo flit
               tlresp_combo = tlresp_fifo_in.read();
		       tlrespflit_received.range(37,0) = tlresp_combo.range(252,215);
		       tlxcmd_store_received.range(214,0) = tlresp_combo.range(214,0);
               tlresp.fromTLRespData(tlrespflit_received);
               cmdf.fromTLxCStore(tlxcmd_store_received);
               //Store Orig tag in case it will be needed later.
               localtag = tlresp.getRespCappTag();
               //Store response dl and cmd dl
               respdl = tlresp.getTLRespDl();
               //store original dl to work with the dp trick...
               origdl = cmdf.getDl();
               //get dp
               respdp = tlresp.getTLRespDp();
               /* NOTE: This is P9 specific here
				* and this behaviour is copied from memcpy3
				* design. For 256B requests only the host
				* might respond with 2 seperate 128B requests
				* instead of a single 256B one. It will use DP
				* to designate the second response (i.e. dp=1).
				* So if we receive a response with DL that does
				* not match the original command we assert keeplookup
				* to prevent eviction from lookuptables later. OpenCAPI
				* v3.0 spec allows any combination for dl/dp - but if
				* we check them currently this might hinder  timing closure.
                */
               keeplookupentry = (respdl != origdl) && (respdp == 0x0);
               //prepare flit for pushing further
               origtag = cmdf.getCappTagOrig();
               tlresp.setRespOrigCappTag(origtag);
               outnid = cmdf.getNetId();
               tlresp.setNetId(outnid);
               //TL Resp command handling below
               if (tlresp.getTLRespState().range(0,0)) {
                   //Annotating Resp code in the pgsize field -- which is not _currently_ used
            	   if(tlresp.TLRespIsXlateDone()) {
            	       cmdf.setTLxCmdRespAnno(0x0);
            	   } else {
                       cmdf.setTLxCmdRespAnno(tlresp.getRespCode());
            	   }
                   /* Remark: Rewriting the command capptag as it seems that it fails in generated verilog simulation
                    * to get written to BRAM during cmd issue step. The rest of the
                    * fields seem to be available correctly.
					*/
                   cmdf.setCappTagLocal(tlresp.getRespCappTag());
                   /* Write rty_cmd out to retry command
                    * Check if you will be pulling flits.
					*/
                   cdata.range(2,2) = origdl.range(1,1) & origdl.range(0,0); //dlt is 11 bit high
                   cdata.range(1,1) = origdl.range(1,1) & !origdl.range(0,0);//dlt is 10 bit high
                   cdata.range(0,0) = !origdl.range(1,1); //It is 01 or 00 then bit high
                   cdata_count = 0x1; //Initialize counter
                   valex_memegress_cmdf_torty = valex_memegress_cmdf_tortyin;
                   valex_memegress_cmdf_tortyin = valex_memegress_cmdf_tortyin + 1;
                   rtry_do_cmd_out.write(cmdf.toOCxFlit());
                   if(cmdf.TLxCmdIsWrite()) {
                       respdataflag = 0; //Don't pull flits from host data will come from ltables
                       smstate = GET_DATA;
                   }
               } else if (tlresp.getTLRespState().range(1,1)) {
                   //prepare state machine to pull flits
                   respdataflag = 1;
                   //Get information from response.
                   cdata.range(2,2) = respdl.range(1,1) & respdl.range(0,0); //dlt is 11 bit high
                   cdata.range(1,1) = respdl.range(1,1) & !respdl.range(0,0);//dlt is 10 bit high
                   cdata.range(0,0) = !respdl.range(1,1); //It is 01 or 00 then bit high
                   cdata_count = 0x1; //Initialize counter
                   smstate = GET_DATA;
                   network_cmd_out0.write(tlresp.toOCxFlit());
               } else if (tlresp.getTLRespState().range(2,2)) { //Flit with nodata (i.e WR_OK) or unrecoverable error.
                   //crossbar here -- WOK or others return capptag
            	   lookup_out.write(tlresp.getRespCappTag());
                   network_cmd_out0.write(tlresp.toOCxFlit());
               }

             }
             break;
        }
        case GET_DATA: {
            /* if respdataflag is set we are serving
             * fresh response data that come from the host.
             */
            if(respdataflag) {
                 if(!tlresp_data_in.empty()) {
                     if (cdata != 0x1) {
                         cdata = cdata - 1;
                     } else {
                         //Return capptag if allowed
                         if (!keeplookupentry) {
                             lookup_out.write(localtag);
                         }
                         smstate = GET_CMD;
                     }
                     valex_memegress_data_cnt = valex_memegress_data_cntin;
                     valex_memegress_data_cntin = valex_memegress_data_cntin + 1;
                     network_cmd_out0.write(tlresp_data_in.read());
                 }
             } else {
            	  valex_memegress_data_torty = valex_memegress_data_tortyin;
                  valex_memegress_data_tortyin = valex_memegress_data_tortyin + 1;
                  //Pulling dataflits from local store first flit at datalookup0
                  if (cdata_count == 0x1) {
                	 datar = data0lookup[(localtag/*^ADDR_MASK*/)];
                     rtry_do_cmd_out.write(datar);
                  } else if (cdata_count == 0x2) {
                	 datar = data1lookup[(localtag/*^ADDR_MASK*/)];
                     rtry_do_cmd_out.write(datar);
                  } else if (cdata_count == 0x3) {
                	 datar = data2lookup[(localtag/*^ADDR_MASK*/)];
                     rtry_do_cmd_out.write(datar);
                  } else { //cdata_count 0x4
                	 datar = data3lookup[(localtag/*^ADDR_MASK*/)];
                     rtry_do_cmd_out.write(datar);
                  }
                  if (cdata_count == cdata) {
                      //Done jump to GET_CMD again
                      smstate = GET_CMD;
                  }
                  cdata_count = cdata_count + 1;
             }
            break;
        }

        default: {break;}
   }; //end of switch

}



