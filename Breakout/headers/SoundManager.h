#pragma once
#include "fmod.hpp"
class SoundManager
{
public: 
	FMOD::System* system; // virtual graphics card/device - virtual sound card
	FMOD::Sound* sound1, * sound2; // texture - memory where you sotre the sound FILE
	FMOD::Channel* channel = 0; // scene buffer - buffer where sound file that are going to be played are processed 
	FMOD_RESULT result; // HRESULT
	void* extradriverdate = 0;

	void InitializeAudio(); // Initialize the fmod sound system
	void PlaySound1(); // play sound
	void PlaySoundTrack();
	void LoadSounds();
	void UpdateSound();
};

