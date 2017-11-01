/*******************************************************************************\
* Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murdoch.edu.au)            *
*                                                                               *
* Redistribution and use in source and binary forms, with or without            *
* modification, are permitted provided that the following conditions are met:   *
*                                                                               *
* (1) Redistributions of source code must retain the above copyright            *
*     notice, this list of conditions and the following disclaimer.             *
*                                                                               *
* (2) Redistributions in binary form must reproduce the above copyright         *
*     notice, this list of conditions and the following disclaimer in           *
*     the documentation and/or other materials provided with the                *
*     distribution.                                                             *
*                                                                               *
* (3) Neither the name of Hendra Gunadi and/or Murdoch University, nor          *
*     the names of contributors may be used to endorse or promote               *
*     products derived from this software without specific prior written        *
*     permission.                                                               *
*                                                                               *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"   *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE     *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE    *
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF          *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS      *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE    *
* POSSIBILITY OF SUCH DAMAGE.                                                   *
*                                                                               *
* URG_parser.cc : Implements URG_parser.h                                       *
\*******************************************************************************/

#include "Event.h"              

#include "URG_parser.h"
#include "Conn.h"
#include "urgentpointer.bif.h"

using namespace plugin::FeatureExtraction_UrgentPointer;

void URG_parser::DeliverPacket(int len, const u_char* data, bool is_orig,
                                uint64 seq, const IP_Hdr* ip, int caplen)
{
  	//Gets Header and delivers packet to app analyzer

  	if ((data == NULL) || (ip == NULL)) 
	{
#ifdef DEBUG_H
		printf("data or ip header is null");
#endif
		return;
	}
	Analyzer::DeliverPacket(len, data, is_orig, seq, ip, caplen);

	if (ip != 0) {  
		const struct tcphdr* tp = (const struct tcphdr*) ip->Payload();
#ifdef DEBUG_H
		printf("URG : %u, Pointer : %u\n", tp->urg, tp->urg_ptr);
#endif
		// build the conn_id
		RecordVal* id_val = new RecordVal(conn_id);
		id_val->Assign(0, new AddrVal(_conn->OrigAddr()));
		id_val->Assign(1, new PortVal(ntohs(_conn->OrigPort()), TRANSPORT_TCP));
		id_val->Assign(2, new AddrVal(_conn->RespAddr()));
		id_val->Assign(3, new PortVal(ntohs(_conn->RespPort()), TRANSPORT_TCP));

		val_list *vl = new val_list;
		vl->append(new StringVal((_conn->GetUID()).Base62("C")));   // pass the UID
		vl->append(id_val);                                         // conn_ID
        vl->append(new Val((is_orig) ? 0 : 1, TYPE_ENUM));          // Direction
		vl->append(new Val(tp->urg, TYPE_COUNT));                   // URG_flag
		vl->append(new Val(ntohs(tp->urg_ptr), TYPE_COUNT));        // URG_ptr
		mgr.QueueEvent(FeatureExtraction::URG_feature_event, vl);
	}
}

