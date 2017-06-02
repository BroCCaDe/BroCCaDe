// See the file "COPYING" in the main distribution directory for copyright.
#include "Event.h"

#include "URG_parser.h"

#include "urgentpointer.bif.h"


using namespace plugin::FeatureExtraction_UrgentPointer;

URG_parser::URG_parser(Connection* conn)
: TCP_ApplicationAnalyzer ("URG_parser", conn) 
{ _conn = conn; }

URG_parser::~URG_parser()
{ }

void URG_parser::Init()
{
  Analyzer::Init();
}

void URG_parser::Done()
{
  Analyzer::Done();
}

void URG_parser::DeliverPacket(int len, const u_char* data, bool is_orig,
                                uint64 seq, const IP_Hdr* ip, int caplen)
{
  	//Gets Header and delivers packet to app analyzer

  	if ((data == 0) || (ip == 0)) printf("\tdata or ip header is null\n");
	Analyzer::DeliverPacket(len, data, is_orig, seq, ip, caplen);

	if (ip != 0) {
		const struct tcphdr* tp = (const struct tcphdr*) ip->Payload();
		printf("URG : %u, Pointer : %u\n", tp->urg, tp->urg_ptr);
		val_list *vl = new val_list;
		vl->append(new StringVal((_conn->GetUID()).Base62()));
		vl->append(new Val(tp->urg, TYPE_COUNT));
		vl->append(new Val(tp->urg_ptr, TYPE_COUNT));
		mgr.QueueEvent(FeatureExtraction::URG_feature_event, vl);
	}
}

