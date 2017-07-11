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
@load /opt/bro/lib/bro/plugins/Feature_PTunnel/lib/bif
@load /opt/bro/lib/bro/plugins/Training/lib/bif
#@load /opt/bro/lib/bro/plugins/BinTraining/lib/bif
#@load /opt/bro/lib/bro/plugins/BinTraining/scripts/Training/Bin/training_bin.bro

global aid_full : vector of FeatureAnalysis::Analysis_ID;
global aid_null : vector of FeatureAnalysis::Analysis_ID;
global aid_one : vector of FeatureAnalysis::Analysis_ID;

event bro_init()
{
	print("Loading Analysis Script");
	DecisionTree::Init(5);
	DecisionTree::LoadModel(FeatureAnalysis::PTUNNEL_SET, "TreeModel-PingTunnel");
	DecisionTree::LoadModel(FeatureAnalysis::URGENT_SET, "TreeModel-URG");
	DecisionTree::LoadModel(FeatureAnalysis::IAT_SET, "TreeModel-IAT");
	DecisionTree::LoadModel(FeatureAnalysis::PACKET_LENGTH_SET, "TreeModel-IAT");
#    DecisionTree::LoadModel(FeatureAnalysis::PACKET_LENGTH_SET, "TreeModel-IAT_KS");
    DecisionTree::LoadModel(FeatureAnalysis::TTL_SET, "TreeModel-TTL");

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
    FeatureAnalysis::LoadNormalData(TTL, "KS_TTL_128");  
    local aid_CCE : vector of FeatureAnalysis::Analysis_ID;
    aid_CCE[0] = FeatureAnalysis::CCE_ANALYSIS;
    local aid_EN_MM : vector of FeatureAnalysis::Analysis_ID;
    aid_EN_MM[0] = FeatureAnalysis::ENTROPY_ANALYSIS;
    aid_EN_MM[1] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
    FeatureAnalysis::LoadInterval(TTL, aid_CCE, "Interval_TTL");
    FeatureAnalysis::SetBinNull(TTL, aid_EN_MM, 256);

#	FeatureTraining::ChangeRelation(FeatureAnalysis::IAT_SET, "metrics");
#	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "KS");
#	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "Entropy");
#	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "CCE");
#	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "MultiModal");
#	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "Autocorrelation");
#	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "Regularity");
#	FeatureTraining::AddClass(FeatureAnalysis::IAT_SET, "CC");
#	FeatureTraining::AddClass(FeatureAnalysis::IAT_SET, "Non-CC");

	FeatureTraining::ChangeRelation(FeatureAnalysis::TTL_SET, "metrics");
	FeatureTraining::AddAttributes(FeatureAnalysis::TTL_SET, "KS");
	FeatureTraining::AddAttributes(FeatureAnalysis::TTL_SET, "Entropy");
	FeatureTraining::AddAttributes(FeatureAnalysis::TTL_SET, "CCE");
	FeatureTraining::AddAttributes(FeatureAnalysis::TTL_SET, "MultiModal");
	FeatureTraining::AddAttributes(FeatureAnalysis::TTL_SET, "Autocorrelation");
	FeatureTraining::AddAttributes(FeatureAnalysis::TTL_SET, "Regularity");
	FeatureTraining::AddClass(FeatureAnalysis::TTL_SET, "CC");
	FeatureTraining::AddClass(FeatureAnalysis::TTL_SET, "Non-CC");
}

event connection_state_remove (c: connection)
{
	FeatureAnalysis::RemoveConn(c$uid);
}

function get_direction(a : addr, b : addr) : FeatureAnalysis::Direction
{
    if (a == b) return FeatureAnalysis::FORWARD;
    return FeatureAnalysis::BACKWARD;
}

event PacketLength_feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PACKET_LENGTH_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid_full, PACKET_LENGTH);
#	FeatureAnalysis::AddFeature(value, aid_one, PACKET_LENGTH);
	FeatureAnalysis::CalculateMetric();
}

event TTL_feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::TTL_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid_full, TTL);
#	FeatureAnalysis::AddFeature(value, aid_one, TTL);
	FeatureAnalysis::CalculateMetric();
}

event new_packet (c: connection, p: pkt_hdr)
{
	if ( p ?$ ip )
	{
#		event PacketLength_feature_event(c$uid, c$id, p$ip$len);
#		event TTL_feature_event(c$uid, c$id, get_direction(c$id$orig_h, p$ip$src), p$ip$ttl);
	}
}

event IAT::feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::IAT_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid_full, INTERARRIVAL_TIME);
#    FeatureAnalysis::AddFeature(value, aid_one, INTERARRIVAL_TIME);
	FeatureAnalysis::CalculateMetric();
}

event FeatureExtraction::URG_feature_event(UID : string, id : conn_id, direction:FeatureAnalysis::Direction, 
    URG_flag : count, URG_ptr : count) 
{
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::URGENT_SET, id, direction);
	FeatureAnalysis::AddFeature(URG_flag, aid_null, URG_FLAG);
	FeatureAnalysis::AddFeature(URG_ptr, aid_null, URG_POINTER);
	FeatureAnalysis::CalculateMetric();
}

event FeatureExtraction::PTunnel_feature_event(UID : string, id : conn_id, direction:FeatureAnalysis::Direction, 
    features : FeatureAnalysis::feature_vector)
{
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PTUNNEL_SET, id, direction);
    for (idx in features)
	    FeatureAnalysis::AddFeature(features[idx], aid_null, ICMP_PAYLOAD_4_BYTES);
	FeatureAnalysis::CalculateMetric();
}

global count_CC : count;
global count_non_CC : count;
global cc_table : table[conn_id, FeatureAnalysis::Direction] of count;
event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, direction:FeatureAnalysis::Direction, 
    v : result_vector, conn_ID: conn_id)
{
    local c = DecisionTree::Classify(id, conn_ID, FeatureAnalysis::Extract_vector(v));
    if (c == 0){
        if ([conn_ID, direction] in cc_table)
            cc_table[conn_ID, direction] = cc_table[conn_ID, direction] + 1;
        else
            cc_table[conn_ID, direction] = 1;
        print ([conn_ID, direction]);
        count_CC = count_CC + 1;}
    else if (c == 1){
        count_non_CC = count_non_CC + 1;}
}

event bro_init()
{
    count_CC = 0;
    count_non_CC = 0;
}

event bro_done()
{
    print cc_table;
    local total : double = count_CC + count_non_CC;
    print(fmt("CC : %d, Non-CC : %d, percent CC : %f, percent non-CC : %f", count_CC, count_non_CC, 
        count_CC / total, count_non_CC / total));
}

#global training_set = set(FeatureAnalysis::TTL_SET);
#
#event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, direction:FeatureAnalysis::Direction, 
#    v : result_vector, conn_ID: conn_id)
#{
#	if (id in training_set)
#	{
#		FeatureTraining::AddDataRow(id, FeatureAnalysis::Extract_vector(v), "Non-CC");
#	}
#}
#
#event bro_done()
#{
#    print("done");
#    for (id in training_set)
#    {
#        FeatureTraining::print_training_data(id, fmt("training_Non-CC_%d.arff", id));
#    }
#}
