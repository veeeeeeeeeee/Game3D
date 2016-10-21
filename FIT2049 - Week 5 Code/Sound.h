/*
 * Sound.h
 * This class represents a single sound effect in our game.
 * It is managed by the SoundManager but the object itself provides
 * methods to play, stop and reset. This class also implementeds the
 * IXAudio2VoiceCallback interface, meaning the sound object is also the
 * callback object.
 */

#ifndef SOUND_H_
#define SOUND_H_

#include <XAudio2.h>

class Sound : public IXAudio2VoiceCallback 
{
private:
	const char* m_filename;					//The filename of the .wav file
	IXAudio2SourceVoice* m_sourceVoice;		//The source voice object used by XAudio2
	unsigned int m_byteDataSize;			//The number of byte in the wave form
	BYTE* m_byteData;						//The byte data itself.
	int m_referenceCount;					//The current reference count of the object

protected:
	//Protected constructors, destructors and refence counting methods used by
	//the SoundManager class
	Sound();
	~Sound();
	void AddRef() { m_referenceCount++; }			
	void RemoveRef() { m_referenceCount--; }			
	int GetRefCount() { return m_referenceCount; }	
	void Release();

public:
	const char* GetFilename() { return m_filename; }
	bool Load(IXAudio2* audioEngine, char* filename);
	void Play();
	void Stop();
	void Reset();

	//The Call backs for this sound...
	void _stdcall OnVoiceProcessingPassStart(UINT32 BytesRequired);
	void _stdcall OnVoiceProcessingPassEnd();
	void _stdcall OnStreamEnd();
    void _stdcall OnBufferStart(void* pBufferContext);
    void _stdcall OnBufferEnd(void* pBufferContext);
    void _stdcall OnLoopEnd(void* pBufferContext);
    void _stdcall OnVoiceError(void* pBufferContext, HRESULT Error);

	//We're friends with the SoundManager.
	friend class SoundManager;
};

#endif