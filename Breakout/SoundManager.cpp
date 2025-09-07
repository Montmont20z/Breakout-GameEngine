#include "headers/SoundManager.h"

void SoundManager::InitializeAudio()
{
	result = FMOD::System_Create(&system);
	result = system->init(32, FMOD_INIT_NORMAL, extradriverdate);
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

	result = system->createStream("assets/659278__seth_makes_sounds__lofi-guitar-beat-70bpm.wav", FMOD_DEFAULT, 0,	&sound2);
	result = sound1->setMode(FMOD_LOOP_OFF);

}

void SoundManager::UpdateSound()
{
	result = system->update();

}
