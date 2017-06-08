
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

void Plugin::ExtractFeature(StringVal* UID, Val* id, double duration) {
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
		vl->append(new StringVal(newStr));	// Bro unique UID
		vl->append(id->Clone());		// 4 tuple network ID
		// IAT is counted as the difference of the current flow's duration
		// and the flow's duration when the last packet was received
		vl->append(new Val(duration - got->second, TYPE_DOUBLE));
		mgr.QueueEvent(IAT::feature_event, vl);

		// remember the last packet's flow's duration
		got->second = duration;			
	}
}
