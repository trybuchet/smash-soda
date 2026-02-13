#include "AudioSource.h"
#include "helpers/WStringConvert.h"

// ==================================================
//   Constants
// ==================================================
#define AUDIOSRC_TARGET_CHANNELS 2


// ==================================================
//   MACROS
// ==================================================
#define SAFE_RELEASE(dirty) if (dirty != nullptr) { dirty->Release(); dirty = nullptr; }
#define FAIL_EXIT(HR) if(FAILED(HR)) goto EXIT;


bool AudioSource::setDevice(int index)
{
	HRESULT hr;
	size_t REFTIMES_PER_SEC = 400000;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;

	IMMDeviceEnumerator* pEnumerator = nullptr;
	IMMDeviceCollection* pCollection = nullptr;
	IMMDevice* pDevice = nullptr;
	IAudioClient* pAudioClient = nullptr;
	WAVEFORMATEX* pwfx = nullptr;
	LPWSTR pwszID = nullptr;
	IPropertyStore* pProps = nullptr;
	bool isDefaultFormat = false;
	WORD bitsPerSample = 0;

	uint32_t freq = 1000;

	auto releaseLocal = [&]()
	{
		CoTaskMemFree(pwszID);
		if (!isDefaultFormat) CoTaskMemFree(pwfx);
		SAFE_RELEASE(pEnumerator);
		SAFE_RELEASE(pCollection);
		SAFE_RELEASE(pDevice);
		SAFE_RELEASE(pAudioClient);
		SAFE_RELEASE(pProps);
	};

	auto releaseAll = [&]() {
		releaseLocal();
		SAFE_RELEASE(m_pCaptureClient);
	};


	m_mutex.lock();

	m_buffers[0].clear();
	m_buffers[1].clear();

	releaseAll();

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
	FAIL_EXIT(hr);

	hr = pEnumerator->EnumAudioEndpoints(m_eDataFlow, DEVICE_STATE_ACTIVE, &pCollection);
	FAIL_EXIT(hr);

	UINT count;
	hr = pCollection->GetCount(&count);
	FAIL_EXIT(hr);
	if (index >= count) goto EXIT;

	hr = pCollection->Item(index, &pDevice);
	if (FAILED(hr))
	{
		releaseAll();
		hr = pEnumerator->GetDefaultAudioEndpoint(m_eDataFlow, eConsole, &pDevice);
		FAIL_EXIT(hr);
	}

	hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
	FAIL_EXIT(hr);

	hr = pAudioClient->GetMixFormat(&pwfx);
	FAIL_EXIT(hr);

	WAVEFORMATEXTENSIBLE preferredFormat;
	ZeroMemory(&preferredFormat, sizeof(WAVEFORMATEXTENSIBLE));
	preferredFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	preferredFormat.Format.nChannels = 2;
	preferredFormat.Format.nSamplesPerSec = 48000;
	preferredFormat.Format.wBitsPerSample = 32;
	preferredFormat.Format.nBlockAlign = (preferredFormat.Format.nChannels * preferredFormat.Format.wBitsPerSample) / 8;
	preferredFormat.Format.nAvgBytesPerSec = preferredFormat.Format.nSamplesPerSec * preferredFormat.Format.nBlockAlign;
	preferredFormat.Format.cbSize = 22;
	preferredFormat.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
	preferredFormat.Samples.wValidBitsPerSample = 32;
	preferredFormat.dwChannelMask = KSAUDIO_SPEAKER_STEREO;

	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		m_streamFlags,
		hnsRequestedDuration,
		0,
		&preferredFormat.Format,
		NULL);

	if (SUCCEEDED(hr)) {
		CoTaskMemFree(pwfx);
		pwfx = &preferredFormat.Format;
		isDefaultFormat = true;
	}
	else {
		hr = pAudioClient->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			m_streamFlags,
			hnsRequestedDuration,
			0,
			pwfx,
			NULL);
	}

	FAIL_EXIT(hr);

	freq = (uint32_t)pwfx->nSamplesPerSec;
	setFrequency((uint32_t) pwfx->nSamplesPerSec);

	// Cache source format so capture path can decode correctly.
	m_sourceChannels = pwfx->nChannels > 0 ? pwfx->nChannels : 1;
	m_outputChannels = AUDIOSRC_TARGET_CHANNELS;
	m_sampleFormat = SampleFormat::Unknown;
	m_bytesPerSample = 0;

	bitsPerSample = pwfx->wBitsPerSample;
	if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		const WAVEFORMATEXTENSIBLE* ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(pwfx);
		bitsPerSample = ext->Samples.wValidBitsPerSample ? ext->Samples.wValidBitsPerSample : pwfx->wBitsPerSample;
		if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT && bitsPerSample == 32) {
			m_sampleFormat = SampleFormat::Float32;
			m_bytesPerSample = 4;
		}
		else if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) {
			if (bitsPerSample == 16) {
				m_sampleFormat = SampleFormat::Int16;
				m_bytesPerSample = 2;
			}
			else if (bitsPerSample == 32) {
				m_sampleFormat = SampleFormat::Int32;
				m_bytesPerSample = 4;
			}
		}
	}
	else if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT && bitsPerSample == 32) {
		m_sampleFormat = SampleFormat::Float32;
		m_bytesPerSample = 4;
	}
	else if (pwfx->wFormatTag == WAVE_FORMAT_PCM) {
		if (bitsPerSample == 16) {
			m_sampleFormat = SampleFormat::Int16;
			m_bytesPerSample = 2;
		}
		else if (bitsPerSample == 32) {
			m_sampleFormat = SampleFormat::Int32;
			m_bytesPerSample = 4;
		}
	}

	if (m_sampleFormat == SampleFormat::Unknown || m_bytesPerSample == 0) {
		goto EXIT;
	}

	// Gets the size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	FAIL_EXIT(hr);

	hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&m_pCaptureClient);
	FAIL_EXIT(hr);

	// Calculates the actual duration of the allocated buffer.
	hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

	// Starts recording.
	hr = pAudioClient->Start();
	FAIL_EXIT(hr);

	m_maxBufferSize = AUDIOSRC_SAMPLES_PER_BUFFER * m_outputChannels;

	currentDevice = m_devices[index];

	releaseLocal();

	m_mutex.unlock();
	return true;

EXIT:
	releaseAll();
	m_mutex.unlock();
	return false;
}

const std::vector<AudioSourceDevice> AudioSource::getDevices()
{
	return m_devices;
}

void AudioSource::captureAudio()
{
	if (!isEnabled) return;

	m_mutex.lock();

	HRESULT hr;
	BYTE* pData;
	UINT32 numFramesInPacket = 0;
	UINT32 numFramesAvailable;
	DWORD flags;

	if (m_pCaptureClient == NULL) {
		m_mutex.unlock();
		setDevice();
		return;
	}

	hr = m_pCaptureClient->GetNextPacketSize(&numFramesInPacket);
	FAIL_EXIT(hr);

	while (numFramesInPacket != 0)
	{
		// Gets the available data in the shared buffer.
		hr = m_pCaptureClient->GetBuffer(
			&pData,
			&numFramesAvailable,
			&flags, NULL, NULL
		);
		FAIL_EXIT(hr);

		if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
		{
			pData = NULL;  // Fill zeros for these frames to preserve cadence.
		}

		const size_t frameStride = m_sourceChannels * m_bytesPerSample;
		if (frameStride == 0) {
			hr = E_FAIL;
			FAIL_EXIT(hr);
		}

		auto sampleToInt16 = [&](const BYTE* samplePtr) -> int16_t {
			float sampleF = 0.0f;
			switch (m_sampleFormat) {
			case SampleFormat::Float32:
				sampleF = *reinterpret_cast<const float*>(samplePtr);
				break;
			case SampleFormat::Int16:
				sampleF = static_cast<float>(*reinterpret_cast<const int16_t*>(samplePtr)) / 32768.0f;
				break;
			case SampleFormat::Int32:
				sampleF = static_cast<float>(*reinterpret_cast<const int32_t*>(samplePtr)) / 2147483648.0f;
				break;
			default:
				sampleF = 0.0f;
				break;
			}

			sampleF = (std::max)(-1.0f, (std::min)(1.0f, sampleF));
			const int scaled = static_cast<int>(sampleF * 32767.0f * volume);
			return static_cast<int16_t>((std::max)(-32768, (std::min)(32767, scaled)));
		};

		auto pushFrame = [&](int16_t left, int16_t right) {
			if (!isEnabled) {
				left = 0;
				right = 0;
			}

			if (m_buffers[m_activeBuffer].size() + 2 > m_maxBufferSize) {
				swapBuffers();
				m_buffers[m_activeBuffer].clear();
				m_isReady = true;
			}

			m_buffers[m_activeBuffer].push_back(left);
			m_buffers[m_activeBuffer].push_back(right);
		};

		if (pData == NULL) {
			for (UINT32 frame = 0; frame < numFramesAvailable; ++frame) {
				pushFrame(0, 0);
			}
		}
		else {
			for (UINT32 frame = 0; frame < numFramesAvailable; ++frame) {
				const BYTE* frameData = pData + static_cast<size_t>(frame) * frameStride;
				const int16_t left = sampleToInt16(frameData);
				int16_t right = left;
				if (m_sourceChannels >= 2) {
					right = sampleToInt16(frameData + m_bytesPerSample);
				}
				pushFrame(left, right);
			}
		}

		if (m_isPlotActive)
		{
			const size_t frameCount = (std::min)(m_buffers[m_activeBuffer].size() / 2, static_cast<size_t>(AUDIOSRC_SAMPLES_PER_BUFFER));
			for (size_t i = 0; i < frameCount; ++i)
			{
				const size_t sampleIndex = i * 2;
				m_plotBuffer[i] = 0.5f * (static_cast<float>(m_buffers[m_activeBuffer][sampleIndex]) + static_cast<float>(m_buffers[m_activeBuffer][sampleIndex + 1]));
			}
		}

		FAIL_EXIT(hr);

		hr = m_pCaptureClient->ReleaseBuffer(numFramesAvailable);
		FAIL_EXIT(hr);

		hr = m_pCaptureClient->GetNextPacketSize(&numFramesInPacket);
		FAIL_EXIT(hr);

	}

	m_mutex.unlock();
	return;

EXIT:
	SAFE_RELEASE(m_pCaptureClient);
	m_mutex.unlock();
}

bool AudioSource::isReady()
{
	return m_isReady;
}

const std::vector<int16_t> AudioSource::popBuffer()
{
	if (!m_isReady)
	{
		return std::vector<int16_t>();
	}

	m_isReady = false;
	return std::vector<int16_t>(m_buffers[1 - m_activeBuffer]);
}

const int AudioSource::popPreviewDecibel()
{
	int inactiveBuffer;
	inactiveBuffer = 1 - m_activeBuffer;

	if (m_previewIndex >= 0 && m_previewIndex < m_buffers[inactiveBuffer].size())
	{
		int decibelValue;
		decibelValue = AudioTools::previewDecibel(isEnabled ? m_buffers[inactiveBuffer][m_previewIndex] : 0);
		m_previewIndex++;

		return decibelValue;
	}

	return AUDIOTOOLS_PREVIEW_MIN_DB;
}

const float* AudioSource::getPlot()
{
	return m_plotBuffer;
}

const void AudioSource::togglePlot(const bool enabled)
{
	m_isPlotActive = enabled;
}

// TODO Better check if the release here interferes with capture loop
void AudioSource::fetchDevices()
{
	IMMDeviceEnumerator* pEnumerator = nullptr;
	IMMDeviceCollection* pCollection = nullptr;
	IMMDevice* pEndpoint = nullptr;
	IPropertyStore* pProps = nullptr;
	LPWSTR pwszID = nullptr;

	HRESULT hr;

	auto releaseAll = [&]() {
		CoTaskMemFree(pwszID);
		SAFE_RELEASE(pEnumerator);
		SAFE_RELEASE(pCollection);
		SAFE_RELEASE(pEndpoint);
		SAFE_RELEASE(pProps);
	};

	releaseAll();

	CoInitialize(nullptr);
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator
	);
	FAIL_EXIT(hr);

	hr = pEnumerator->EnumAudioEndpoints(m_eDataFlow, DEVICE_STATE_ACTIVE, &pCollection);
	FAIL_EXIT(hr);

	UINT count;
	hr = pCollection->GetCount(&count);
	FAIL_EXIT(hr);

	m_devices.clear();
	if (count == 0) { std::cerr << "No endpoints found!" << std::endl; }
	for (size_t i = 0; i < count; i++)
	{
		hr = pCollection->Item(i, &pEndpoint);
		FAIL_EXIT(hr);

		hr = pEndpoint->GetId(&pwszID);
		FAIL_EXIT(hr);

		hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
		FAIL_EXIT(hr);

		PROPVARIANT varName;
		PropVariantInit(&varName);
		hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
		FAIL_EXIT(hr);

		std::wstring wname(varName.pwszVal);
		std::wstring wid(pwszID);
				std::string name = WStringToString(wname);

				std::string id = WStringToString(wid);

		printf("Endpoint %d: \"%S\" (%S)\n", i, varName.pwszVal, pwszID);
		m_devices.push_back({ name, id, i });

		CoTaskMemFree(pwszID);
		pwszID = NULL;
		PropVariantClear(&varName);

		SAFE_RELEASE(pProps);
		SAFE_RELEASE(pEndpoint);
	}

EXIT:
	releaseAll();

	return;
}

void AudioSource::setFrequency(uint32_t  frequency)
{
	m_frequency = frequency;
}

const uint32_t AudioSource::getFrequency() const
{
	return m_frequency;
}


// ====================================================
//   PRIVATE
// ====================================================
void AudioSource::swapBuffers()
{
	m_activeBuffer = 1 - m_activeBuffer;
	m_previewIndex = 0;
}

void AudioSource::setEDataFlow(EDataFlow eDataFlow)
{
	m_eDataFlow = eDataFlow;
}

void AudioSource::setStreamFlags(DWORD streamFlags)
{
	m_streamFlags = streamFlags;
}

