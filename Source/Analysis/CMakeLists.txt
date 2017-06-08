
cmake_minimum_required(VERSION 2.8)

project(Plugin)

include(BroPlugin)

bro_plugin_begin(Analysis FeatureAnalysis)
bro_plugin_cc(src/Plugin.cc src/Flow.cc src/KS_Flow.cc src/Data_Container.cc
	src/Bin_Strategy_Interval.cc src/Entropy_Flow.cc src/Histogram.cc
	src/CCE_Flow.cc src/Pattern_Data_Container.cc src/Tree.cc
	src/MultiModal_Flow.cc src/Autocorrelation_Flow.cc src/Regularity_Flow.cc
	src/Regularity_Data_Container.cc)
bro_plugin_bif(src/analysis.bif)
bro_plugin_dist_files(README CHANGES COPYING VERSION)
bro_plugin_end()
