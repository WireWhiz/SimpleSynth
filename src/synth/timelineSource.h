//
// Created by eli on 9/8/2022.
//

#ifndef SIMPLESYNTH_TIMELINESOURCE_H
#define SIMPLESYNTH_TIMELINESOURCE_H

#include "soundSource.h"
#include <vector>
#include <unordered_set>
#include <robin_hood/robin_hood.h>

class Synth;
class TimelineSource : public SoundSource
{

public:
	Synth* synth = nullptr;
	struct Beat
	{
		robin_hood::unordered_set<float> notes;
	};

	bool loop = false;
	float bpm = 120;
	std::vector<Beat> beats;
	double startTime = 0;
	Sample getSample(double currentTime) override;
	double duration() const;
};


#endif //SIMPLESYNTH_TIMELINESOURCE_H
