event new_packet (c: connection, p: pkt_hdr)
{
	IAT::ExtractFeature(c$uid, c$id, c$duration);
	if ( p ?$ ip )
	{
		event PacketLength_feature_event(c$uid, c$id, p$ip$len);
	}
}


