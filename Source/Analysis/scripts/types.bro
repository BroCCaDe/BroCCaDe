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

type analysis_tag : enum {INTERARRIVAL_TIME=0, PACKET_LENGTH=1, 
    URG_FLAG=2, URG_POINTER=3, ICMP_PAYLOAD_4_BYTES=4, TTL=5};

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

	type set_ID : enum {URGENT_SET = 0, IAT_SET = 1, PTUNNEL_SET = 2, PACKET_LENGTH_SET = 3, TTL_SET = 4};
    type Direction : enum {FORWARD = 0, BACKWARD = 1};

	type decision_tree_args : record
	{
		URG_flag : double;
		URG_pointer : double;
	};

	type feature_vector : vector of double;
}
module GLOBAL;
