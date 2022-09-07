//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_SOUNDSOURCE_H
#define SIMPLESYNTH_SOUNDSOURCE_H


#include <cstdint>
#include <tuple>

class SoundSource
{
public:
	uint64_t startSample = 0;
	virtual std::tuple<float, float> getSample(uint64_t currentSample, uint64_t samplesPerSec) = 0;
};


#endif //SIMPLESYNTH_SOUNDSOURCE_H
