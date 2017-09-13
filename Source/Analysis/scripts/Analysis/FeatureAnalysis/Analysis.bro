#################################################################################
# Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murdoch.edu.au)            #
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

@load /opt/bro/lib/bro/plugins/Feature_IAT/lib/bif
@load /opt/bro/lib/bro/plugins/Feature_PTunnel/lib/bif
# @load /opt/bro/lib/bro/plugins/Feature_URG/lib/bif

global aid : vector of FeatureAnalysis::Analysis_ID;
global aid_null : vector of FeatureAnalysis::Analysis_ID = vector (FeatureAnalysis::NULL_ANALYSIS);

event bro_init()
{
	DecisionTree::LoadModel(FeatureAnalysis::IAT_SET, "TreeModel1");
    DecisionTree::LoadModel(FeatureAnalysis::TTL_SET, "TreeModel1");
	DecisionTree::LoadModel(FeatureAnalysis::PTUNNEL_SET, "TreeModel-PingTunnel");
	DecisionTree::LoadModel(FeatureAnalysis::URGENT_SET, "TreeModel-URG");
	DecisionTree::LoadModel(FeatureAnalysis::PACKET_LENGTH_SET, "TreeModel1");

    aid[|aid|] = FeatureAnalysis::KS_ANALYSIS;
#    aid[|aid|] = FeatureAnalysis::ENTROPY_ANALYSIS;
#    aid[|aid|] = FeatureAnalysis::CCE_ANALYSIS;
#    aid[|aid|] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
#    aid[|aid|] = FeatureAnalysis::AUTOCORRELATION_ANALYSIS;
#    aid[|aid|] = FeatureAnalysis::REGULARITY_ANALYSIS;
#    aid[|aid|] = FeatureAnalysis::NULL_ANALYSIS;

    FeatureAnalysis::SetStepSize(FeatureAnalysis::URGENT_SET, 1);
    FeatureAnalysis::SetStepSize(FeatureAnalysis::PTUNNEL_SET, 1);
    FeatureAnalysis::SetStepSize(FeatureAnalysis::TTL_SET, 50);
    FeatureAnalysis::SetStepSize(FeatureAnalysis::IAT_SET, 50);
    FeatureAnalysis::SetStepSize(FeatureAnalysis::PACKET_LENGTH_SET, 50);
#    FeatureAnalysis::SetKSWindowSize(FeatureAnalysis::TTL_SET, 500);
#    FeatureAnalysis::SetRegularityParameters(FeatureAnalysis::TTL_SET, 15, 100);
#    FeatureAnalysis::SetCCEPatternSize(FeatureAnalysis::TTL_SET, 5);
#    FeatureAnalysis::SetAutocorrelationLags(FeatureAnalysis::TTL_SET, 5);
#    FeatureAnalysis::SetKSWindowSize(FeatureAnalysis::IAT_SET, 500);
#    FeatureAnalysis::SetRegularityParameters(FeatureAnalysis::IAT_SET, 15, 100);
#    FeatureAnalysis::SetCCEPatternSize(FeatureAnalysis::IAT_SET, 5);
#    FeatureAnalysis::SetAutocorrelationLags(FeatureAnalysis::IAT_SET, 5);
    FeatureAnalysis::SetKSWindowSize(FeatureAnalysis::PACKET_LENGTH_SET, 500);
    FeatureAnalysis::SetRegularityParameters(FeatureAnalysis::PACKET_LENGTH_SET, 15, 100);
    FeatureAnalysis::SetCCEPatternSize(FeatureAnalysis::PACKET_LENGTH_SET, 5);
    FeatureAnalysis::SetAutocorrelationLags(FeatureAnalysis::PACKET_LENGTH_SET, 5);
    FeatureAnalysis::ConfigureInternalType();

    local aid_CCE : vector of FeatureAnalysis::Analysis_ID;
    aid_CCE[0] = FeatureAnalysis::CCE_ANALYSIS;
    local aid_EN_MM : vector of FeatureAnalysis::Analysis_ID;
    aid_EN_MM[0] = FeatureAnalysis::ENTROPY_ANALYSIS;
    aid_EN_MM[1] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
    FeatureAnalysis::LoadNormalData(TTL, "/home/hendra/Experiment/trace/performance_test/ALL_TTL_KS_100");
    FeatureAnalysis::LoadInterval(TTL, aid_CCE, "/home/hendra/Experiment/trace/performance_test/ALL_TTL_Interval_5");
    FeatureAnalysis::SetBinNull(TTL, aid_EN_MM, 256);
    FeatureAnalysis::LoadNormalData(INTERARRIVAL_TIME, "/home/hendra/Experiment/trace/performance_test/ALL_IAT_KS_100");
    FeatureAnalysis::LoadInterval(INTERARRIVAL_TIME, aid_CCE, "/home/hendra/Experiment/trace/performance_test/ALL_IAT_Interval_5");
    FeatureAnalysis::LoadInterval(INTERARRIVAL_TIME, aid_EN_MM, "/home/hendra/Experiment/trace/performance_test/ALL_IAT_Interval_5");
    FeatureAnalysis::LoadNormalData(PACKET_LENGTH, "/home/hendra/Experiment/trace/performance_test/ALL_PLEN_KS_100");
    FeatureAnalysis::LoadInterval(PACKET_LENGTH, aid_CCE, "/home/hendra/Experiment/trace/performance_test/ALL_PLEN_Interval_5");
    FeatureAnalysis::SetBinNull(PACKET_LENGTH, aid_EN_MM, 65536);
}

event connection_state_remove (c: connection)
{
	FeatureAnalysis::RemoveConn(c$uid);
}

event TTL_feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::TTL_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid, TTL);
	FeatureAnalysis::CalculateMetric();
}

event PacketLength_feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PACKET_LENGTH_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid, PACKET_LENGTH);
	FeatureAnalysis::CalculateMetric();
}

 event new_packet (c: connection, p: pkt_hdr)
 {
     if ( p ?$ ip )
 	{
#        event TTL_feature_event(c$uid, c$id, FeatureAnalysis::GetDirection(c$id$orig_h, p$ip$src), p$ip$ttl);
#        IAT::ExtractFeature(c$uid, c$id, FeatureAnalysis::GetDirection(c$id$orig_h, p$ip$src), c$duration);
        event PacketLength_feature_event(c$uid, c$id, FeatureAnalysis::GetDirection(c$id$orig_h, p$ip$src), p$ip$len);
     }
 }

# event icmp_echo_reply(c: connection, icmp: icmp_conn, id: count, seq: count, payload: string) {
#     FeatureExtraction::ExtractHeaderFeature(c$uid, c$id, FeatureAnalysis::BACKWARD, payload, 0, 4);}

# event icmp_echo_request(c: connection, icmp: icmp_conn, id: count, seq: count, payload: string) {
#     FeatureExtraction::ExtractHeaderFeature(c$uid, c$id, FeatureAnalysis::FORWARD, payload, 0, 4);}

event IAT::feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::IAT_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid, INTERARRIVAL_TIME);
	FeatureAnalysis::CalculateMetric();
}

# event FeatureExtraction::URG_feature_event(UID : string, id : conn_id, direction:FeatureAnalysis::Direction, 
#     URG_flag : count, URG_ptr : count) 
# {
# 	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::URGENT_SET, id, direction);
# 	FeatureAnalysis::AddFeature(URG_flag, aid_null, URG_FLAG);
# 	FeatureAnalysis::AddFeature(URG_ptr, aid_null, URG_POINTER);
# 	FeatureAnalysis::CalculateMetric();
# }

# event FeatureExtraction::PTunnel_feature_event(UID : string, id : conn_id, direction:FeatureAnalysis::Direction, 
#    features : FeatureAnalysis::feature_vector)
# {
#	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PTUNNEL_SET, id, direction);
#    for (idx in features)
#	    FeatureAnalysis::AddFeature(features[idx], aid_null, ICMP_PAYLOAD_4_BYTES);
#	FeatureAnalysis::CalculateMetric();
# }

event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, direction:FeatureAnalysis::Direction, 
    v : result_vector, conn_ID: conn_id)
{
#    DecisionTree::Classify(id, conn_ID, FeatureAnalysis::Extract_vector(v));
}
