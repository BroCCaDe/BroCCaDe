/*******************************************************************************\
* Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murdoch.edu.au)            *
*                                                                               *
* Redistribution and use in source and binary forms, with or without            *
* modification, are permitted provided that the following conditions are met:   *
*                                                                               *
* (1) Redistributions of source code must retain the above copyright            *
*     notice, this list of conditions and the following disclaimer.             *
*                                                                               *
* (2) Redistributions in binary form must reproduce the above copyright         *
*     notice, this list of conditions and the following disclaimer in           *
*     the documentation and/or other materials provided with the                *
*     distribution.                                                             *
*                                                                               *
* (3) Neither the name of Hendra Gunadi and/or Murdoch University, nor          *
*     the names of contributors may be used to endorse or promote               *
*     products derived from this software without specific prior written        *
*     permission.                                                               *
*                                                                               *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"   *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE     *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE    *
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF          *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS      *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE    *
* POSSIBILITY OF SUCH DAMAGE.                                                   *
*                                                                               *
* Plugin.cc : Implements Plugin.h                                               *
\*******************************************************************************/

#include "Plugin.h"
#include "Event.h"
#include "featureextraction.bif.h"
#include "Type.h"

#define DEBUG_H

namespace plugin { namespace PTunnel_FeatureExtraction { Plugin plugin; } }

using namespace plugin::PTunnel_FeatureExtraction;

plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "PTunnel::FeatureExtraction";
	config.description = "<Insert description>";
	config.version.major = 0;
	config.version.minor = 1;

	return config;
	}

// the length is limited to 4 bytes
void plugin::PTunnel_FeatureExtraction::ExtractFeature(StringVal* UID, Val* conn_ID, 
		Val* direction, StringVal* payload, unsigned int position, unsigned int len) {
    if (payload->Len() < position + len) return; // if there's not enough bytes then skip

	const u_char* bytes = payload->Bytes();
	val_list* vl = new val_list;
	unsigned int i; unsigned long feature = 0;

	BroString* newStr = new BroString(*(UID->AsString()));
	vl->append(new StringVal(newStr));	                // Bro unique UID

	RecordVal* conn_r = conn_ID->AsRecordVal(); 
    RecordVal* conn_ID_copy = new RecordVal(conn_id); 
    conn_ID_copy->Assign(0, new AddrVal(conn_r->Lookup(0)->AsAddr())); 
    PortVal* src_port = conn_r->Lookup(1)->AsPortVal(); 
    conn_ID_copy->Assign(1, new PortVal(src_port->Port(), src_port->PortType())); 
    conn_ID_copy->Assign(2, new AddrVal(conn_r->Lookup(2)->AsAddr())); 
    PortVal* dst_port = conn_r->Lookup(3)->AsPortVal(); 
    conn_ID_copy->Assign(3, new PortVal(dst_port->Port(), dst_port->PortType())); 
    vl->append(conn_ID_copy);                           // 4 tuple network ID
    vl->append(new Val(direction->AsEnum(), TYPE_ENUM));// direction

    vl->append(new VectorVal(internal_type("FeatureAnalysis::feature_vector")->AsVectorType()));
    for (i = 0; i < len; i++) (*vl)[3]->AsVectorVal()->Assign(i, new Val((double)(*(bytes+position+i)), TYPE_DOUBLE));

	mgr.QueueEvent(FeatureExtraction::PTunnel_feature_event, vl);
}
