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

@load /opt/bro/lib/bro/plugins/Feature_IAT/lib/bif
@load /opt/bro/lib/bro/plugins/Training/lib/bif

global aid : vector of FeatureAnalysis::Analysis_ID;

event bro_init()
{
    aid[0] = FeatureAnalysis::KS_ANALYSIS;
	aid[1] = FeatureAnalysis::ENTROPY_ANALYSIS;
	aid[2] = FeatureAnalysis::CCE_ANALYSIS;
	aid[3] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
	aid[4] = FeatureAnalysis::AUTOCORRELATION_ANALYSIS;
	aid[5] = FeatureAnalysis::REGULARITY_ANALYSIS;

    FeatureAnalysis::ConfigureInternalType();
    local aid_CCE : vector of FeatureAnalysis::Analysis_ID;
    aid_CCE[0] = FeatureAnalysis::CCE_ANALYSIS;
    local aid_EN_MM : vector of FeatureAnalysis::Analysis_ID;
    aid_EN_MM[0] = FeatureAnalysis::ENTROPY_ANALYSIS;
    aid_EN_MM[1] = FeatureAnalysis::MULTIMODAL_ANALYSIS;
    FeatureAnalysis::LoadNormalData(TTL, "/home/hendra/Experiment/trace/09-0230/HASH_TTL_KS");
    FeatureAnalysis::LoadInterval(TTL, aid_CCE, "/home/hendra/Experiment/trace/09-0230/HASH_TTL_Interval_16");
    FeatureAnalysis::SetBinNull(TTL, aid_EN_MM, 256);
    FeatureAnalysis::LoadNormalData(INTERARRIVAL_TIME, "/home/hendra/Experiment/trace/09-0230/ALL_IAT_KS");
    FeatureAnalysis::LoadInterval(INTERARRIVAL_TIME, aid_CCE, "/home/hendra/Experiment/trace/09-0230/ALL_IAT_Interval_20");
    FeatureAnalysis::LoadInterval(INTERARRIVAL_TIME, aid_EN_MM, "/home/hendra/Experiment/trace/09-0230/ALL_IAT_Interval_20");

	FeatureTraining::ChangeRelation(FeatureAnalysis::IAT_SET, "metrics");
	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "KS");
	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "Entropy");
	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "CCE");
	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "MultiModal");
	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "Autocorrelation");
	FeatureTraining::AddAttributes(FeatureAnalysis::IAT_SET, "Regularity");
	FeatureTraining::AddClass(FeatureAnalysis::IAT_SET, "CC");
	FeatureTraining::AddClass(FeatureAnalysis::IAT_SET, "Non-CC");
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

event TTL_feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::TTL_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid, TTL);
	FeatureAnalysis::CalculateMetric();
}

event new_packet (c: connection, p: pkt_hdr)
{
    if ( p ?$ ip )
	{
        IAT::ExtractFeature(c$uid, c$id, get_direction(c$id$orig_h, p$ip$src), c$duration);
    }
}

event IAT::feature_event(UID:string, id:conn_id, direction:FeatureAnalysis::Direction, value: double) {
	FeatureAnalysis::RegisterAnalysis(UID, FeatureAnalysis::IAT_SET, id, direction);
	FeatureAnalysis::AddFeature(value, aid, INTERARRIVAL_TIME);
	FeatureAnalysis::CalculateMetric();
}

global training_set = set(FeatureAnalysis::IAT_SET);

event FeatureAnalysis::metric_event(id : FeatureAnalysis::set_ID, direction:FeatureAnalysis::Direction, 
    v : result_vector, conn_ID: conn_id)
{
	if (id in training_set)
	{
		FeatureTraining::AddDataRow(id, FeatureAnalysis::Extract_vector(v), "Non-CC");
	}
}

event bro_done()
{
    print("cleaning up");
    for (id in training_set)
    {
        FeatureTraining::print_training_data(id, "/home/hendra/Experiment/trace/09-0230/IAT_Non_CC_HASH_20.arff");
    }
    print("done printing");
}
