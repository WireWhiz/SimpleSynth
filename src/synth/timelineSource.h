//
// Created by eli on 9/8/2022.
//

#ifndef SIMPLESYNTH_TIMELINESOURCE_H
#define SIMPLESYNTH_TIMELINESOURCE_H

#include "soundSource.h"
#include <vector>
#include <unordered_set>

class Synth;
class TimelineSource : public SoundSource
{

public:
	Synth* synth = nullptr;
	struct Beat
	{
		std::unordered_set<float> notes;
	};

	bool loop = false;
	float bpm = 120;
	std::vector<Beat> beats;
	double startTime = 0;
	Sample getSample(double currentTime) override;
};


#endif //SIMPLESYNTH_TIMELINESOURCE_H
