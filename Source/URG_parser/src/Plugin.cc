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
* Note      : to attach the parser to Bro analyzer tree, we have to add the     *
*             component, hook to the analyzer tree setup, and actually attach   *
*             our parser                                                        *
\*******************************************************************************/

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
