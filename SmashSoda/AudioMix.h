#pragma once

#include <vector>
#include <algorithm>
#include <cstdint>

class AudioMix
{
public:
	static const std::vector<int16_t>& mix(const std::vector<int16_t>& buffer1, const std::vector<int16_t>& buffer2);
	
	// Resample stereo audio buffer from one sample rate to another using linear interpolation
	// streamKey separates state between independent streams to keep phase continuity per source.
	static std::vector<int16_t> resample(const std::vector<int16_t>& buffer, uint32_t srcRate, uint32_t dstRate, uint64_t streamKey = 0);
};




