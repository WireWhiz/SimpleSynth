add_library(synth STATIC  simpleSynth.cpp soundDevice.cpp soundSource.cpp soundSource.h synthSource.cpp synthSource.h)
#target_link_libraries(synth PUBLIC)