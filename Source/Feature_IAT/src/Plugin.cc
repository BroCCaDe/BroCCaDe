
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

void Plugin::ExtractFeature(StringVal* UID, double duration) {
	if (UID == NULL) {printf("UID is null\n"); return;}
	//BroString* newStr = new BroString(*(UID->AsString()));
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
		val_list* vl = new val_list;
		BroString* newStr = new BroString(*(UID->AsString()));
		vl->append(new StringVal(newStr));
		//VectorVal* v = new VectorVal(new VectorType(base_type(TYPE_DOUBLE)));
		//v->Assign(0u, new Val(duration - got->second, TYPE_DOUBLE));
		vl->append(new Val(duration - got->second, TYPE_DOUBLE));
		got->second = duration;
		//vl->append(v);
		mgr.QueueEvent(IAT::feature_event, vl);
	}
}
