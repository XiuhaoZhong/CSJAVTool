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
    CSJMFAudioData() :
        m_data(nullptr), 
        m_length(0) {
    
    }
    CSJMFAudioData(BYTE* data, DWORD length) {
        m_data = new BYTE[length];
        memcpy(m_data, data, length);
        m_length = length;
    }
    ~CSJMFAudioData() {
        if (m_data) {
            delete m_data;
            m_data = nullptr;
        }
    }

    CSJMFAudioData(CSJMFAudioData&& audioData) {
        m_data = audioData.m_data;
        m_length = audioData.m_length;

        audioData.m_data = nullptr;
    }

    CSJMFAudioData& operator=(CSJMFAudioData&& audioData) {
        m_data = audioData.m_data;
        m_length = audioData.m_length;

        audioData.m_data = nullptr;

        return *this;
    }

    BYTE* getData() {
        return m_data;
    }

    DWORD getLength() {
        return m_length;
    }

private:
    BYTE *m_data;
    DWORD m_length;
};
