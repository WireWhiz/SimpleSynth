//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_SOUNDSOURCE_H
#define SIMPLESYNTH_SOUNDSOURCE_H


#include <cstdint>
#include <utility>
#include "sample.h"

class SoundSource
{
public:
	uint64_t startSample = 0;
	virtual Sample getSample(double currentTime) = 0;
};


#endif //SIMPLESYNTH_SOUNDSOURCE_H
