type analysis_tag : enum {INTERARRIVAL_TIME, PACKET_LENGTH, URG_FLAG, URG_POINTER};

#type analysis_id : IAT::Analysis_ID;

type analysis_result : record
{
	value : double;
	tag : analysis_tag;
	aid : int;
};

type result_vector : vector of analysis_result;

export
{
	module FeatureAnalysis;

	type set_ID : enum {URGENT_SET, IAT_SET, PTUNNEL_SET};

	type decision_tree_args : record
	{
		URG_flag : double;
		URG_pointer : double;
	};

	type feature_vector : vector of double;
}
module GLOBAL;
