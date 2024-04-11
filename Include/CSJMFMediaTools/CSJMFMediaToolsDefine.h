#pragma once

#ifdef CSJMFMEDIATOOLS_EXPORTS
#define CSJMFMEDIATOOLS_API __declspec(dllexport)
#else
#define CSJMFMEDIATOOLS_API __declspec(dllimport)
#endif

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

class CSJMFAudioData {
public:
    CSJMFAudioData() : m_sample(NULL) {
    
    }
    CSJMFAudioData(IMFSample *sample) : m_sample(sample) {
        m_sample->AddRef();
    }
    ~CSJMFAudioData() {
        if (m_sample) {
            m_sample->Release();
        }
    }

    CSJMFAudioData(CSJMFAudioData&& audioData) {
        m_sample = audioData.m_sample;
    }

    CSJMFAudioData& operator=(CSJMFAudioData&& audioData) {
        m_sample = audioData.m_sample;
        return *this;
    }

    IMFSample* getData() {
        return m_sample;
    }

private:
    IMFSample *m_sample;
};
