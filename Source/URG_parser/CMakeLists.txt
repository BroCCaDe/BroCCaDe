
cmake_minimum_required(VERSION 2.8)

project(Plugin)

include(BroPlugin)

bro_plugin_begin(FeatureExtraction UrgentPointer)
bro_plugin_cc(src/Plugin.cc src/URG_parser.cc)
bro_plugin_bif(src/urgentpointer.bif)
bro_plugin_dist_files(README CHANGES COPYING VERSION)
bro_plugin_end()
