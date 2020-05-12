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


#ifndef OCX_FLIT_HH
#define OCX_FLIT_HH

/* This class provides all flit parsing functions
   and preserves the datapath maximum size to 512 
   (standard opencapi flit size) and in some cases
   513 (flit size + Bad data indicator bit).
   The only types used here are:
   ocxflitbdi_t: 513 bits (ocxflit + bdi) 
   ocxflit_t: 512 bits ocxflit
   
   according to context. 
   
 */

#include "globals.hpp"
#include "types.hpp"

class OCxFlit {

    private:
          ocxflitbdi_t flit;

    public:
          OCxFlit();
          void fromOCxFlit(ocxflit_t fl);
          ocxflit_t toOCxFlit(void); 
          void fromOCxBdiFlit(ocxflitbdi_t fl);
          ocxflitbdi_t toOCxBdiFlit(void);
          void setBdi(valid_t bdi);
 
          //TL Response CMD flit parsing
          void fromTLRespData(tlrflit_t fl);
          tlrflit_t toTLRespData(void);
          sel_t getTLRespState(void);
          dl_t getTLRespDl(void);
          dp_t getTLRespDp(void);
          tag_t getRespCappTag(void); 
          void setRespOrigCappTag(tag_t tag); 
          tag_t getRespOrigCappTag(void);      
          code_t getRespCode(void);
          opcode_t getRespOpcode(void);
          bool TLRespIsXlateDone(void);
          bool TLRespCodeIsFailure(void);


          //TLx Command CMD flit parsing
          void fromTLxCmd(tlxcflit_t fl);
          tlxcflit_t toTLxCmdFlit(void);
          void fromTLxCStore(tlxscflit_t fl);
          tlxscflit_t toTLxCStore(void);
          bool TLxCmdIsWrite(void);
          bool TLxCmdIsRead(void);
          bool TLxCmdIsAssignActag(void);
          dl_t getDl(void);
          void setOpcode(opcode_t opc);
          void setDl(dl_t dl);
          void setEA(addr_t addr);
          void setBe(addr_t be);
          void setTLxCmdRespAnno(code_t cd);
          void setActag(actag_t ac);
          void setPasid(pasid_t pasid);
          code_t getTLxCmdRespAnno(void);
          tag_t getCappTagLocal(void);
          void setCappTagLocal(tag_t tag);
          void setCappTagOrig(tag_t tag);
          tag_t getCappTagOrig(void);

          //Network part
          nid_t getNetId(void);
          void setNetId(nid_t netid);
};




#endif

