add_library(synth STATIC
        simpleSynth.cpp
        soundDevice.cpp
        soundSource.cpp
        sinSynth.cpp
        compressorSource.cpp
        ampSource.cpp
        squareSynth.cpp
        sawSynth.cpp
        synth.cpp
        timelineSource.cpp sample.cpp sample.h)
if(UNIX)
target_link_libraries(synth PUBLIC -lpulse)
endif()