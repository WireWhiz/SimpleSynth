//
// Created by eli on 9/5/2022.
//

#include "simpleSynth.h"
#include <stdexcept>
#include <iostream>
#include <limits>

SimpleSynth::SimpleSynth()
{

}

SoundDevice& SimpleSynth::device()
{
	return _device;
}
