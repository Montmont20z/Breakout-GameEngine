#include "headers/SoundManager.h"

bool SoundManager::Initialize()
{
    if (!Check(FMOD::System_Create(&system))) return false;
    if (!Check(system->init(32, FMOD_INIT_NORMAL, extradriverdate))) return false;

    // Create groups
    if (!Check(system->createChannelGroup("Master", &masterGroup))) return false;
    if (!Check(system->createChannelGroup("Music", &musicGroup))) return false;
    if (!Check(system->createChannelGroup("SFX", &sfxGroup))) return false;

    // Attach to master
    if (!Check(masterGroup->addGroup(musicGroup))) return false;
    if (!Check(masterGroup->addGroup(sfxGroup)))   return false;
    system->getMasterChannelGroup(&masterGroup);

	// Example: set volume for all sounds
	if (masterGroup) masterGroup->setVolume(0.5f);  // 50% volume

    // Route system output to master
    //if (!Check(system->setMasterChannelGroup(masterGroup))) return false;

    // Default volumes
    SetVolume(Bus::Master, 1.0f);
    SetVolume(Bus::Music, 1.0f);
    SetVolume(Bus::SFX, 1.0f);
    return true;
}

void SoundManager::Shutdown()
{
    // Release all sounds
    for (auto& kv : sounds) {
        if (kv.second.sound) kv.second.sound->release();
    }
    sounds.clear();

    // Release groups
    if (sfxGroup) { sfxGroup->release();    sfxGroup = nullptr; }
    if (musicGroup) { musicGroup->release();  musicGroup = nullptr; }
    if (masterGroup) { masterGroup->release(); masterGroup = nullptr; }

    if (system) {
        system->close();
        system->release();
        system = nullptr;
    }
}

void SoundManager::Update()
{
    if (system) system->update();
}

bool SoundManager::Load(const std::string& alias, const std::string& path, bool stream, bool loop)
{
    if (!system) return false;

    FMOD_MODE mode = FMOD_DEFAULT;
    if (loop) mode |= FMOD_LOOP_NORMAL;
    else      mode |= FMOD_LOOP_OFF;

    FMOD::Sound* snd = nullptr;
    if (stream) {
        if (!Check(system->createStream(path.c_str(), mode, 0, &snd))) return false;
    }
    else {
        if (!Check(system->createSound(path.c_str(), mode, 0, &snd))) return false;
    }

    SoundEntry e;
    e.sound = snd;
    e.isMusic = stream; // treat streams as music by default
    sounds[alias] = e;
    return true;
}

bool SoundManager::Play(const std::string& alias, bool paused)
{
    auto it = sounds.find(alias);
    if (it == sounds.end() || !system) return false;

    FMOD::Channel* ch = nullptr;
    FMOD::ChannelGroup* group = it->second.isMusic ? musicGroup : sfxGroup;

    if (!Check(system->playSound(it->second.sound, group, paused, &ch))) return false;
    // Optionally tweak per-play settings here (3D pos, pitch, etc.)
    return true;
}

void SoundManager::Stop(const std::string& alias)
{
    // Simple approach: pause/stop all channels in the group that likely owns the alias.
    // If you need per-alias stop only, you can track channels returned by Play().
    auto it = sounds.find(alias);
    if (it == sounds.end()) return;

    FMOD::ChannelGroup* group = it->second.isMusic ? musicGroup : sfxGroup;
    if (group) {
        // Stop all in that group (coarse). For finer control, track channels from Play().
        group->stop();
    }
}

void SoundManager::StopAll()
{
    if (masterGroup) masterGroup->stop();
}

void SoundManager::SetVolume(Bus bus, float volume)
{
    if (volume < 0.f) volume = 0.f;
    if (volume > 1.f) volume = 1.f;

    FMOD::ChannelGroup* g = nullptr;
    switch (bus) {
    case Bus::Master: g = masterGroup; break;
    case Bus::Music:  g = musicGroup;  break;
    case Bus::SFX:    g = sfxGroup;    break;
    }
    if (g) g->setVolume(volume);
}

float SoundManager::GetVolume(Bus bus) const
{
    float v = 0.f;
    FMOD::ChannelGroup* g = nullptr;
    switch (bus) {
    case Bus::Master: g = masterGroup; break;
    case Bus::Music:  g = musicGroup;  break;
    case Bus::SFX:    g = sfxGroup;    break;
    }
    if (g) g->getVolume(&v);
    return v;
}
