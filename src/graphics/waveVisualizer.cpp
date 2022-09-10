//
// Created by eli on 9/6/2022.
//

#include "waveVisualizer.h"
#include "imgui.h"
#include <vector>

WaveVisualizer::WaveVisualizer(SoundSource* soundSource)
{
	source = soundSource;
}

void WaveVisualizer::draw()
{
	size_t width = (size_t)ImGui::GetWindowContentRegionWidth();
	float timePeriod = (endTime - startTime) / 1000;
	uint64_t samplesPerSec = (uint64_t)((1 / timePeriod) * (float)width);

	std::vector<float> samples;
	samples.reserve(width);

	for (int64_t i = 0; i < width; ++i)
	{
		auto [left, right] = source->getSample(startTime + ((float)i / (float)samplesPerSec));
		samples.push_back(left);
	}

	ImGui::PlotLines("##wv", samples.data(), samples.size(), 0, nullptr, -1.2, 1.2, {(float)width, 200});
}
