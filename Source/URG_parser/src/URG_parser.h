#ifndef AUX_PLUGINS_HEADER_URGENTPOINTER_H
#define AUX_PLUGINS_HEADER_URGENTPOINTER_H

#include "analyzer/protocol/tcp/TCP.h"
#include "urgentpointer.bif.h"

namespace plugin { namespace FeatureExtraction_UrgentPointer {

class URG_parser : public analyzer::tcp::TCP_ApplicationAnalyzer {
public:
	URG_parser(Connection* conn);
	virtual ~URG_parser();

	virtual void Init();
	virtual void Done();

	static analyzer::Analyzer* Instantiate(Connection* conn)
		{ printf("URG_parser::Instantiate()\n"); return new URG_parser(conn); }
protected:
	virtual void DeliverPacket(int len, const u_char* data, bool is_orig,
                                uint64 seq, const IP_Hdr* ip, int caplen);

private:
	Connection *_conn;

};

} } 

#endif
