//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_SOUNDDEVICE_H
#define SIMPLESYNTH_SOUNDDEVICE_H

#ifdef WIN32
#include <Audioclient.h>
#include <Mmdeviceapi.h>
#include <comdef.h>

#elif __linux__
#include <pulse/thread-mainloop.h>
#include <pulse/error.h>
#include <pulse/stream.h>
#endif

#include <thread>
#include <atomic>
#include <vector>
#include "sample.h"

class SoundSource;
class SoundDevice
{
	uint32_t _samplesPerSec;
	std::atomic<uint64_t> _currentSample = 0;

	std::vector<SoundSource*> _sources;
#ifdef WIN32
    std::thread _audioThread;
	std::atomic_bool _threadPlaying = false;
	IMMDevice* _device = nullptr;
	IAudioClient* _client = nullptr;
	IAudioRenderClient* _renderClient = nullptr;
	WAVEFORMATEX* _format = nullptr;
	HANDLE _bufferReady = nullptr;
	void initWindows();
#elif __linux__
    pa_threaded_mainloop* _mainLoop = nullptr;
    pa_context* _ctx = nullptr;
    pa_stream* _outStream = nullptr;
	void initLinux();
#endif

    Sample getSample(double time);
public:
	SoundDevice();
	~SoundDevice();
	double currentTime() const;
	void addSource(SoundSource* source);
	void removeSource(SoundSource* source);
};


#endif //SIMPLESYNTH_SOUNDDEVICE_H
