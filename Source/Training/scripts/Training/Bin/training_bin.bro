event connection_state_remove (c: connection)
{
	print(fmt("connection is removed %s", c$uid));
	BinTraining::RemoveConn(c$uid);
}

#event FeatureExtraction::URG_feature_event(UID : string, URG_flag : count, URG_ptr : count)
#{
#	print(fmt ("%s - URG_flag : %d - URG_ptr : %d", UID, URG_flag, URG_ptr));
#	BinTraining::add_feature(UID, URG_flag, URG_FLAG);
#	BinTraining::add_feature(UID, URG_ptr, URG_POINTER);
#}

event IAT::feature_event(UID:string, value: double)
{
	print(fmt ("UID : %s, iat : %f", UID, value));
	BinTraining::add_feature(UID, value, INTERARRIVAL_TIME);
}
