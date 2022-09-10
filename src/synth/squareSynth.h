//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_SQUARESYNTH_H
#define SIMPLESYNTH_SQUARESYNTH_H


#include "synth.h"

class SquareSynth : public Synth
{
public:
	Sample getSample(double currentTime) override;
};


#endif //SIMPLESYNTH_SQUARESYNTH_H
