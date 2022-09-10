//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_SAWSYNTH_H
#define SIMPLESYNTH_SAWSYNTH_H


#include "synth.h"

class SawSynth : public Synth
{
public:
	Sample getSample(double currentTime) override;
};


#endif //SIMPLESYNTH_SAWSYNTH_H
