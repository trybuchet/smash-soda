#include "AudioMix.h"
#include <cmath>
#include <unordered_map>

const std::vector<int16_t>& AudioMix::mix(const std::vector<int16_t>& buffer1, const std::vector<int16_t>& buffer2)
{
	// Thread-local to avoid cross-thread clobbering when multiple mixes happen concurrently.
	thread_local std::vector<int16_t> mixBuffer;
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

std::vector<int16_t> AudioMix::resample(const std::vector<int16_t>& buffer, uint32_t srcRate, uint32_t dstRate, uint64_t streamKey)
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
	const uint64_t scaledFrames = static_cast<uint64_t>(srcFrames) * static_cast<uint64_t>(dstRate);
	size_t dstFrames = static_cast<size_t>((scaledFrames + (srcRate / 2)) / srcRate);
	
	if (dstFrames == 0) {
		return buffer;
	}

	std::vector<int16_t> result(dstFrames * 2);

	struct ResampleState {
		double carry = 0.0;
		uint32_t src = 0;
		uint32_t dst = 0;
	};

	static thread_local std::unordered_map<uint64_t, ResampleState> states;
	const uint64_t stateKey = streamKey ^ (static_cast<uint64_t>(srcRate) << 32) ^ static_cast<uint64_t>(dstRate);
	ResampleState& st = states[stateKey];

	if (st.src != srcRate || st.dst != dstRate) {
		st.carry = 0.0;
		st.src = srcRate;
		st.dst = dstRate;
	}

	const double step = static_cast<double>(srcRate) / static_cast<double>(dstRate);
	double srcCursor = st.carry;

	for (size_t i = 0; i < dstFrames; i++) {
		size_t srcIndex = static_cast<size_t>(srcCursor);
		double frac = srcCursor - static_cast<double>(srcIndex);

		// Clamp to valid range.
		if (srcIndex >= srcFrames) {
			srcIndex = srcFrames - 1;
			frac = 0.0;
		}
		const size_t nextIndex = (srcIndex + 1 < srcFrames) ? (srcIndex + 1) : srcIndex;

		// Linear interpolation for left channel
		const int16_t left1 = buffer[srcIndex * 2];
		const int16_t left2 = buffer[nextIndex * 2];
		result[i * 2] = static_cast<int16_t>(left1 + frac * (left2 - left1));

		// Linear interpolation for right channel
		const int16_t right1 = buffer[srcIndex * 2 + 1];
		const int16_t right2 = buffer[nextIndex * 2 + 1];
		result[i * 2 + 1] = static_cast<int16_t>(right1 + frac * (right2 - right1));

		srcCursor += step;
	}

	// Preserve leftover fractional position for the next call so streams stay phase-continuous.
	double carry = srcCursor - static_cast<double>(srcFrames);
	if (carry < 0.0) {
		carry = 0.0;
	}
	// Avoid unbounded growth if caller feeds tiny buffers.
	const double maxCarry = step * 4.0;
	if (carry > maxCarry) {
		carry = std::fmod(carry, step);
	}
	st.carry = carry;

	return result;
}
