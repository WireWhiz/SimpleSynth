//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_AMPSOURCE_H
#define SIMPLESYNTH_AMPSOURCE_H

#include "soundSource.h"

class AmpSource : public SoundSource
{
public:
	SoundSource* _source = nullptr;
	float amplitude = 1;
	AmpSource(SoundSource* source);
	Sample getSample(double currentTime) override;
};


#endif //SIMPLESYNTH_AMPSOURCE_H
