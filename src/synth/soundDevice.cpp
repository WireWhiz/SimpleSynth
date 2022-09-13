//
// Created by eli on 9/6/2022.
//

#include <string>
#include "soundDevice.h"
#include <stdexcept>
#include "soundSource.h"
#include <tuple>
#include <bit>
#include <cassert>

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
	_samplesPerSec = _format->nSamplesPerSec;
	if(_format->cbSize >= 22)
	{
		WAVEFORMATEXTENSIBLE* pwfxx = (WAVEFORMATEXTENSIBLE*)_format;
		KSDATAFORMAT_SUBTYPE_PCM;
		KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
	}

	cwr(_client->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 64, 0, _format, nullptr));
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

				Sample sample = getSample((double)_currentSample / (double)_samplesPerSec);
				*(float*)frame = sample.left;
				*(float*)(frame + _format->nBlockAlign / _format->nChannels) = sample.right;
				++_currentSample;
			}

			cwr(_renderClient->ReleaseBuffer(writableSize, 0));
		}
	});


}
#elif __linux__
void cr(int res)
{
    if(res != 0)
        throw std::runtime_error("Pulse Audio Error: " + (std::string)pa_strerror(res));
}

void SoundDevice::initLinux()
{
    _mainLoop =  pa_threaded_mainloop_new();
    cr(pa_threaded_mainloop_start(_mainLoop));

    pa_operation* o = nullptr;
    pa_threaded_mainloop_lock(_mainLoop);


    auto* api = pa_threaded_mainloop_get_api(_mainLoop);
    _ctx = pa_context_new( api, "SimpleSynthCtx");

    pa_context_set_state_callback(_ctx, [](pa_context* ctx, void* userdata) {
        auto* device = static_cast<SoundDevice*>(userdata);
        auto state = pa_context_get_state(ctx);

        if(state == pa_context_state::PA_CONTEXT_FAILED)
            throw std::runtime_error("Could not connect audio context!");
        if(state != pa_context_state::PA_CONTEXT_READY)
            return;

        pa_sample_spec spec{};
        spec.channels = 2;
        spec.format = (std::endian::native == std::endian::big) ? pa_sample_format::PA_SAMPLE_FLOAT32BE : pa_sample_format::PA_SAMPLE_FLOAT32LE;
        spec.rate = 48000;
        device->_samplesPerSec = 48000;

        pa_channel_map map;
        pa_channel_map_init_stereo(&map);

        device->_outStream = pa_stream_new(ctx, "SimpleSynthOutStream", &spec, &map);

        pa_buffer_attr attr{};
        attr.maxlength = -1;
        attr.fragsize = -1;
        attr.minreq = -1;
        attr.prebuf = -1;
        attr.tlength = sizeof(float) * 2 * 64;

        cr(pa_stream_connect_playback(device->_outStream, nullptr, &attr, pa_stream_flags::PA_STREAM_START_UNMUTED, nullptr, nullptr));


        pa_stream_set_write_callback(device->_outStream, [](pa_stream* stream, size_t nBytes, void* userdata){
            auto* device = (SoundDevice*)userdata;
            nBytes = pa_stream_writable_size(stream);
            float* data = (float*)new uint8_t[nBytes];
            // Sizeof float32 * 2 for the two streams
            size_t sampleCount = nBytes / 8;

            for(size_t sample = 0; sample < sampleCount; ++sample)
            {
                Sample s = device->getSample((double)device->_currentSample++ / (double)device->_samplesPerSec);
                data[sample * 2] = s.left;
                data[sample * 2 + 1] = s.right;
            }

            cr(pa_stream_write(stream, (void*)data, nBytes, [](void* data){delete[] (float*)data;}, 0, pa_seek_mode::PA_SEEK_RELATIVE));

        }, device);
    }, this);

    cr(pa_context_connect(_ctx, nullptr, pa_context_flags::PA_CONTEXT_NOFLAGS, nullptr));

    pa_threaded_mainloop_unlock(_mainLoop);
}
#endif

SoundDevice::SoundDevice()
{
#ifdef WIN32
	initWindows();
#elif __linux__
    initLinux();
#endif
}

SoundDevice::~SoundDevice()
{

#ifdef WIN32
    _threadPlaying = false;
	if(_audioThread.joinable())
		_audioThread.join();
	CoTaskMemFree(_format);
#elif __linux__
    pa_stream_disconnect(_outStream);

    pa_context_disconnect(_ctx);

    pa_threaded_mainloop_stop(_mainLoop);
    pa_threaded_mainloop_free(_mainLoop);
#endif
}

void SoundDevice::addSource(SoundSource* source)
{
	_sources.push_back(source);
}

void SoundDevice::removeSource(SoundSource* source)
{
	auto i = _sources.begin();
	while(i != _sources.end())
	{
		if(*i == source)
		{
			_sources.erase(i);
			return;
		}
	}
	assert(false); //Could not remove source
}

double SoundDevice::currentTime() const
{
	return (double)_currentSample / (double)_samplesPerSec;
}

Sample SoundDevice::getSample(double time)
{
    Sample sample{0,0};
    for(SoundSource* s : _sources)
        sample += s->getSample(time);
    return sample;
}
