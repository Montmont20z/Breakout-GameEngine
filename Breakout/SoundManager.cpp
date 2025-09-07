#include "headers/SoundManager.h"

bool SoundManager::Initialize()
{
	result = FMOD::System_Create(&system);
	result = system->init(32, FMOD_INIT_NORMAL, extradriverdate);
	if (result != FMOD_OK) return false;
	return true;
}

void SoundManager::PlaySound1()
{
	result = system->playSound(sound1, 0, false, &channel);
}

void SoundManager::PlaySoundTrack()
{
	result = system->playSound(sound2, 0, false, &channel);
}

void SoundManager::LoadSounds()
{
	result = system->createSound("assets/128294__xenonn__layered-gunshot-2.wav", FMOD_DEFAULT, 0,	&sound1);
	result = sound1->setMode(FMOD_LOOP_OFF);

	result = system->createStream("assets/never_gonna_give_you_up.mp3", FMOD_DEFAULT, 0,	&sound2);
	result = sound1->setMode(FMOD_LOOP_OFF);

}

void SoundManager::UpdateSound()
{
	result = system->update();

}
