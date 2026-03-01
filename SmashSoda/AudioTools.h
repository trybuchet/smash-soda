#pragma once

#include <iostream>
#include <algorithm>
#include <cmath>

#define AUDIOTOOLS_PREVIEW_MIN_DB -60
#define AUDIOTOOLS_PREVIEW_MAX_AMP 32768
#define AUDIOTOOLS_PREVIEW_MIN_AMP 0.001f

using namespace std;

class AudioTools
{
public:
	static const int previewDecibel(int16_t amplitude)
	{
		const float relativeAmp = std::abs(static_cast<float>(amplitude) / static_cast<float>(AUDIOTOOLS_PREVIEW_MAX_AMP));
		const float relativeAmpClamped = max(AUDIOTOOLS_PREVIEW_MIN_AMP, relativeAmp);
		const int decibelValue = static_cast<int>(20.0f * log10(relativeAmpClamped));

		return max(AUDIOTOOLS_PREVIEW_MIN_DB, decibelValue);
	}

	static const float decibelToFloat(int decibel)
	{
		static const int absMinDb = abs(AUDIOTOOLS_PREVIEW_MIN_DB);
		return ((float)(decibel + absMinDb)) / ((float)absMinDb);
	}
};




