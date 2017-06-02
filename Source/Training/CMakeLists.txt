
cmake_minimum_required(VERSION 2.8)

project(Plugin)

include(BroPlugin)

bro_plugin_begin(Training Bin)
bro_plugin_cc(src/Bin_Training.cc src/Plugin.cc)
bro_plugin_bif(src/bin.bif)
bro_plugin_dist_files(README CHANGES COPYING VERSION)
bro_plugin_end()
