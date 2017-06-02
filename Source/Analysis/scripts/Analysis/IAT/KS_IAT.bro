@load /opt/bro/lib/bro/plugins/FeatureExtraction/lib/bif

event bro_init()
{
	print("Loading Analysis Script");
	DecisionTree::Init(3);
	DecisionTree::LoadModel(FeatureAnalysis::PTUNNEL_SET, "TreeModel-PingTunnel");
	DecisionTree::LoadModel(FeatureAnalysis::URGENT_SET, "TreeModel-URG");
	DecisionTree::LoadModel(FeatureAnalysis::IAT_SET, "TreeModel-PingTunnel"); # TODO: actual model
}

event connection_state_remove (c: connection)
{
	#print(fmt("connection is removed %s", c$uid));
	FeatureAnalysis::RemoveConn(c$uid);
}

event IAT::feature_event(UID:string, value: double)
{
	#print (UID);
	FeatureAnalysis::RegisterAnalysis(UID, value, FeatureAnalysis::KS_ANALYSIS, INTERARRIVAL_TIME);
	FeatureAnalysis::RegisterAnalysis(UID, value, FeatureAnalysis::ENTROPY_ANALYSIS, INTERARRIVAL_TIME);
	FeatureAnalysis::RegisterAnalysis(UID, value, FeatureAnalysis::CCE_ANALYSIS, INTERARRIVAL_TIME);
	FeatureAnalysis::RegisterAnalysis(UID, value, FeatureAnalysis::MULTIMODAL_ANALYSIS, INTERARRIVAL_TIME);
	FeatureAnalysis::RegisterAnalysis(UID, value, FeatureAnalysis::AUTOCORRELATION_ANALYSIS, INTERARRIVAL_TIME);
	FeatureAnalysis::CalculateMetric(FeatureAnalysis::IAT_SET);
}

event FeatureExtraction::URG_feature_event(UID : string, URG_flag : count, URG_ptr : count)
{
	print(fmt ("%s - URG_flag : %d - URG_ptr : %d", UID, URG_flag, URG_ptr));
	FeatureAnalysis::RegisterAnalysis(UID, URG_flag, FeatureAnalysis::NULL_ANALYSIS, URG_FLAG);
	FeatureAnalysis::RegisterAnalysis(UID, URG_ptr, FeatureAnalysis::NULL_ANALYSIS, URG_POINTER);
	FeatureAnalysis::CalculateMetric(FeatureAnalysis::URGENT_SET);
}

event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, v : result_vector)
{
	local vec : FeatureAnalysis::feature_vector;
	if (id == FeatureAnalysis::URGENT_SET)
	{
		local URG_flag = FeatureAnalysis::Extract(v, FeatureAnalysis::NULL_ANALYSIS, URG_FLAG);
		local URG_ptr = FeatureAnalysis::Extract(v, FeatureAnalysis::NULL_ANALYSIS, URG_POINTER);
		print(fmt("URG_flag: %f, URG_ptr: %f", URG_flag, URG_ptr));
		vec[|vec|] = URG_flag;
		vec[|vec|] = URG_ptr;
		DecisionTree::Classify(FeatureAnalysis::URGENT_SET, vec);
	}
	else if (id == FeatureAnalysis::IAT_SET)
	{
		local ks_value = FeatureAnalysis::Extract(v, FeatureAnalysis::KS_ANALYSIS, INTERARRIVAL_TIME);
		local entropy_value = FeatureAnalysis::Extract(v, FeatureAnalysis::ENTROPY_ANALYSIS, INTERARRIVAL_TIME);
		local cce_value = FeatureAnalysis::Extract(v, FeatureAnalysis::CCE_ANALYSIS, INTERARRIVAL_TIME);
		local mm_value = FeatureAnalysis::Extract(v, FeatureAnalysis::MULTIMODAL_ANALYSIS, INTERARRIVAL_TIME);
		local ac_value = FeatureAnalysis::Extract(v, FeatureAnalysis::AUTOCORRELATION_ANALYSIS, INTERARRIVAL_TIME);
		#print(fmt("KS: %f, Entropy: %f, CCE: %f, MM: %f, AC: %f", ks_value, entropy_value, cce_value, mm_value, ac_value));
		vec[|vec|] = cce_value;
		vec[|vec|] = entropy_value;
		vec[|vec|] = ks_value;
		DecisionTree::Classify(FeatureAnalysis::IAT_SET, vec);
	}
}

event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, v : result_vector)
{
	#print("vector metric event");
	local vec : FeatureAnalysis::feature_vector = FeatureAnalysis::Extract_vector(v);
	local name_vec : vector of string;
	if (id == FeatureAnalysis::URGENT_SET)
	{
		name_vec[|name_vec|] = "URG_flag";
		name_vec[|name_vec|] = "URG_pointer";
		DecisionTree::Classify(FeatureAnalysis::URGENT_SET, vec);
		DecisionTree::Classify_with_strings(FeatureAnalysis::URGENT_SET, vec, name_vec);
		local r : FeatureAnalysis::decision_tree_args = [$URG_flag=vec[0], $URG_pointer=vec[1]];
		DecisionTree::Classify_record(FeatureAnalysis::URGENT_SET, r);
	}
	else if (id == FeatureAnalysis::IAT_SET)
	{
		DecisionTree::Classify(FeatureAnalysis::IAT_SET, vec);
	}
}
