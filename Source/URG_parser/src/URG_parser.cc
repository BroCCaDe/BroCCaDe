// See the file "COPYING" in the main distribution directory for copyright.
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
#if DEBUG_H
		printf("data or ip header is null");
#endif
		return;
	}
	Analyzer::DeliverPacket(len, data, is_orig, seq, ip, caplen);

	if (ip != 0) {
		const struct tcphdr* tp = (const struct tcphdr*) ip->Payload();
#if DEBUG_H
		printf("URG : %u, Pointer : %u\n", tp->urg, tp->urg_ptr);
#endif
		// build the conn_id
		RecordVal* id_val = new RecordVal(conn_id);
		id_val->Assign(0, new AddrVal(_conn->OrigAddr()));
		id_val->Assign(1, new PortVal(ntohs(_conn->OrigPort()), TRANSPORT_TCP));
		id_val->Assign(2, new AddrVal(_conn->RespAddr()));
		id_val->Assign(3, new PortVal(ntohs(_conn->RespPort()), TRANSPORT_TCP));

		val_list *vl = new val_list;
		//vl->append(BuildConnVal());
		vl->append(new StringVal((_conn->GetUID()).Base62("C")));
		vl->append(id_val);
		vl->append(new Val(tp->urg, TYPE_COUNT));
		vl->append(new Val(tp->urg_ptr, TYPE_COUNT));
		mgr.QueueEvent(FeatureExtraction::URG_feature_event, vl);
	}
}

