//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_COMPRESSORSOURCE_H
#define SIMPLESYNTH_COMPRESSORSOURCE_H

#include "soundSource.h"

class CompressorSource : public SoundSource
{
public:
	SoundSource* source = nullptr;
	int32_t resolution = 32;
	CompressorSource(SoundSource* source);
	std::tuple<float, float> getSample(uint64_t currentSample, uint64_t samplesPerSec) override;
};


#endif //SIMPLESYNTH_COMPRESSORSOURCE_H
