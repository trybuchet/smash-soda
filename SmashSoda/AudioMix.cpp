#include "AudioMix.h"

const std::vector<int16_t>& AudioMix::mix(const std::vector<int16_t>& buffer1, const std::vector<int16_t>& buffer2)
{
	static std::vector<int16_t> mixBuffer;
	constexpr float INV_INT16 = 1.0f / 32768.0f;
	constexpr float MIC_MIX_GAIN = 0.45f;
	constexpr float SPEAKER_MIX_GAIN = 1.0f;

	size_t larger = (std::max)(buffer1.size(), buffer2.size());
	mixBuffer.resize(larger);

	for (size_t i = 0; i < mixBuffer.size(); i++)
	{
		const bool hasA = i < buffer1.size();
		const bool hasB = i < buffer2.size();

		if (hasA && hasB) {
			// Mic (A) + speakers (B): keep speaker dominant and limit peaks.
			float mic = static_cast<float>(buffer1[i]) * INV_INT16;
			float speaker = static_cast<float>(buffer2[i]) * INV_INT16;
			float mixed = mic * MIC_MIX_GAIN + speaker * SPEAKER_MIX_GAIN;

			// Gentle limiter above full scale to avoid harsh clipping distortion.
			if (mixed > 1.0f) {
				mixed = 1.0f + (mixed - 1.0f) * 0.2f;
			}
			else if (mixed < -1.0f) {
				mixed = -1.0f + (mixed + 1.0f) * 0.2f;
			}

			if (mixed > 1.0f) mixed = 1.0f;
			if (mixed < -1.0f) mixed = -1.0f;

			mixBuffer[i] = static_cast<int16_t>(mixed * 32767.0f);
		}
		else if (hasA) {
			mixBuffer[i] = buffer1[i];
		}
		else if (hasB) {
			mixBuffer[i] = buffer2[i];
		}
		else {
			mixBuffer[i] = 0;
		}
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
	if (srcFrames < 2) {
		return buffer;
	}
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
		
		// Clamp to valid range.
		if (srcIndex >= srcFrames) {
			srcIndex = srcFrames - 1;
			frac = 0.0;
		}
		const size_t nextIndex = (srcIndex + 1 < srcFrames) ? (srcIndex + 1) : srcIndex;
		
		// Linear interpolation for left channel
		int16_t left1 = buffer[srcIndex * 2];
		int16_t left2 = buffer[nextIndex * 2];
		result[i * 2] = (int16_t)(left1 + frac * (left2 - left1));
		
		// Linear interpolation for right channel
		int16_t right1 = buffer[srcIndex * 2 + 1];
		int16_t right2 = buffer[nextIndex * 2 + 1];
		result[i * 2 + 1] = (int16_t)(right1 + frac * (right2 - right1));
	}
	
	return result;
}
