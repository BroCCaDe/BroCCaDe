
#include "Plugin.h"
#include "URG_parser.h"

namespace plugin { namespace FeatureExtraction_UrgentPointer { Plugin plugin; } }

using namespace plugin::FeatureExtraction_UrgentPointer;

plugin::Configuration Plugin::Configure()
{
	AddComponent(new ::analyzer::Component("URG_parser", 
		::plugin::FeatureExtraction_UrgentPointer::URG_parser::Instantiate));

	plugin::Configuration config;
	config.name = "FeatureExtraction::UrgentPointer";
	config.description = "<Insert description>";
	config.version.major = 0;
	config.version.minor = 1;
	
	EnableHook(HOOK_SETUP_ANALYZER_TREE, 0);

	return config;
}

void Plugin::HookSetupAnalyzerTree(Connection *conn)
{
	// only take care of TCP packet
	if ( conn->ConnTransport() != TRANSPORT_TCP )
		return;

	analyzer::TransportLayerAnalyzer* root = 0;
	root = conn->GetRootAnalyzer();

	// create the packet analyzer
	URG_parser* urg_parser = new URG_parser(conn);

	// attach the packet analyzer to the analyzer tree
	((analyzer::tcp::TCP_Analyzer *) root)->AddChildPacketAnalyzer(urg_parser);
	
	urg_parser->Init();
}
