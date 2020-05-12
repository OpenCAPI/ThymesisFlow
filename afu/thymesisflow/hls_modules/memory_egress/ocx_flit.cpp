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



/* General OCx Flit format to support all transformations

ocxflitbdi_t 

bit rng |---------------------|
512-512 |  BDI BIT            |  optional field 
511-355 |  TL/TLx Cmd Common  |  fields shared between TL/TLx commands
354-313 |  TLx Cmd extras     |  extra TLx fields pasid,actag, etc..
312-297 |  Local CAPPTAG      |  This holds that CAPPTAG to be used for local cmd issue.
313-xxx |  Network Header Id  |  custom network annotation depends on the network used
        |---------------------|  

The BDI is used to trigger SUE errors 

*/

#define TLRESP_CODE_RTY 0x2
#define TLRESP_CODE_PEND 0x4

#include "ocx_flit.hpp"

OCxFlit::OCxFlit()
{
   flit = 0;
   return;
}

void OCxFlit::fromOCxFlit(ocxflit_t fl)
{
   flit.range(511,0) = fl.range(511,0);
   return;
}

ocxflit_t OCxFlit::toOCxFlit(void)
{
   ocxflit_t fl;
 
   fl = flit.range(511,0);
   return fl;
}

void OCxFlit::fromOCxBdiFlit(ocxflitbdi_t fl)
{
   flit = fl;
   return;
}

ocxflitbdi_t OCxFlit::toOCxBdiFlit(void)
{
   return flit;
}

void OCxFlit::setBdi(valid_t bdi)
{
   flit.range(512,512) = bdi;
   return;
}


/* TL RESPONSE CMD FLIT PARSING STARTS HERE */

// TL Response flit to 512 OCXFlit for network transmission
void OCxFlit::fromTLRespData(tlrflit_t fl)
{
   //opcode
   flit.range(511,504) = fl.range(37,30); //8
   //capptag
   flit.range(503,488) = fl.range(29,14); //16
   //respcode
   flit.range(487,484) = fl.range(13,10); //4
   //dl 
   flit.range(483,482) = fl.range(9,8); //2
   //dp 
   flit.range(481,480) = fl.range(7,6); //2
   //pgsize 
   flit.range(479,474) = fl.range(5,0);//6
}

tlrflit_t OCxFlit::toTLRespData(void)
{
   tlrflit_t fl;
   //opcode
   fl.range(37,30) = flit.range(511,504); //8
   //capptag
   fl.range(29,14) = flit.range(503,488); //16
   //respcode
   fl.range(13,10) = flit.range(487,484); //4
   //dl
   fl.range(9,8) = flit.range(483,482); //2
   //dp
   fl.range(7,6) = flit.range(481,480); //2
   //pgsize
   fl.range(5,0) = flit.range(479,474);//6

   return fl;
}

tag_t OCxFlit::getRespCappTag(void)
{ 
  tag_t tag;
 
  tag = flit.range(503,488);

  return tag;
}

dl_t OCxFlit::getTLRespDl(void)
{
  dl_t dl;
 
  dl = flit.range(483,482);
  return dl;
}

dl_t OCxFlit::getTLRespDp(void)
{
  dp_t dp;
 
  dp = flit.range(481,480);
  return dp;
}


void OCxFlit::setRespOrigCappTag(tag_t tag)
{ 
  //set it where tag local is annotated
  flit.range(312,297)  = tag;
  return;
}


tag_t OCxFlit::getRespOrigCappTag(void)
{ 
  tag_t tag;

  tag = flit.range(312,297);  
  return tag;
}


code_t OCxFlit::getRespCode()
{
  code_t code;

  code = flit.range(487,484);
  return code;
}

opcode_t OCxFlit::getRespOpcode(void)
{
  opcode_t opc;
  
  opc = flit.range(511,504);
  return opc;

}

bool OCxFlit::TLRespIsXlateDone(void)
{
	return (flit.range(508,508) & flit.range(507,507));
}

bool OCxFlit::TLRespCodeIsFailure(void)
{
	return flit.range(487,487);
}

// Combinational logic that determines how the response flit should be handled.
sel_t OCxFlit::getTLRespState(void)
{
   sel_t sel;
   bool resp_fail;
   bool resp_rty_only;
   bool resp_rty_pend;

   //Determine if resp _opcodes_ indicate failure:
   resp_fail = flit.range(504,504); //The last opcode bit will be 1 if RD or WD failure
   //Determine if resp _codes_ indicate a command rtry:
   resp_rty_only =  (flit.range(487,484) == TLRESP_CODE_RTY);
   resp_rty_pend = ((flit.range(487,484) == TLRESP_CODE_RTY) || (flit.range(487,484) == TLRESP_CODE_PEND));
   /*Sel is 001 FAILURE_RTY => RTY: The Opcode is either FAIL or RTY with backoff.
     in case of XLATE_DONE the command will be immediately retried.
   */
   sel.range(0,0) = (((resp_fail == 1) && (resp_rty_only == 1)) || (flit.range(511,504) == TL_XLATE_DONE));
   /*010 CMD WITH DATA => NETWORK: The command flit is followed by data.
     Only on read family success
   */
   sel.range(1,1) = flit.range(506,506) & !flit.range(504,504);
   /*100 CMD WITH NO DATA => NETWORK: The command flit is not followed by data.
     Only on write family success OR unrecoverable failures of both read and write.
    */
   //RESP is strictly WRITEOK and if it is fail it is not either a retry or pend code is must be a SUE error code
   sel.range(2,2) =  (flit.range(507,507) & !flit.range(504,504))  | (resp_fail & (!resp_rty_pend));

   //else 000 => resp_pend or code failure only cases not captured.

   return sel;

}

/* TLX CMDFLIT PARSING*/

void OCxFlit::fromTLxCmd(tlxcflit_t fl)
{
    //opcode
    flit.range(511,504) = fl.range(198,191);//8
    //ea
    flit.range(503,440) = fl.range(190,127); //64
    //tag
    flit.range(439,424) = fl.range(126,111); // 16
    //dl 
    flit.range(423,422) = fl.range(110,109);//2
    //pl 
    flit.range(421,419) = fl.range(108,106);//3
    //be 
    flit.range(418,355) = fl.range(105,42);//64
    //actag
    flit.range(354,343) = fl.range(41,30);//12
    //streamid
    flit.range(342,339) = fl.range(29,26); //4 
    //pasid
    flit.range(338,319) = fl.range(25,6);//20
    //pgsize
    flit.range(318,313) = fl.range(5,0);//6
}

void OCxFlit::setTLxCmdRespAnno(code_t cd)
{
  flit.range(318,313) = cd;
}

code_t OCxFlit::getTLxCmdRespAnno(void)
{ 
  code_t cd;

  cd = flit.range(318,313);  
  return cd;
}

/*pulls TLx cmd for final issue*/
tlxcflit_t OCxFlit::toTLxCmdFlit(void)
{
    tlxcflit_t fl;

    //opcode
    fl.range(198,191) = flit.range(511,504);//8
    //ea
    fl.range(190,127) = flit.range(503,440); //64
    //tag -- important this is the local tag to use
    //which lies after pgsize and before netid
    fl.range(126,111) = flit.range(312,297); // 16
    //dl 
    fl.range(110,109) = flit.range(423,422);//2
    //pl 
    fl.range(108,106) = flit.range(421,419);//3
    //be 
    fl.range(105,42) = flit.range(418,355);//64
    //actag
    fl.range(41,30) = flit.range(354,343);//12
    //streamid
    fl.range(29,26) = flit.range(342,339); //4 
    //pasid
    fl.range(25,6) = flit.range(338,319);//20
    //pgsize
    fl.range(5,0) = flit.range(318,313); //6
  
    return fl;

}

//Stores the TLxcommand in a lookup table for retries
void OCxFlit::fromTLxCStore(tlxscflit_t fl)
{
    //This holds the local capptag used
    flit.range(312,297) = fl.range(214,199);//16
    //opcode
    flit.range(511,504) = fl.range(198,191);//8
    //ea
    flit.range(503,440) = fl.range(190,127); //64
    //tag
    flit.range(439,424) = fl.range(126,111); // 16
    //dl 
    flit.range(423,422) = fl.range(110,109);//2
    //pl 
    flit.range(421,419) = fl.range(108,106);//3
    //be 
    flit.range(418,355) = fl.range(105,42);//64
    //actag
    flit.range(354,343) = fl.range(41,30);//12
    //streamid
    flit.range(342,339) = fl.range(29,26); //4 
    //pasid
    flit.range(338,319) = fl.range(25,6);//20
    //netid    
    flit.range(296,291) = fl.range(5,0);//6
}


tlxscflit_t OCxFlit::toTLxCStore(void)
{
    tlxcflit_t fl;

    //This holds the local capptag used
    fl.range(214,199) = flit.range(312,297);//16 
    //opcode
    fl.range(198,191) = flit.range(511,504);//8
    //ea
    fl.range(190,127) = flit.range(503,440); //64
    //tag
    fl.range(126,111) = flit.range(439,424); // 16
    //dl 
    fl.range(110,109) = flit.range(423,422);//2
    //pl 
    fl.range(108,106) = flit.range(421,419);//3
    //be 
    fl.range(105,42) = flit.range(418,355);//64
    //actag
    fl.range(41,30) = flit.range(354,343);//12
    //streamid
    fl.range(29,26) = flit.range(342,339); //4 
    //pasid
    fl.range(25,6) = flit.range(338,319);//20
    //netid
    fl.range(5,0) = flit.range(296,291); //6
  
    return fl;

}


//Check if cmd opcode is write command
bool OCxFlit::TLxCmdIsWrite(void)
{
   //According to Opencapi TLx command codes
   //Write family opcodes have bit 5 high only.
   return flit.range(509,509);
}

//Check if cmd opcode is write command
bool OCxFlit::TLxCmdIsRead(void)
{
   //According to Opencapi TLx command codes
   //Read family opcodes have bit 5 high only and not bit 7 (assign actag)
   return flit.range(508,508);
}

bool OCxFlit::TLxCmdIsAssignActag(void)
{
  return flit.range(510,510);
}

dl_t OCxFlit::getDl(void)
{
  dl_t dl;  

  dl =  flit.range(423,422);
  return dl;
}

void OCxFlit::setDl(dl_t dl)
{
  flit.range(423,422) = dl;
  return;
}

void OCxFlit::setBe(addr_t be)
{
  flit.range(418,355) = be;
  return;
}

void OCxFlit::setOpcode(opcode_t opc)
{
  flit.range(511,504) = opc;
  return;
}

void OCxFlit::setEA(addr_t addr)
{
  flit.range(503,440) = addr;
  return;
}

void OCxFlit::setPasid(pasid_t pasid)
{
  flit.range(338,319) = pasid;
  return;
}

void OCxFlit::setActag(actag_t ac)
{
  flit.range(354,343) = ac;
  return;
}

void OCxFlit::setCappTagLocal(tag_t tag)
{
   flit.range(312,297) = tag;
   return;
}

tag_t OCxFlit::getCappTagLocal(void)
{
  tag_t tag;
  
  tag = flit.range(312,297);
  return tag;
}

tag_t OCxFlit::getCappTagOrig(void)
{
  tag_t tag;

  tag =  flit.range(439,424);
  return tag;
}

void OCxFlit::setCappTagOrig(tag_t tag)
{

  flit.range(439,424) = tag;
  return;
}


/* Network handling function functions*/

//returns the network identifier
nid_t OCxFlit::getNetId(void)
{
   nid_t netid;
 
   netid = flit.range(296,291);
   return netid;
}

//sets the network identifier
void OCxFlit::setNetId(nid_t netid)
{
   flit.range(296,291) = netid;
   return;
}




/*DATA Flit handling same across all ocxflit_t formats */



