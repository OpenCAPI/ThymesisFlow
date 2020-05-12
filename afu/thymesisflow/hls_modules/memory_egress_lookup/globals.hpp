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

#ifndef GLOBALS_HH
#define GLOBALS_HH

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdint.h>
#include <cstdlib>
#include <hls_stream.h>
#include <ap_int.h>

using namespace hls;

#define TL_RD_RESPOK 0x4
#define TL_RD_RESPFAIL 0x5
#define TL_WR_RESPOK 0x8
#define TL_WR_RESPFAIL 0x9
#define TL_XLATE_DONE 0x18


#define TLRESP_DATA  0x2
#define TLRESP_CODE_RTY 0x2
#define TLRESP_CODE_PEND 0x8

#define TLRESP_RTY 0x1
#define TLRESP_DATA 0x2

#define LOOKUPSIZE 512

#endif
