event new_packet (c: connection, p: pkt_hdr)
{
	IAT::ExtractFeature(c$uid, c$duration);
}


