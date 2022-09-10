//
// Created by eli on 9/6/2022.
//

#include "soundSource.h"

SoundSource::Sample& SoundSource::Sample::operator+=(const SoundSource::Sample& o)
{
	left += o.left;
	right += o.right;
	return *this;
}

SoundSource::Sample SoundSource::Sample::operator+(const SoundSource::Sample& o) const
{
	return {left + o.left, right + o.right};
}

SoundSource::Sample SoundSource::Sample::operator*(float v) const
{
	return {left * v, right * v};
}
