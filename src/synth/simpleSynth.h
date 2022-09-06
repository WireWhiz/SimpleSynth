//
// Created by eli on 9/5/2022.
//

#ifndef SIMPLESYNTH_SIMPLESYNTH_H
#define SIMPLESYNTH_SIMPLESYNTH_H


#include "soundDevice.h"

class SimpleSynth
{
	SoundDevice _device;
public:
	SimpleSynth();
	SoundDevice& device();
};


#endif //SIMPLESYNTH_SIMPLESYNTH_H
