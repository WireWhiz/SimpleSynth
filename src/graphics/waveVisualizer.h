//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_WAVEVISUALIZER_H
#define SIMPLESYNTH_WAVEVISUALIZER_H


#include "../synth/soundSource.h"

class WaveVisualizer
{
public:
	SoundSource* source;
	float startTime = 0;
	float endTime = 100;
	WaveVisualizer(SoundSource* soundSource);
	void cache();
	void draw();
};


#endif //SIMPLESYNTH_WAVEVISUALIZER_H
