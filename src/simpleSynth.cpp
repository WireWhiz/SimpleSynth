//
// Created by eli on 9/5/2022.
//

#include "simpleSynth.h"
#include <Audioclient.h>
#include <Mmdeviceapi.h>
#include <stdexcept>
#include <iostream>
#include <comdef.h>
#include <limits>

void checkResult(HRESULT result)
{
	if(FAILED(result))
	{
		_com_error err(result);
		std::string errorStr = err.ErrorMessage();
		throw std::runtime_error(errorStr);
	}
}

SimpleSynth::SimpleSynth()
{
	checkResult(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	IMMDeviceEnumerator* devEnum;
	checkResult(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&devEnum));

	IMMDevice* device = nullptr;
	checkResult(devEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &device));

	IAudioClient* client = nullptr;
	checkResult(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&client));

	WAVEFORMATEX * pwfx = nullptr;
	checkResult(client->GetMixFormat(&pwfx));

	std::cout << "Format: " << pwfx->wFormatTag << std::endl;
    std::cout << "Channels: " << pwfx->nChannels << std::endl;
	std::cout << "Bits per sample: " << pwfx->wBitsPerSample << std::endl;
	std::cout << "Samples per second: " << pwfx->nSamplesPerSec << std::endl;
	std::cout << "Audio frame size: " << pwfx->nBlockAlign << std::endl;
	if(pwfx->cbSize >= 22)
	{
		WAVEFORMATEXTENSIBLE* pwfxx = (WAVEFORMATEXTENSIBLE*)pwfx;
		std::cout << "Format: " << pwfxx->SubFormat.Data1 << std::endl;
	}

	checkResult(client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 256, 0, pwfx, nullptr));
	uint32_t bufferSize = 0;
	checkResult(client->GetBufferSize(&bufferSize));

	IAudioRenderClient* renderClient = nullptr;
	checkResult(client->GetService(__uuidof(IAudioRenderClient), (void**)&renderClient));

	BYTE* audioData = nullptr;
	uint64_t currentSample = 0;

	checkResult(client->Reset());
	checkResult(client->Start());
	while(true)
	{
		uint32_t padding = 0;
		client->GetCurrentPadding(&padding);
		uint32_t writableSize = bufferSize - padding;
		checkResult(renderClient->GetBuffer(writableSize, &audioData));

		//Fill buffer with something here;
		for(uint32_t fIndex = 0; fIndex < writableSize; ++fIndex)
		{
			BYTE* frame = audioData + fIndex * pwfx->nBlockAlign;
			//*(int32_t*)frame = static_cast<int32_t>(static_cast<int8_t>(std::sin(440 * (2 * 3.14159) * (double)fIndex / 10000000) * 0.5) * SCHAR_MAX) * (INT_MAX / SCHAR_MAX);
			float hertz1 = 59.0;
			float hertz2 = 75.0;
			float pi = 3.14159;
			float amplitude1 = std::sin(hertz1 * (2.0 * pi) * (float)currentSample++ / pwfx->nSamplesPerSec);
			*(float*)frame = amplitude1;

			float amplitude2 = std::sin(hertz2 * (2.0 * pi) * (float)currentSample++ / pwfx->nSamplesPerSec);
			*(float*)(frame + pwfx->nBlockAlign / 2) = amplitude2;
		}

		checkResult(renderClient->ReleaseBuffer(writableSize, 0));
	}

	CoTaskMemFree(pwfx);
}
