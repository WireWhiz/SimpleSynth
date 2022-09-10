//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_SINSYNTH_H
#define SIMPLESYNTH_SINSYNTH_H

#include "synth.h"

class SinSynth : public Synth
{
public:
	Sample getSample(double currentTime) override;
};


#endif //SIMPLESYNTH_SINSYNTH_H
