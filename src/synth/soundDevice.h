//
// Created by eli on 9/6/2022.
//

#ifndef SIMPLESYNTH_SOUNDDEVICE_H
#define SIMPLESYNTH_SOUNDDEVICE_H

#ifdef WIN32
#include <Audioclient.h>
#include <Mmdeviceapi.h>
#include <comdef.h>

#elif UNIX

#endif

#include <thread>
#include <atomic>

class SoundDevice
{
	uint32_t _samplesPerSec;
	std::thread _audioThread;
	std::atomic_bool _threadPlaying = false;
#ifdef WIN32
	IMMDevice* _device = nullptr;
	IAudioClient* _client = nullptr;
	IAudioRenderClient* _renderClient = nullptr;
	WAVEFORMATEX* _format = nullptr;
	HANDLE _bufferReady = nullptr;
	void initWindows();
#elif UNIX
	void initUnix();
#endif
public:
	SoundDevice();
	~SoundDevice();
};


#endif //SIMPLESYNTH_SOUNDDEVICE_H
