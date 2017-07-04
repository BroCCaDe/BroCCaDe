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

#@load /opt/bro/lib/bro/plugins/Feature_IAT/lib/bif
#@load /opt/bro/lib/bro/plugins/Feature_URG/lib/bif
#@load /opt/bro/lib/bro/plugins/Feature_PTunnel/lib/bif
# @load /opt/bro/lib/bro/plugins/Training/lib/bif

global aid_full : vector of FeatureAnalysis::Analysis_ID;
global aid_null : vector of FeatureAnalysis::Analysis_ID;
global aid_one : vector of FeatureAnalysis::Analysis_ID;

event bro_init()
{
	print("Loading Analysis Script");
	DecisionTree::Init(4);
	DecisionTree::LoadModel(FeatureAnalysis::PTUNNEL_SET, "TreeModel-PingTunnel");
	DecisionTree::LoadModel(FeatureAnalysis::URGENT_SET, "TreeModel-URG");
	DecisionTree::LoadModel(FeatureAnalysis::IAT_SET, "TreeModel-IAT");
	DecisionTree::LoadModel(FeatureAnalysis::PACKET_LENGTH_SET, "TreeModel-IAT");
#    DecisionTree::LoadModel(FeatureAnalysis::PACKET_LENGTH_SET, "TreeModel-IAT_KS");

    aid_full[0] = FeatureAnalysis::KS_ANALYSIS;
	aid_full[1] = FeatureAnalysis::ENTROPY_ANALYSIS;
	aid_full[2] = FeatureAnalysis::CCE_ANALYSIS;
	aid_full[3] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
	aid_full[4] = FeatureAnalysis::AUTOCORRELATION_ANALYSIS;
	aid_full[5] = FeatureAnalysis::REGULARITY_ANALYSIS;

    aid_null[0] = FeatureAnalysis::NULL_ANALYSIS;

    aid_one[0] = FeatureAnalysis::KS_ANALYSIS;
#	aid_one[0] = FeatureAnalysis::ENTROPY_ANALYSIS;
#	aid_one[0] = FeatureAnalysis::CCE_ANALYSIS;
#	aid_one[0] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
#	aid_one[0] = FeatureAnalysis::AUTOCORRELATION_ANALYSIS;
#	aid_one[0] = FeatureAnalysis::REGULARITY_ANALYSIS;

    FeatureAnalysis::ConfigureInternalType();
}

event connection_state_remove (c: connection)
{
	FeatureAnalysis::RemoveConn(c$uid);
}

event PacketLength_dummy_event(UID:string, id:conn_id, value:double) {}

event PacketLength_feature_event(UID:string, id:conn_id, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PACKET_LENGTH_SET, id);
	FeatureAnalysis::AddFeature(UID, value, aid_full, PACKET_LENGTH);
#	FeatureAnalysis::AddFeature(UID, value, aid_one, PACKET_LENGTH);
	FeatureAnalysis::CalculateMetric();
}

event new_packet (c: connection, p: pkt_hdr)
{
	if ( p ?$ ip )
	{
		event PacketLength_feature_event(c$uid, c$id, p$ip$len);
#		event PacketLength_dummy_event(c$uid, c$id, p$ip$len);
	}
}

event IAT::feature_event(UID:string, id:conn_id, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::IAT_SET, id);
#	FeatureAnalysis::AddFeature(UID, value, aid_full, INTERARRIVAL_TIME);
    FeatureAnalysis::AddFeature(UID, value, aid_one, INTERARRIVAL_TIME);
	FeatureAnalysis::CalculateMetric();
}

event FeatureExtraction::URG_feature_event(UID : string, id : conn_id, URG_flag : count, URG_ptr : count) 
{
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::URGENT_SET, id);
	FeatureAnalysis::AddFeature(UID, URG_flag, aid_null, URG_FLAG);
	FeatureAnalysis::AddFeature(UID, URG_ptr, aid_null, URG_POINTER);
	FeatureAnalysis::CalculateMetric();
}

event FeatureExtraction::PTunnel_feature_event(UID : string, id : conn_id, payload : double)
{
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PTUNNEL_SET, id);
	FeatureAnalysis::AddFeature(UID, payload, aid_null, ICMP_PAYLOAD_4_BYTES);
	FeatureAnalysis::CalculateMetric();
}

event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, v : result_vector, conn_ID: conn_id)
{
	DecisionTree::Classify(id, conn_ID, FeatureAnalysis::Extract_vector(v));
}
