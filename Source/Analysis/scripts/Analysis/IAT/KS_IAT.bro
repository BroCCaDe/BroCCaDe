#################################################################################
# Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murodch.edu.au)            #
#                                                                               #
# Redistribution and use in source and binary forms, with or without            #
# modification, are permitted provided that the following conditions are met:   #
#                                                                               #
# (1) Redistributions of source code must retain the above copyright            #
#     notice, this list of conditions and the following disclaimer.             #
#                                                                               #
# (2) Redistributions in binary form must reproduce the above copyright         #
#     notice, this list of conditions and the following disclaimer in           #
#     the documentation and/or other materials provided with the                #
#     distribution.                                                             #
#                                                                               #
# (3) Neither the name of Hendra Gunadi and/or Murdoch University, nor          #
#     the names of contributors may be used to endorse or promote               #
#     products derived from this software without specific prior written        #
#     permission.                                                               #
#                                                                               #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"   #
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE     #
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE    #
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     #
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           #
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF          #
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS      #
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       #
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       #
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE    #
# POSSIBILITY OF SUCH DAMAGE.                                                   #
#################################################################################

@load /opt/bro/lib/bro/plugins/FeatureExtraction/lib/bif
@load /opt/bro/lib/bro/plugins/FeatureExtraction_URG/lib/bif
@load /opt/bro/lib/bro/plugins/FeatureExtraction_PTunnel/lib/bif
@load /opt/bro/lib/bro/plugins/Training/lib/bif
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

event new_packet (c: connection, p: pkt_hdr)
{
	if ( p ?$ ip )
	{
		event PacketLength_feature_event(c$uid, c$id, p$ip$len);
	}
}


event connection_state_remove (c: connection)
{
	FeatureAnalysis::RemoveConn(c$uid);
}

event PacketLength_feature_event(UID:string, id:conn_id, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PACKET_LENGTH_SET, id);

	local aid : vector of FeatureAnalysis::Analysis_ID;
	aid[0] = FeatureAnalysis::KS_ANALYSIS;
	aid[1] = FeatureAnalysis::ENTROPY_ANALYSIS;
	aid[2] = FeatureAnalysis::CCE_ANALYSIS;
	aid[3] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
	aid[4] = FeatureAnalysis::AUTOCORRELATION_ANALYSIS;
	aid[5] = FeatureAnalysis::REGULARITY_ANALYSIS;

	FeatureAnalysis::AddFeature(UID, value, aid, PACKET_LENGTH);
	FeatureAnalysis::CalculateMetric();
}

event IAT::feature_event(UID:string, id:conn_id, value: double) {
	local aid : vector of FeatureAnalysis::Analysis_ID;
	aid[0] = FeatureAnalysis::KS_ANALYSIS;
	aid[1] = FeatureAnalysis::ENTROPY_ANALYSIS;
	aid[2] = FeatureAnalysis::CCE_ANALYSIS;
	aid[3] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
	aid[4] = FeatureAnalysis::AUTOCORRELATION_ANALYSIS;
	aid[5] = FeatureAnalysis::REGULARITY_ANALYSIS;

	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::IAT_SET, id);
	FeatureAnalysis::AddFeature(UID, value, aid, INTERARRIVAL_TIME);
	FeatureAnalysis::CalculateMetric();
}

event FeatureExtraction::URG_feature_event(UID : string, id : conn_id, URG_flag : count, URG_ptr : count) {
	local aid : vector of FeatureAnalysis::Analysis_ID;
	aid[0] = FeatureAnalysis::NULL_ANALYSIS;

	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::URGENT_SET, id);
	FeatureAnalysis::AddFeature(UID, URG_flag, aid, URG_FLAG);
	FeatureAnalysis::AddFeature(UID, URG_ptr, aid, URG_POINTER);
	FeatureAnalysis::CalculateMetric();
}

event FeatureExtraction::PTunnel_feature_event(UID : string, id : conn_id, payload : double)
{
	local aid : vector of FeatureAnalysis::Analysis_ID;
	aid[0] = FeatureAnalysis::NULL_ANALYSIS;

	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PTUNNEL_SET, id);
	FeatureAnalysis::AddFeature(UID, payload, aid, ICMP_PAYLOAD_4_BYTES);
	FeatureAnalysis::CalculateMetric();
}

event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, v : result_vector, conn_ID: conn_id)
{
	print(conn_ID);
	print (v);
#	for (i in v)
#		if (v[i]$value > 0.5) print(v[i]$value);
	DecisionTree::Classify(id, FeatureAnalysis::Extract_vector(v));
}

global training_set = set(FeatureAnalysis::IAT_SET, FeatureAnalysis::PACKET_LENGTH_SET);

event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, v : result_vector, conn_ID: conn_id)
{
	if (id in training_set)
	{
		FeatureTraining::ChangeRelation(id, "metrics");
		FeatureTraining::AddAttributes(id, "KS");
		FeatureTraining::AddAttributes(id, "Entropy");
		FeatureTraining::AddAttributes(id, "CCE");
		FeatureTraining::AddAttributes(id, "MultiModal");
		FeatureTraining::AddAttributes(id, "Autocorrelation");
		FeatureTraining::AddAttributes(id, "Regularity");
		FeatureTraining::AddClass(id, "CC");
		FeatureTraining::AddClass(id, "Non-CC");
		FeatureTraining::AddDataRow(id, FeatureAnalysis::Extract_vector(v), "Non-CC");
		FeatureTraining::print_training_data(id, "test_training.arff");
	}
}

#event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, v : result_vector, conn_ID: conn_id)
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

#event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, v : result_vector, conn_ID: conn_id)
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
