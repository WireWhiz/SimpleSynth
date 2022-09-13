//
// Created by eli on 9/6/2022.
//

#include "sinSynth.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <utility>

Sample SinSynth::getSample(double currentTime)
{
	const float PI_2 = 3.14159265359 * 2;
	float amplitude = std::sin(hertz * PI_2 * currentTime);
	return {amplitude, amplitude};
}
