//
// Created by eli on 9/6/2022.
//

#include <utility>
#include "squareSynth.h"

SoundSource::Sample SquareSynth::getSample(double currentTime)
{
	float a = (float)(static_cast<int64_t>(currentTime * hertz * 2) % 2) * 2 - 1;

	return {a,a};
}
