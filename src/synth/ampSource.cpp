//
// Created by eli on 9/6/2022.
//

#include "ampSource.h"

AmpSource::AmpSource(SoundSource* source)
{
	_source = source;
}

std::tuple<float, float> AmpSource::getSample(uint64_t currentSample, uint64_t samplesPerSec)
{
	auto [left, right] = _source->getSample(currentSample, samplesPerSec);
	left *= amplitude;
	right *= amplitude;
	return {left, right};
}
