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
* Plugin.h : Bro plugin which implements functions specified in iat.bif         *
* Content:                                                                      *
*   * ExtractFeature    : extract inter arrival time feature                    *
*   * RemoveConn        : remove a connection from the dictionary               *
*   * feature_event     : event raised by the plugin containing the inter       *
*                         arrival time value.                                   *
\*******************************************************************************/

#ifndef BRO_PLUGIN_FEATUREEXTRACTION_IAT
#define BRO_PLUGIN_FEATUREEXTRACTION_IAT

#include <plugin/Plugin.h>
#include <unordered_map>

namespace plugin {
namespace FeatureExtraction_IAT {

class Plugin : public ::plugin::Plugin
{
public:
	// calculate the inter arrival time. This inherently implies
	// that the first packet will not generate an event
	void ExtractFeature(StringVal* UID, Val* id, Val* direction, double duration);
	// whenever Bro removes a flow, we also remove the floe
	void RemoveConnection(StringVal* UID);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	// maintain the last duration of each flow
	std::unordered_map<std::string, double>* _flow_dict;	
};

extern Plugin plugin;

}
}

#endif
