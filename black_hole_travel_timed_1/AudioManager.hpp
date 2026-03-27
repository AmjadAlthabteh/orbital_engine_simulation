#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>

// Sound effect types
enum class SoundEffect
{
    ENGINE_THRUST,
    ENGINE_STOP,
    LANDING_SUCCESS,
    LANDING_FAILURE,
    TAKEOFF,
    ACHIEVEMENT_UNLOCK,
    SCIENCE_SCAN_START,
    SCIENCE_SCAN_COMPLETE,
    WAYPOINT_CREATE,
    COLLISION_WARNING,
    DAMAGE_TAKEN,
    REPAIR_COMPLETE,
    REFUEL,
    UI_CLICK,
    UI_HOVER,
    COSMIC_EVENT,
    CRITICAL_WARNING,
    PAUSE,
    UNPAUSE
};

// Music tracks
enum class MusicTrack
{
    MAIN_THEME,
    SPACE_AMBIENT,
    INTENSE_FLIGHT,
    NEAR_BLACK_HOLE,
    PLANET_APPROACH,
    VICTORY
};

class AudioManager
{
private:
    // Sound buffers (loaded once, reused)
    std::unordered_map<SoundEffect, sf::SoundBuffer> soundBuffers;

    // Active sound instances
    std::vector<std::unique_ptr<sf::Sound>> activeSounds;

    // Background music
    sf::Music backgroundMusic;
    MusicTrack currentTrack;

    // Volume settings
    float masterVolume;
    float sfxVolume;
    float musicVolume;

    // Audio enabled flags
    bool audioEnabled;
    bool musicEnabled;
    bool sfxEnabled;

    // Cooldown for repeated sounds
    std::unordered_map<SoundEffect, float> soundCooldowns;
    const float DEFAULT_COOLDOWN = 0.1f;

    // Initialize sound buffers (placeholder - would load actual audio files)
    void initializeSounds()
    {
        // In a real implementation, you would load actual audio files here:
        // soundBuffers[SoundEffect::ENGINE_THRUST].loadFromFile("sounds/engine_thrust.wav");
        // For now, we just initialize empty buffers as placeholders

        std::cout << "[AudioManager] Audio system initialized (placeholder mode)\n";
        std::cout << "[AudioManager] To use real sounds, place audio files in 'sounds/' directory\n";

        // Create placeholder entries
        soundBuffers[SoundEffect::ENGINE_THRUST] = sf::SoundBuffer();
        soundBuffers[SoundEffect::ENGINE_STOP] = sf::SoundBuffer();
        soundBuffers[SoundEffect::LANDING_SUCCESS] = sf::SoundBuffer();
        soundBuffers[SoundEffect::LANDING_FAILURE] = sf::SoundBuffer();
        soundBuffers[SoundEffect::TAKEOFF] = sf::SoundBuffer();
        soundBuffers[SoundEffect::ACHIEVEMENT_UNLOCK] = sf::SoundBuffer();
        soundBuffers[SoundEffect::SCIENCE_SCAN_START] = sf::SoundBuffer();
        soundBuffers[SoundEffect::SCIENCE_SCAN_COMPLETE] = sf::SoundBuffer();
        soundBuffers[SoundEffect::WAYPOINT_CREATE] = sf::SoundBuffer();
        soundBuffers[SoundEffect::COLLISION_WARNING] = sf::SoundBuffer();
        soundBuffers[SoundEffect::DAMAGE_TAKEN] = sf::SoundBuffer();
        soundBuffers[SoundEffect::REPAIR_COMPLETE] = sf::SoundBuffer();
        soundBuffers[SoundEffect::REFUEL] = sf::SoundBuffer();
        soundBuffers[SoundEffect::UI_CLICK] = sf::SoundBuffer();
        soundBuffers[SoundEffect::UI_HOVER] = sf::SoundBuffer();
        soundBuffers[SoundEffect::COSMIC_EVENT] = sf::SoundBuffer();
        soundBuffers[SoundEffect::CRITICAL_WARNING] = sf::SoundBuffer();
        soundBuffers[SoundEffect::PAUSE] = sf::SoundBuffer();
        soundBuffers[SoundEffect::UNPAUSE] = sf::SoundBuffer();
    }

public:
    AudioManager()
    {
        masterVolume = 100.0f;
        sfxVolume = 80.0f;
        musicVolume = 60.0f;

        audioEnabled = true;
        musicEnabled = true;
        sfxEnabled = true;

        currentTrack = MusicTrack::SPACE_AMBIENT;

        initializeSounds();
    }

    // Update audio system (cleanup finished sounds, update cooldowns)
    void update(float deltaTime)
    {
        // Remove finished sounds
        activeSounds.erase(
            std::remove_if(activeSounds.begin(), activeSounds.end(),
                [](const std::unique_ptr<sf::Sound>& sound) {
                    return sound->getStatus() == sf::Sound::Stopped;
                }),
            activeSounds.end()
        );

        // Update cooldowns
        for (auto& pair : soundCooldowns)
        {
            if (pair.second > 0.0f)
            {
                pair.second -= deltaTime;
                if (pair.second < 0.0f) pair.second = 0.0f;
            }
        }
    }

    // Play a sound effect
    void playSound(SoundEffect effect, float pitch = 1.0f, bool force = false)
    {
        if (!audioEnabled || !sfxEnabled) return;

        // Check cooldown
        if (!force && soundCooldowns[effect] > 0.0f)
            return;

        // Check if sound buffer exists
        if (soundBuffers.find(effect) == soundBuffers.end())
        {
            std::cerr << "[AudioManager] Sound effect not loaded: " << static_cast<int>(effect) << "\n";
            return;
        }

        // Create new sound instance
        auto sound = std::make_unique<sf::Sound>();
        sound->setBuffer(soundBuffers[effect]);
        sound->setVolume((masterVolume / 100.0f) * sfxVolume);
        sound->setPitch(pitch);
        sound->play();

        activeSounds.push_back(std::move(sound));

        // Set cooldown
        soundCooldowns[effect] = DEFAULT_COOLDOWN;

        // Console feedback (useful during development)
        // std::cout << "[AudioManager] Playing sound: " << getSoundName(effect) << "\n";
    }

    // Play looping sound (for engine thrust)
    void playLoopingSound(SoundEffect effect, float pitch = 1.0f)
    {
        if (!audioEnabled || !sfxEnabled) return;

        // Check if already playing
        for (const auto& sound : activeSounds)
        {
            if (sound->getBuffer() == &soundBuffers[effect] && sound->getLoop())
                return;  // Already looping
        }

        auto sound = std::make_unique<sf::Sound>();
        sound->setBuffer(soundBuffers[effect]);
        sound->setVolume((masterVolume / 100.0f) * sfxVolume);
        sound->setPitch(pitch);
        sound->setLoop(true);
        sound->play();

        activeSounds.push_back(std::move(sound));
    }

    // Stop looping sound
    void stopLoopingSound(SoundEffect effect)
    {
        for (auto& sound : activeSounds)
        {
            if (sound->getBuffer() == &soundBuffers[effect] && sound->getLoop())
            {
                sound->stop();
            }
        }
    }

    // Stop all sounds
    void stopAllSounds()
    {
        for (auto& sound : activeSounds)
        {
            sound->stop();
        }
        activeSounds.clear();
    }

    // Play background music
    void playMusic(MusicTrack track)
    {
        if (!audioEnabled || !musicEnabled) return;

        currentTrack = track;

        // In real implementation, load music file:
        // std::string filename = getMusicFilename(track);
        // if (backgroundMusic.openFromFile(filename))
        // {
        //     backgroundMusic.setVolume((masterVolume / 100.0f) * musicVolume);
        //     backgroundMusic.setLoop(true);
        //     backgroundMusic.play();
        // }

        std::cout << "[AudioManager] Playing music: " << getMusicName(track) << " (placeholder)\n";
    }

    // Stop music
    void stopMusic()
    {
        backgroundMusic.stop();
    }

    // Pause/Resume music
    void pauseMusic()
    {
        if (backgroundMusic.getStatus() == sf::Music::Playing)
            backgroundMusic.pause();
    }

    void resumeMusic()
    {
        if (backgroundMusic.getStatus() == sf::Music::Paused)
            backgroundMusic.play();
    }

    // Volume controls
    void setMasterVolume(float volume)
    {
        masterVolume = std::max(0.0f, std::min(100.0f, volume));
        updateAllVolumes();
    }

    void setSFXVolume(float volume)
    {
        sfxVolume = std::max(0.0f, std::min(100.0f, volume));
        updateAllVolumes();
    }

    void setMusicVolume(float volume)
    {
        musicVolume = std::max(0.0f, std::min(100.0f, volume));
        backgroundMusic.setVolume((masterVolume / 100.0f) * musicVolume);
    }

    void updateAllVolumes()
    {
        // Update active sounds
        for (auto& sound : activeSounds)
        {
            sound->setVolume((masterVolume / 100.0f) * sfxVolume);
        }

        // Update music
        backgroundMusic.setVolume((masterVolume / 100.0f) * musicVolume);
    }

    // Toggle audio
    void toggleAudio() { audioEnabled = !audioEnabled; }
    void toggleMusic() { musicEnabled = !musicEnabled; if (!musicEnabled) stopMusic(); }
    void toggleSFX() { sfxEnabled = !sfxEnabled; if (!sfxEnabled) stopAllSounds(); }

    // Getters
    float getMasterVolume() const { return masterVolume; }
    float getSFXVolume() const { return sfxVolume; }
    float getMusicVolume() const { return musicVolume; }
    bool isAudioEnabled() const { return audioEnabled; }
    bool isMusicEnabled() const { return musicEnabled; }
    bool isSFXEnabled() const { return sfxEnabled; }

    // Helper: Get sound effect name
    std::string getSoundName(SoundEffect effect) const
    {
        switch (effect)
        {
            case SoundEffect::ENGINE_THRUST: return "Engine Thrust";
            case SoundEffect::ENGINE_STOP: return "Engine Stop";
            case SoundEffect::LANDING_SUCCESS: return "Landing Success";
            case SoundEffect::LANDING_FAILURE: return "Landing Failure";
            case SoundEffect::TAKEOFF: return "Takeoff";
            case SoundEffect::ACHIEVEMENT_UNLOCK: return "Achievement Unlock";
            case SoundEffect::SCIENCE_SCAN_START: return "Science Scan Start";
            case SoundEffect::SCIENCE_SCAN_COMPLETE: return "Science Scan Complete";
            case SoundEffect::WAYPOINT_CREATE: return "Waypoint Created";
            case SoundEffect::COLLISION_WARNING: return "Collision Warning";
            case SoundEffect::DAMAGE_TAKEN: return "Damage Taken";
            case SoundEffect::REPAIR_COMPLETE: return "Repair Complete";
            case SoundEffect::REFUEL: return "Refuel";
            case SoundEffect::UI_CLICK: return "UI Click";
            case SoundEffect::UI_HOVER: return "UI Hover";
            case SoundEffect::COSMIC_EVENT: return "Cosmic Event";
            case SoundEffect::CRITICAL_WARNING: return "Critical Warning";
            case SoundEffect::PAUSE: return "Pause";
            case SoundEffect::UNPAUSE: return "Unpause";
            default: return "Unknown";
        }
    }

    // Helper: Get music track name
    std::string getMusicName(MusicTrack track) const
    {
        switch (track)
        {
            case MusicTrack::MAIN_THEME: return "Main Theme";
            case MusicTrack::SPACE_AMBIENT: return "Space Ambient";
            case MusicTrack::INTENSE_FLIGHT: return "Intense Flight";
            case MusicTrack::NEAR_BLACK_HOLE: return "Near Black Hole";
            case MusicTrack::PLANET_APPROACH: return "Planet Approach";
            case MusicTrack::VICTORY: return "Victory";
            default: return "Unknown";
        }
    }

    // Display audio status
    void displayStatus() const
    {
        std::cout << "\n[AudioManager] ========== AUDIO STATUS ==========\n";
        std::cout << "[AudioManager] Audio Enabled: " << (audioEnabled ? "Yes" : "No") << "\n";
        std::cout << "[AudioManager] Music Enabled: " << (musicEnabled ? "Yes" : "No") << "\n";
        std::cout << "[AudioManager] SFX Enabled: " << (sfxEnabled ? "Yes" : "No") << "\n";
        std::cout << "[AudioManager] Master Volume: " << masterVolume << "%\n";
        std::cout << "[AudioManager] Music Volume: " << musicVolume << "%\n";
        std::cout << "[AudioManager] SFX Volume: " << sfxVolume << "%\n";
        std::cout << "[AudioManager] Active Sounds: " << activeSounds.size() << "\n";
        std::cout << "[AudioManager] Current Track: " << getMusicName(currentTrack) << "\n";
        std::cout << "[AudioManager] ========================================\n\n";
    }
};
