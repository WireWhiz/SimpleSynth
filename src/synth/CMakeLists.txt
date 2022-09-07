add_library(synth STATIC
        simpleSynth.cpp
        soundDevice.cpp
        soundSource.cpp
        sinSource.cpp
        compressorSource.cpp
        ampSource.cpp
        squareSource.cpp
        sawSource.cpp)
#target_link_libraries(synth PUBLIC)