#include "Sound.h"
#include "WaveFile.h"

Sound::Sound()
{
	m_filename = NULL;
	m_sourceVoice = NULL;
	m_byteDataSize = 0;
	m_byteData = NULL;
	m_referenceCount = 0;
}
Sound::~Sound()
{
	Release();
}

void Sound::Release()
{
	//Destory the voice
	if(m_sourceVoice)
	{
		m_sourceVoice->DestroyVoice();
		m_sourceVoice = NULL;
	}
	//Delete the sound data
	if(m_byteData)
	{
		delete[] m_byteData;
		m_byteData = NULL;
	}
}

bool Sound::Load(IXAudio2* audioEngine, char* filename)
{
	//We'll use the WaveFile class for DXUT to load the waveform
	WaveFile wav;
	if (FAILED(wav.Open(filename, NULL, WAVEFILE_READ)))
	{
		return false;
	}

	// Get format of wave file
	WAVEFORMATEX* pwfx = wav.GetFormat();

	// Calculate how many bytes and samples are in the wave
	m_byteDataSize = wav.GetSize();

	// Read the sample data into memory
	m_byteData = new BYTE[m_byteDataSize];
	DWORD bytesRead;
	if(FAILED(wav.Read(m_byteData, m_byteDataSize, &bytesRead)))
	{
		return false;
	}
	
	// Create the source voice
	if (FAILED(audioEngine->CreateSourceVoice(&m_sourceVoice, pwfx, 0U, 2.0f, this)))
	{
		//If that fails then delete the sound data.
		if(m_byteData)
		{
			delete[] m_byteData;
			m_byteData = NULL;
		}
		return false;
	}

	//Set the filename and return true if all goes well.
	m_filename = filename;

	return true;
}

void Sound::Play()
{
	//Get the current state of the voice
	XAUDIO2_VOICE_STATE State;
	m_sourceVoice->GetState(&State);
	//If nothing is queued to play then...
	if (State.BuffersQueued <= 0)
	{		
		//we need to start it again
		Reset();
	}
	//Then start the voice playing
	m_sourceVoice->Start(0);
}

void Sound::Stop()
{
	//Get the current state of the voice
	XAUDIO2_VOICE_STATE State;
	m_sourceVoice->GetState(&State);
	//If there is something queued to play...
	if (State.BuffersQueued > 0)
	{
		//stop it.
		m_sourceVoice->Stop();
	}
}

void Sound::Reset()
{
	XAUDIO2_BUFFER buffer = {0};
	buffer.pAudioData = m_byteData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	buffer.AudioBytes = m_byteDataSize;

	m_sourceVoice->Stop();	//Stop the source buffer from trying to play any data, so we can load into it.
	m_sourceVoice->SubmitSourceBuffer(&buffer);
}

void _stdcall Sound::OnVoiceProcessingPassStart(UINT32 BytesRequired)
{
}

void _stdcall Sound::OnVoiceProcessingPassEnd()
{
}

void _stdcall Sound::OnStreamEnd()
{
}
    
void _stdcall Sound::OnBufferStart(void* pBufferContext)
{
}

void _stdcall Sound::OnBufferEnd(void* pBufferContext)
{

}

void _stdcall Sound::OnLoopEnd(void* pBufferContext)
{
}

void _stdcall Sound::OnVoiceError(void* pBufferContext, HRESULT Error)
{
}