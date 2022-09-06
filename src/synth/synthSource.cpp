//
// Created by eli on 9/6/2022.
//

#include "synthSource.h"
#define _USE_MATH_DEFINES
#include <cmath>

std::tuple<float, float> SynthSource::getSample(uint64_t currentSample, uint64_t samplesPerSec)
{
	const float PI_2 = 3.14159265359 * 2;
	float amplitude = std::sin(hertz * PI_2 * (float)currentSample / (float)samplesPerSec) * volume;
	return {amplitude, amplitude};
}
