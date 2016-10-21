#include "SoundManager.h"

using namespace std;

SoundManager::SoundManager()
{
	m_xAudioSystem = NULL;
	m_masteringVoice = NULL;
}

SoundManager::~SoundManager()
{
	Release();
}

bool SoundManager::Initialise()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	UINT32 u32Flags = 0;

	//Create an XAudio2 interface
	if(FAILED(XAudio2Create(&m_xAudioSystem, u32Flags)))
	{
		return false;
	}

	//Create a mastering voice
	if (FAILED(m_xAudioSystem->CreateMasteringVoice(&m_masteringVoice)))
	{
		if(m_xAudioSystem)
		{
			m_xAudioSystem->Release();
			m_xAudioSystem = NULL;
		}
		return false;
	}

	//Start the XAudio2 Engine
	m_xAudioSystem->StartEngine();

	return true;
}

bool SoundManager::Load(char* filename)
{
	Sound* tempSound = NULL;

	if(filename == 0)					//If the filename is empty then return
	{
		return false;
	}

	if(GetSound(filename) != NULL)	//If the texture is already loaded then return
	{
		return true;
	}

	tempSound = new Sound();		//Create a new Sound Object

	if(tempSound->Load(m_xAudioSystem, filename))		//Attempt to load the file
	{
		string filenameString = filename;
		m_soundMap[filenameString] = tempSound;	//If loaded then add sound to the map
		tempSound = NULL;
		return true;
	}
	else
	{
		delete tempSound;						//If the sound didn't load delete the Texture object
		tempSound = NULL;
		return false;
	}
}

Sound* SoundManager::GetSound(char* filename)
{
	string filenameString = filename;		//Convert the char* to a std::string

	//Use the find method to get an iterator pointing to the sound stored with the filename
	SoundMap::const_iterator searchResult = m_soundMap.find(filenameString);

	if(searchResult != m_soundMap.end())	//If the sound is found
	{
		searchResult->second->AddRef();		//Increase the reference counter
		return searchResult->second;		//And return it
	}

	return NULL;
}

void SoundManager::ReleaseSound(Sound* sound)
{
	if(sound)
	{
		sound->RemoveRef();
		if(sound->GetRefCount() <= 0)
		{
			string filename = sound->GetFilename();
			SoundMap::const_iterator searchResult = m_soundMap.find(filename);
			if(searchResult != m_soundMap.end())
			{
				m_soundMap.erase(searchResult);
			}
			delete sound;
			sound = NULL;
		}
	}
}

void SoundManager::Release()
{
	if(!m_soundMap.empty())
	{
		SoundMap::iterator mapIterator;
		for(mapIterator = m_soundMap.begin(); mapIterator != m_soundMap.end(); mapIterator++)
		{
			mapIterator->second->Release();
			mapIterator->second = NULL;
		}

		m_soundMap.clear();
	}
}