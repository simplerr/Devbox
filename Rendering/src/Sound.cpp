#include "Sound.h"

Sound::Sound()
{
	mEngine = irrklang::createIrrKlangDevice();
	mMusic = NULL;
	mMusicSource = "none";
	// should read from a text file
	mMusicMuted = false;
	mEffectsMuted = false;
	mEffectVolume = mMusicVolume = 1.0f;
}

Sound::~Sound()
{
	if(mMusic != NULL)	{
		mMusic->stop();
		mMusic->drop();
	}
	mEngine->removeAllSoundSources();
	mEngine->drop(); 
}

irrklang::ISound* Sound::PlayMusic(string source, bool loop, bool track)
{
	//return 0;
	if(!mMusicMuted)	
	{
		if(mMusicSource != source)
		{
			if(mMusic != NULL)	{
				mMusic->stop();
				mMusic->drop();
				mMusic = NULL;
			}

			mMusicSource = source;
			mMusic = mEngine->play2D(mMusicSource.c_str(), loop, true, true);
			mMusic->setVolume(mMusicVolume);
			mMusic->setIsPaused(false);
		}
	}

	return NULL;
}

irrklang::ISound* Sound::PlayEffect(string source, float volume)
{
	//return 0;
	//irrklang::ISound *effect;
	if(!mEffectsMuted)	{
		irrklang::ISound* soundEffect = mEngine->play2D(source.c_str(), false, true, true);
		if(volume == -1)
			soundEffect->setVolume(mEffectVolume);
		else 
			soundEffect->setVolume(volume);

		soundEffect->setIsPaused(false);
	}

	return NULL;
}

void Sound::MuteMusic(bool mute)
{
	if(mute && mMusic != NULL)
	{
		mMusic->stop();
		mMusic->drop();
		mMusic = NULL;
		mMusicSource = "none";
	}

	mMusicMuted = mute;
}

void Sound::MuteEffects(bool mute)
{
	mEffectsMuted = mute;
}

bool Sound::GetMusicMuted(void)
{
	return mMusicMuted;
}

bool Sound::GetEffectsMuted(void)
{
	return mEffectsMuted;
}

void Sound::StopMusic(bool stop)
{
	if(stop)
		mMusic->stop();
}

void Sound::SetEffectVolume(float volume)
{
	mEffectVolume = volume;
}

void Sound::SetMusicVolume(float volume)
{
	mMusicVolume = volume;
	if(mMusic != NULL && !mMusicMuted)
		mMusic->setVolume(mMusicVolume);
}