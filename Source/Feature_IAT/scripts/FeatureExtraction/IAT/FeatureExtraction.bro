event new_packet (c: connection, p: pkt_hdr)
{
    if (p ?$ ip)
    {
	    IAT::ExtractFeature(c$uid, c$id, get_direction(c$id$orig_h, p$ip$src), p$c$duration);
    }
}

