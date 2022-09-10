//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_SOUNDSOURCE_H
#define SIMPLESYNTH_SOUNDSOURCE_H


#include <cstdint>
#include <utility>

class SoundSource
{
public:
	struct Sample
	{
		float left;
		float right;
		Sample& operator+=(const Sample&);
		Sample operator+(const Sample&) const;
		Sample operator*(float) const;
	};

	uint64_t startSample = 0;
	virtual Sample getSample(double currentTime) = 0;
};


#endif //SIMPLESYNTH_SOUNDSOURCE_H
