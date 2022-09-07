//
// Created by eli on 9/6/2022.
//

#include "compressorSource.h"
#include <cmath>

CompressorSource::CompressorSource(SoundSource* source)
{
	this->source = source;
}

std::tuple<float, float> CompressorSource::getSample(uint64_t currentSample, uint64_t samplesPerSec)
{
	auto [left, right] = source->getSample(currentSample, samplesPerSec);
	double power =  std::pow(2, resolution) / 2;
	left  = (float)(std::round(left  * power) / power);
	right = (float)(std::round(right * power) / power);
	return {left, right};
}
