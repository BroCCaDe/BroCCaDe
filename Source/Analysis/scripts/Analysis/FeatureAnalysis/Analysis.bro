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
@load /opt/bro/lib/bro/plugins/Training/lib/bif
@load /opt/bro/lib/bro/plugins/BinTraining/lib/bif

global aid : vector of FeatureAnalysis::Analysis_ID;

event bro_init()
{
	DecisionTree::LoadModel(FeatureAnalysis::IAT_SET, "/home/hendra/Experiment/trace/automate_training/TreeModel-RATE_ALL_250_KS_500_500_EN_MM_16192_CCE_10_10_AC_50_500_REG_500_500");
    DecisionTree::LoadModel(FeatureAnalysis::TTL_SET, "/home/hendra/Experiment/trace/automate_training/TreeModel-TTL_ALL_250_KS_500_500_EN_MM_16192_CCE_10_10_AC_50_500_REG_500_500");
    DecisionTree::LoadModel(FeatureAnalysis::PACKET_LENGTH_SET, "/home/hendra/Experiment/trace/automate_training/TreeModel-PLEN_ALL_250_KS_500_500_EN_MM_16192_CCE_10_10_AC_50_500_REG_500_500");

#     aid[|aid|] = FeatureAnalysis::KS_ANALYSIS;
    aid[|aid|] = FeatureAnalysis::ENTROPY_ANALYSIS;
    aid[|aid|] = FeatureAnalysis::CCE_ANALYSIS;
    aid[|aid|] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
    aid[|aid|] = FeatureAnalysis::AUTOCORRELATION_ANALYSIS;
    aid[|aid|] = FeatureAnalysis::REGULARITY_ANALYSIS;
#    aid[|aid|] = FeatureAnalysis::NULL_ANALYSIS;

    FeatureAnalysis::SetStepSize(FeatureAnalysis::TTL_SET, 250);
    FeatureAnalysis::SetStepSize(FeatureAnalysis::IAT_SET, 250);
    FeatureAnalysis::SetStepSize(FeatureAnalysis::PACKET_LENGTH_SET, 250);
    FeatureAnalysis::SetKSWindowSize(FeatureAnalysis::TTL_SET, 500);
    FeatureAnalysis::SetRegularityParameters(FeatureAnalysis::TTL_SET, 500, 500);
    FeatureAnalysis::SetCCEPatternSize(FeatureAnalysis::TTL_SET, 10);
    FeatureAnalysis::SetAutocorrelationLags(FeatureAnalysis::TTL_SET, 50);
    FeatureAnalysis::SetKSWindowSize(FeatureAnalysis::IAT_SET, 500);
    FeatureAnalysis::SetRegularityParameters(FeatureAnalysis::IAT_SET, 500, 500);
    FeatureAnalysis::SetCCEPatternSize(FeatureAnalysis::IAT_SET, 10);
    FeatureAnalysis::SetAutocorrelationLags(FeatureAnalysis::IAT_SET, 50);
    FeatureAnalysis::SetKSWindowSize(FeatureAnalysis::PACKET_LENGTH_SET, 500);
    FeatureAnalysis::SetRegularityParameters(FeatureAnalysis::PACKET_LENGTH_SET, 500, 500);
    FeatureAnalysis::SetCCEPatternSize(FeatureAnalysis::PACKET_LENGTH_SET, 10);
    FeatureAnalysis::SetAutocorrelationLags(FeatureAnalysis::PACKET_LENGTH_SET, 50);
    FeatureAnalysis::ConfigureInternalType();

    local aid_CCE : vector of FeatureAnalysis::Analysis_ID;
    aid_CCE[0] = FeatureAnalysis::CCE_ANALYSIS;
    local aid_EN_MM : vector of FeatureAnalysis::Analysis_ID;
    aid_EN_MM[0] = FeatureAnalysis::ENTROPY_ANALYSIS;
    aid_EN_MM[1] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
    FeatureAnalysis::LoadNormalData(TTL, "/home/hendra/Experiment/trace/automate_training/ALL_TTL_KS_500");
    FeatureAnalysis::LoadInterval(TTL, aid_CCE, "/home/hendra/Experiment/trace/automate_training/ALL_TTL_Interval_10");
    FeatureAnalysis::SetBinNull(TTL, aid_EN_MM, 256);
    FeatureAnalysis::LoadNormalData(INTERARRIVAL_TIME, "/home/hendra/Experiment/trace/automate_training/ALL_IAT_KS_500");
    FeatureAnalysis::LoadInterval(INTERARRIVAL_TIME, aid_CCE, "/home/hendra/Experiment/trace/automate_training/ALL_IAT_Interval_10");
    FeatureAnalysis::LoadInterval(INTERARRIVAL_TIME, aid_EN_MM, "/home/hendra/Experiment/trace/automate_training/ALL_IAT_Interval_16192");
    FeatureAnalysis::LoadNormalData(PACKET_LENGTH, "/home/hendra/Experiment/trace/automate_training/ALL_PLEN_KS_500");
    FeatureAnalysis::LoadInterval(PACKET_LENGTH, aid_CCE, "/home/hendra/Experiment/trace/automate_training/ALL_PLEN_Interval_10");
    FeatureAnalysis::SetBinNull(PACKET_LENGTH, aid_EN_MM, 65536);
}

event connection_state_remove (c: connection)
{
	FeatureAnalysis::RemoveConn(c$uid);
}

event PacketLength_feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::PACKET_LENGTH_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid, PACKET_LENGTH);
	FeatureAnalysis::CalculateMetric();
}

event TTL_feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::TTL_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid, TTL);
	FeatureAnalysis::CalculateMetric();
}

event new_packet (c: connection, p: pkt_hdr)
{
    if ( p ?$ ip )
	{
        IAT::ExtractFeature(c$uid, c$id, FeatureAnalysis::GetDirection(c$id$orig_h, p$ip$src), c$duration);
    }
}

event IAT::feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::IAT_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid, INTERARRIVAL_TIME);
	FeatureAnalysis::CalculateMetric();
}

global cc_set : set[conn_id];
global id_set : set[conn_id];
#global cc_count : table[conn_id] of count;
#global non_cc_count : table[conn_id] of count;
#global cc_trigger : table[conn_id] of count;
global cc_hist : table[conn_id] of string;
event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, direction:FeatureAnalysis::Direction, 
    v : result_vector, conn_ID: conn_id)
{
    local c = DecisionTree::Classify(id, conn_ID, FeatureAnalysis::Extract_vector(v));
    local actual_ID : conn_id;
    switch direction
    {
        case FeatureAnalysis::FORWARD:
        {
            actual_ID$orig_h = conn_ID$orig_h;
            actual_ID$resp_h = conn_ID$resp_h;
            actual_ID$orig_p = conn_ID$orig_p;
            actual_ID$resp_p = conn_ID$resp_p;
            break;
        }
        case FeatureAnalysis::BACKWARD:
        {
            actual_ID$orig_h = conn_ID$resp_h;
            actual_ID$resp_h = conn_ID$orig_h;
            actual_ID$orig_p = conn_ID$resp_p;
            actual_ID$resp_p = conn_ID$orig_p;
            break;
        }
    }
#    if (actual_ID !in cc_count) {
#        cc_count[actual_ID] = 0;
#        non_cc_count[actual_ID] = 0;
#        cc_trigger[actual_ID] = 0;
#    }
    if (actual_ID !in cc_hist) {
        cc_hist[actual_ID] = "";
    }
    if (c == 0){ # for now we assume that the CC is always on index 0
        add cc_set[actual_ID];
        cc_hist[actual_ID] += "0";
#        cc_count[actual_ID] += 1;
#        if (cc_trigger[actual_ID] == 0) {
#            cc_trigger[actual_ID] = non_cc_count[actual_ID] + 1; 
#        }
    }
    else{
#        non_cc_count[actual_ID] += 1;
        cc_hist[actual_ID] += "1";
    }
}

function actual_protocol(t : transport_proto) : count
{
    switch t
    {
        case tcp:
            return 6;
        case udp:
            return 17;
        case icmp:
            return 1;
        case unknown_transport:
            return 253;
    }
}

event bro_done()
{
#    local extras = open("extras_RATE_250_KS_500_500_EN_MM_16192_CCE_10_10_AC_50_500_REG_500_500");
    local hist = open("hist_RATE_250_KS_500_500_EN_MM_16192_CCE_10_10_AC_50_500_REG_500_500");
#    for (conn_ID in cc_set) # don't care about direction for now
#    {
#        print (fmt("%s %s %d %d %d", conn_ID$orig_h, conn_ID$resp_h, actual_protocol(get_port_transport_proto(conn_ID$orig_p)), 
#                port_to_count(conn_ID$orig_p), port_to_count(conn_ID$resp_p)));
#    }
#    for ([conn_ID] in non_cc_count) {
#        print extras, (fmt("%s %s %d %d %d %d %d %d", conn_ID$orig_h, conn_ID$resp_h, 
#                actual_protocol(get_port_transport_proto(conn_ID$orig_p)), 
#                port_to_count(conn_ID$orig_p), port_to_count(conn_ID$resp_p), 
#                cc_trigger[conn_ID], cc_count[conn_ID], non_cc_count[conn_ID]));
#    }
    for ([conn_ID] in cc_hist) {
        print hist, (fmt("%s %s %d %d %d %s", conn_ID$orig_h, conn_ID$resp_h, 
                actual_protocol(get_port_transport_proto(conn_ID$orig_p)), 
                port_to_count(conn_ID$orig_p), port_to_count(conn_ID$resp_p), cc_hist[conn_ID]));
    }
#    close(extras);
    close(hist);
}
