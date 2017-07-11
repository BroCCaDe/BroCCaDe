/*******************************************************************************\
* Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murodch.edu.au)            *
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
#include "iat.bif.h"
#include "Type.h"
#include "Net.h"
#include <unordered_map>

namespace plugin { namespace FeatureExtraction_IAT { Plugin plugin; } }

using namespace plugin::FeatureExtraction_IAT;

plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "FeatureExtraction::IAT";
	config.description = "<Insert description>";
	config.version.major = 0;
	config.version.minor = 1;

	_flow_dict = new std::unordered_map<std::string, double>();	
	
	return config;
	}

void Plugin::RemoveConnection(StringVal* UID)
{
	BroString* newStr = new BroString(*(UID->AsString()));
	std::string UID_str((const char*) newStr->Bytes());
	_flow_dict->erase(UID_str);
}

void Plugin::ExtractFeature(StringVal* UID, Val* conn_ID, Val* direction, double duration) {
	if (UID == NULL) {printf("UID is null\n"); return;}

	std::string UID_str((const char*) UID->Bytes());
	std::unordered_map<std::string, double>::iterator got = 
		_flow_dict->find(UID_str);

	// this is the first time we see, so no IAT is returned
	if (got == _flow_dict->end())
	{
		pair<std::unordered_map<std::string, double>::const_iterator, bool>
			result = _flow_dict->emplace(UID_str, duration);
	}
	else
	{ 
		// send the event : [ UID, id, feature ]
		val_list* vl = new val_list;
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

		// IAT is counted as the difference of the current flow's duration
		// and the flow's duration when the last packet was received
		vl->append(new Val(duration - got->second, TYPE_DOUBLE));
		mgr.QueueEvent(IAT::feature_event, vl);

		// remember the last packet's flow's duration
		got->second = duration;			
	}
}
