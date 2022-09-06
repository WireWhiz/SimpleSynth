//
// Created by eli on 9/6/2022.
//

#include <string>
#include "soundDevice.h"
#include <stdexcept>
#include <cmath>

#ifdef WIN32
void cwr(HRESULT res)
{
	if(FAILED(res))
	{
		_com_error err(res);
		std::string errorStr = err.ErrorMessage();
		throw std::runtime_error(errorStr);
	}
}
void SoundDevice::initWindows()
{
	cwr(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	IMMDeviceEnumerator* devEnum;
	cwr(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&devEnum));

	cwr(devEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &_device));

	cwr(_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&_client));

	cwr(_client->GetMixFormat(&_format));

	/*std::cout << "Format: " << pwfx->wFormatTag << std::endl;
	std::cout << "Channels: " << pwfx->nChannels << std::endl;
	std::cout << "Bits per sample: " << pwfx->wBitsPerSample << std::endl;
	std::cout << "Samples per second: " << pwfx->nSamplesPerSec << std::endl;
	std::cout << "Audio frame size: " << pwfx->nBlockAlign << std::endl;*/
	if(_format->cbSize >= 22)
	{
		WAVEFORMATEXTENSIBLE* pwfxx = (WAVEFORMATEXTENSIBLE*)_format;
		KSDATAFORMAT_SUBTYPE_PCM;
		KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
	}

	cwr(_client->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 256, 0, _format, nullptr));
	uint32_t bufferSize = 0;
	cwr(_client->GetBufferSize(&bufferSize));

	cwr(_client->GetService(__uuidof(IAudioRenderClient), (void**)&_renderClient));

	cwr(_client->Reset());

	_bufferReady = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!_bufferReady)
		throw std::runtime_error("Could not create audio callback!");

	_client->SetEventHandle(_bufferReady);
	cwr(_client->Start());

	_audioThread = std::thread([this, bufferSize](){
		_threadPlaying = true;
		BYTE* audioData = nullptr;
		uint64_t currentSample = 0;
		while(_threadPlaying)
		{
			DWORD res = WaitForSingleObject(_bufferReady, 2000);
			if (res != WAIT_OBJECT_0)
				throw std::runtime_error("Audio thread timed out!");

			uint32_t padding = 0;
			_client->GetCurrentPadding(&padding);
			uint32_t writableSize = bufferSize - padding;
			cwr(_renderClient->GetBuffer(writableSize, &audioData));

			//Fill buffer with something here;
			for(uint32_t fIndex = 0; fIndex < writableSize; ++fIndex)
			{
				BYTE* frame = audioData + fIndex * _format->nBlockAlign;
				//*(int32_t*)frame = static_cast<int32_t>(static_cast<int8_t>(std::sin(440 * (2 * 3.14159) * (double)fIndex / 10000000) * 0.5) * SCHAR_MAX) * (INT_MAX / SCHAR_MAX);
				float hertz1 = 59.0;
				float hertz2 = 75.0;
				float pi = 3.14159;
				float amplitude1 = std::sin(hertz1 * (2.0 * pi) * (float)currentSample++ / _format->nSamplesPerSec);
				*(float*)frame = amplitude1;

				float amplitude2 = std::sin(hertz2 * (2.0 * pi) * (float)currentSample++ / _format->nSamplesPerSec);
				*(float*)(frame + _format->nBlockAlign / 2) = amplitude2;
			}

			cwr(_renderClient->ReleaseBuffer(writableSize, 0));
		}
	});


}
#elif UNIX
void SoundDevice::initUnix()
{

}
#endif

SoundDevice::SoundDevice()
{
#ifdef WIN32
	initWindows();
#elif UNIX
	initUnix();
#endif
}

SoundDevice::~SoundDevice()
{
	_threadPlaying = false;
	if(_audioThread.joinable())
		_audioThread.join();

#ifdef WIN32
	CoTaskMemFree(_format);
#elif UNIX

#endif
}
