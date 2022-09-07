//
// Created by eli on 9/6/2022.
//

#include "sawSource.h"
#include <cmath>

std::tuple<float, float> SawSource::getSample(uint64_t currentSample, uint64_t samplesPerSec)
{
	float amp = ((float )currentSample / (float )samplesPerSec) * (hertz);
	float a = (amp - static_cast<int32_t>(amp)) * 2 - 1;

	return {a,a};
}