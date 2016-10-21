/*
 * SoundManager.h
 * This class is similar to the Texture and Mesh Managers.
 * It creates and manages a reference counted map of sound objects.
 * It also looks after the initialisation and management of the XAudio2
 * interface and mastering voice.
 */
#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#include <map>
#include <string>
#include <Xaudio2.h>
#include "Sound.h"

#pragma comment(lib, "Xaudio2.lib")

class SoundManager
{
private:
	typedef std::map<std::string, Sound*> SoundMap;
	SoundMap m_soundMap;
	IXAudio2* m_xAudioSystem;
	IXAudio2MasteringVoice* m_masteringVoice;
public:
	SoundManager();
	~SoundManager();
	bool Initialise();
	bool Load(char* filename);
	Sound* GetSound(char* filename);
	void ReleaseSound(Sound* sound);
	void Release();
};

#endif