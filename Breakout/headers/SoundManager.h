#pragma once
#include "fmod.hpp"
#include <string>
#include <unordered_map>

class SoundManager {
public:
    // Categories for volume control
    enum class Bus { Master, Music, SFX };

    bool Initialize();
    void Shutdown();      // <-- new: release everything
    void Update();        // <-- was UpdateSound()

    // Load any sound/stream by alias
    // stream=true for long music (MP3), false for short SFX (WAV/OGG)
    // loop=true to loop (good for BGM)
    bool Load(const std::string& alias, const std::string& path, bool stream = false, bool loop = false);

    // Play by alias; returns false if alias not loaded
    bool Play(const std::string& alias, bool paused = false);

    // Stop a specific alias (stops all channels currently playing that sound)
    void Stop(const std::string& alias);

    // Stop everything
    void StopAll();

    // Volume 0..1 per bus
    void SetVolume(Bus bus, float volume);
    float GetVolume(Bus bus) const;

private:
    // Convenience error check (returns false on error)
    bool Check(FMOD_RESULT r) { return (r == FMOD_OK); }

    FMOD::System* system = nullptr;
    FMOD_RESULT        result;
    void* extradriverdate = 0; // keep your existing field name

    // Channel groups for mixing
    FMOD::ChannelGroup* masterGroup = nullptr;
    FMOD::ChannelGroup* musicGroup = nullptr;
    FMOD::ChannelGroup* sfxGroup = nullptr;

    struct SoundEntry {
        FMOD::Sound* sound = nullptr;
        bool          isMusic = false;  // choose group at play time
    };

    // alias -> sound
    std::unordered_map<std::string, SoundEntry> sounds;

    // Track live channels to support Stop(alias)
    // (We don’t store all of them; we just iterate in StopAll via groups.)
    // For Stop(alias) we re-play via channel retrieval from system if needed,
    // but in simple games you can omit per-alias channel tracking.
};
