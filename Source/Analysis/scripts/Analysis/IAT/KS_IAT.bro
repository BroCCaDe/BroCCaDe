@load /opt/bro/lib/bro/plugins/FeatureExtraction/lib/bif
@load /opt/bro/lib/bro/plugins/FeatureExtraction_URG/lib/bif
@load /opt/bro/lib/bro/plugins/FeatureExtraction_PTunnel/lib/bif
@load /opt/bro/lib/bro/plugins/FeatureExtraction_PTunnel/scripts/PTunnel/FeatureExtraction

event bro_init()
{
	print("Loading Analysis Script");
	DecisionTree::Init(4);
	DecisionTree::LoadModel(FeatureAnalysis::PTUNNEL_SET, "TreeModel-PingTunnel");
	DecisionTree::LoadModel(FeatureAnalysis::URGENT_SET, "TreeModel-URG");
	DecisionTree::LoadModel(FeatureAnalysis::IAT_SET, "TreeModel-IAT");
	DecisionTree::LoadModel(FeatureAnalysis::PACKET_LENGTH_SET, "TreeModel-IAT");
}

event connection_state_remove (c: connection)
{
	FeatureAnalysis::RemoveConn(c$uid);
}

event PacketLength_feature_event(UID:string, id:conn_id, value: double) {
#	print (fmt("LENGTH : %s", UID));
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PACKET_LENGTH_SET, id);

	local aid : vector of FeatureAnalysis::Analysis_ID;
	aid[0] = FeatureAnalysis::KS_ANALYSIS;
	aid[1] = FeatureAnalysis::ENTROPY_ANALYSIS;
	aid[2] = FeatureAnalysis::CCE_ANALYSIS;
	aid[3] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
	aid[4] = FeatureAnalysis::AUTOCORRELATION_ANALYSIS;
	aid[5] = FeatureAnalysis::REGULARITY_ANALYSIS;

	FeatureAnalysis::AddFeature(UID, value, aid, PACKET_LENGTH);

#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::KS_ANALYSIS, PACKET_LENGTH);
#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::ENTROPY_ANALYSIS, PACKET_LENGTH);
#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::CCE_ANALYSIS, PACKET_LENGTH);
#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::MULTIMODAL_ANALYSIS, PACKET_LENGTH);
#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::AUTOCORRELATION_ANALYSIS, PACKET_LENGTH);
#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::REGULARITY_ANALYSIS, PACKET_LENGTH);
	FeatureAnalysis::CalculateMetric();
}

event IAT::feature_event(UID:string, id:conn_id, value: double) {
#	print (fmt("IAT : %s", UID));
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::IAT_SET, id);

	local aid : vector of FeatureAnalysis::Analysis_ID;
	aid[0] = FeatureAnalysis::KS_ANALYSIS;
	aid[1] = FeatureAnalysis::ENTROPY_ANALYSIS;
	aid[2] = FeatureAnalysis::CCE_ANALYSIS;
	aid[3] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
	aid[4] = FeatureAnalysis::AUTOCORRELATION_ANALYSIS;
	aid[5] = FeatureAnalysis::REGULARITY_ANALYSIS;

	FeatureAnalysis::AddFeature(UID, value, aid, INTERARRIVAL_TIME);

#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::KS_ANALYSIS, INTERARRIVAL_TIME);
#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::ENTROPY_ANALYSIS, INTERARRIVAL_TIME);
#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::CCE_ANALYSIS, INTERARRIVAL_TIME);
#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::MULTIMODAL_ANALYSIS, INTERARRIVAL_TIME);
#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::AUTOCORRELATION_ANALYSIS, INTERARRIVAL_TIME);
#	FeatureAnalysis::AddFeature(UID, value, FeatureAnalysis::REGULARITY_ANALYSIS, INTERARRIVAL_TIME);
	FeatureAnalysis::CalculateMetric();
}

event FeatureExtraction::URG_feature_event(UID : string, id : conn_id, URG_flag : count, URG_ptr : count) {
#	print (fmt("URG : %s", UID));

	local aid : vector of FeatureAnalysis::Analysis_ID;
	aid[0] = FeatureAnalysis::NULL_ANALYSIS;

	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::URGENT_SET, id);
	FeatureAnalysis::AddFeature(UID, URG_flag, aid, URG_FLAG);
	FeatureAnalysis::AddFeature(UID, URG_ptr, aid, URG_POINTER);
	FeatureAnalysis::CalculateMetric();
}

event FeatureExtraction::PTunnel_feature_event(UID : string, id : conn_id, payload : double)
{
#	print (fmt("PTUNNEL : %s", UID));

	local aid : vector of FeatureAnalysis::Analysis_ID;
	aid[0] = FeatureAnalysis::NULL_ANALYSIS;

	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PTUNNEL_SET, id);
	FeatureAnalysis::AddFeature(UID, payload, aid, ICMP_PAYLOAD_4_BYTES);
	FeatureAnalysis::CalculateMetric();
}

event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, v : result_vector)
{
#	print (fmt("ID is %d : ", id));
#	print (v);
	for (i in v)
		if (v[i]$value > 0.5) print(v[i]$value);
	DecisionTree::Classify(id, FeatureAnalysis::Extract_vector(v));
}

#event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, v : result_vector)
#{
#	local vec : FeatureAnalysis::feature_vector;
#	if (id == FeatureAnalysis::URGENT_SET)
#	{
#		local URG_flag = FeatureAnalysis::Extract(v, FeatureAnalysis::NULL_ANALYSIS, URG_FLAG);
#		local URG_ptr = FeatureAnalysis::Extract(v, FeatureAnalysis::NULL_ANALYSIS, URG_POINTER);
#		print(fmt("URG_flag: %f, URG_ptr: %f", URG_flag, URG_ptr));
#		vec[|vec|] = URG_flag;
#		vec[|vec|] = URG_ptr;
#		DecisionTree::Classify(FeatureAnalysis::URGENT_SET, vec);
#	}
#	else if (id == FeatureAnalysis::IAT_SET)
#	{
#		local ks_value = FeatureAnalysis::Extract(v, FeatureAnalysis::KS_ANALYSIS, INTERARRIVAL_TIME);
#		local entropy_value = FeatureAnalysis::Extract(v, FeatureAnalysis::ENTROPY_ANALYSIS, INTERARRIVAL_TIME);
#		local cce_value = FeatureAnalysis::Extract(v, FeatureAnalysis::CCE_ANALYSIS, INTERARRIVAL_TIME);
#		local mm_value = FeatureAnalysis::Extract(v, FeatureAnalysis::MULTIMODAL_ANALYSIS, INTERARRIVAL_TIME);
#		local ac_value = FeatureAnalysis::Extract(v, FeatureAnalysis::AUTOCORRELATION_ANALYSIS, INTERARRIVAL_TIME);
#		#print(fmt("KS: %f, Entropy: %f, CCE: %f, MM: %f, AC: %f", ks_value, entropy_value, cce_value, mm_value, ac_value));
#		vec[|vec|] = cce_value;
#		vec[|vec|] = entropy_value;
#		vec[|vec|] = ks_value;
#		DecisionTree::Classify(FeatureAnalysis::IAT_SET, vec);
#	}
#}

#event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, v : result_vector)
#{
#	#print("vector metric event");
#	local vec : FeatureAnalysis::feature_vector = FeatureAnalysis::Extract_vector(v);
#	local name_vec : vector of string;
#	if (id == FeatureAnalysis::URGENT_SET)
#	{
#		name_vec[|name_vec|] = "URG_flag";
#		name_vec[|name_vec|] = "URG_pointer";
#		DecisionTree::Classify(FeatureAnalysis::URGENT_SET, vec);
#		DecisionTree::Classify_with_strings(FeatureAnalysis::URGENT_SET, vec, name_vec);
#		local r : FeatureAnalysis::decision_tree_args = [$URG_flag=vec[0], $URG_pointer=vec[1]];
#		DecisionTree::Classify_record(FeatureAnalysis::URGENT_SET, r);
#	}
#	else if (id == FeatureAnalysis::IAT_SET)
#	{
#		DecisionTree::Classify(FeatureAnalysis::IAT_SET, vec);
#	}
#}
