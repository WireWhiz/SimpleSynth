//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_SINSOURCE_H
#define SIMPLESYNTH_SINSOURCE_H

#include "soundSource.h"

class SinSource : public SoundSource
{
public:
	float hertz = 0;

	std::tuple<float, float> getSample(uint64_t currentSample, uint64_t samplesPerSec) override;
};


#endif //SIMPLESYNTH_SINSOURCE_H
