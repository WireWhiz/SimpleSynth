add_subdirectory(graphics)
add_library(synth STATIC  simpleSynth.cpp simpleSynth.h)
#target_link_libraries(synth PUBLIC)