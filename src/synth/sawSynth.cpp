//
// Created by eli on 9/6/2022.
//

#include "sawSynth.h"
#include <cmath>
#include <utility>

Sample SawSynth::getSample(double currentTime)
{
	float a = (float)(currentTime * hertz);
	a = (a - static_cast<int32_t>(a)) * 2 - 1;

	return {a,a};
}