//
// Created by eli on 9/6/2022.
//

#include "squareSource.h"

std::tuple<float, float> SquareSource::getSample(uint64_t currentSample, uint64_t samplesPerSec)
{
	float a = (float)(static_cast<int64_t>(((double )currentSample / (double )samplesPerSec) * (double )(hertz * 2)) % 2) * 2 - 1;

	return {a,a};
}
