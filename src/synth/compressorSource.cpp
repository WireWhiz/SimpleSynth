//
// Created by eli on 9/6/2022.
//

#include "compressorSource.h"
#include <cmath>
#include <utility>

CompressorSource::CompressorSource(SoundSource* source)
{
	this->source = source;
}

Sample CompressorSource::getSample(double currentTime)
{
	auto [left, right] = source->getSample(currentTime);
	double power =  std::pow(2, resolution) / 2;
	left  = (float)(std::round(left  * power) / power);
	right = (float)(std::round(right * power) / power);
	return {left, right};
}
