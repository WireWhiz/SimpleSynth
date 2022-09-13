//
// Created by eli on 9/6/2022.
//

#include <utility>
#include "ampSource.h"

AmpSource::AmpSource(SoundSource* source)
{
	_source = source;
}

Sample AmpSource::getSample(double currentTime)
{
	auto s = _source->getSample(currentTime);
	s.left *= amplitude;
	s.right *= amplitude;
	return s;
}
