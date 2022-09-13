//
// Created by eli on 9/8/2022.
//

#include "timelineSource.h"
#include "synth.h"

Sample TimelineSource::getSample(double currentTime)
{
	currentTime -= startTime;
	Sample output = {0,0};
	if(!synth || beats.empty())
		return output;
	size_t currentBeat = currentTime / (60 / bpm);
	if(loop)
		currentBeat %= beats.size();
	else if(currentBeat >= beats.size())
		return output;

	for(float note : beats[currentBeat].notes)
	{
		synth->hertz = note;
		output += synth->getSample(currentTime);
	}

	return output;
}
