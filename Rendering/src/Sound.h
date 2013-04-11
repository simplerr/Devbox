#pragma once

#include <string>
#include "C:\Program Files (x86)\irrKlang-1.3.0\include\irrKlang.h"
#pragma comment(lib, "C:\\Program Files (x86)\\irrKlang-1.3.0\\lib\\Win32-visualStudio\\irrKlang.lib")
using namespace std;

class Sound
{
public:
	Sound();
	~Sound();

	irrklang::ISound* PlayMusic(string source, bool loop, bool track);
	irrklang::ISound* PlayEffect(string source, float volume = -1);	// - 1 = use mVolume
	void StopMusic(bool stop);

	void MuteMusic(bool mute);
	void MuteEffects(bool mute);

	bool GetMusicMuted(void);
	bool GetEffectsMuted(void);

	void SetEffectVolume(float volume);
	void SetMusicVolume(float volume);
private:
	bool mMusicMuted;
	bool mEffectsMuted;
	float mEffectVolume;
	float mMusicVolume;

	irrklang::ISoundEngine* mEngine;
	irrklang::ISound* mMusic;
	std::string mMusicSource;
};