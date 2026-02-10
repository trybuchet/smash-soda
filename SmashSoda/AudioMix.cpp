#include "AudioMix.h"

const std::vector<int16_t>& AudioMix::mix(const std::vector<int16_t>& buffer1, const std::vector<int16_t>& buffer2)
{
	static std::vector<int16_t> mixBuffer;

	//size_t shorter = std::min(buffer1.size(), buffer2.size());
	size_t larger = (std::max)(buffer1.size(), buffer2.size());
	mixBuffer.resize(larger);

	for (size_t i = 0; i < mixBuffer.size(); i++)
	{
		mixBuffer[i] = 0;
		if (i < buffer1.size()) mixBuffer[i] += buffer1[i];
		if (i < buffer2.size()) mixBuffer[i] += buffer2[i];
	}


	return mixBuffer;
}

std::vector<int16_t> AudioMix::resample(const std::vector<int16_t>& buffer, uint32_t srcRate, uint32_t dstRate)
{
	// No resampling needed if rates match or buffer is empty
	if (srcRate == dstRate || buffer.empty() || srcRate == 0 || dstRate == 0) {
		return buffer;
	}

	// Buffer is stereo (interleaved L, R, L, R, ...)
	size_t srcFrames = buffer.size() / 2;
	size_t dstFrames = (size_t)((double)srcFrames * dstRate / srcRate);
	
	if (dstFrames == 0) {
		return buffer;
	}

	std::vector<int16_t> result(dstFrames * 2);
	
	double ratio = (double)srcRate / dstRate;
	
	for (size_t i = 0; i < dstFrames; i++) {
		double srcPos = i * ratio;
		size_t srcIndex = (size_t)srcPos;
		double frac = srcPos - srcIndex;
		
		// Clamp to valid range
		if (srcIndex >= srcFrames - 1) {
			srcIndex = srcFrames - 1;
			frac = 0.0;
		}
		
		// Linear interpolation for left channel
		int16_t left1 = buffer[srcIndex * 2];
		int16_t left2 = buffer[(srcIndex + 1) * 2];
		result[i * 2] = (int16_t)(left1 + frac * (left2 - left1));
		
		// Linear interpolation for right channel
		int16_t right1 = buffer[srcIndex * 2 + 1];
		int16_t right2 = buffer[(srcIndex + 1) * 2 + 1];
		result[i * 2 + 1] = (int16_t)(right1 + frac * (right2 - right1));
	}
	
	return result;
}


