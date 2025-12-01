#include "client_audioManager.h"
#include <SDL.h>
#include <iostream>
#include <stdexcept>

AudioManager::AudioManager() 
    : currentTrack_(MUSIC_LOBBY)
    , initialized_(false)
    , musicVolume_(70)
    , soundVolume_(80) {
    
    // Inicializar SDL_mixer
    if (Mix_OpenAudio(AUDIO_FREQUENCY, MIX_DEFAULT_FORMAT, AUDIO_CHANNELS, AUDIO_CHUNKSIZE) < 0) {
        std::cerr << "[AudioManager] Error al inicializar SDL_mixer: " << Mix_GetError() << std::endl;
        throw std::runtime_error("No se pudo inicializar SDL_mixer");
    }

    // Establecer número de canales para efectos de sonido
    Mix_AllocateChannels(MAX_SOUND_CHANNELS);

    // Establecer volúmenes iniciales
    Mix_VolumeMusic(static_cast<int>(musicVolume_ * MIX_MAX_VOLUME / 100));
    Mix_Volume(-1, static_cast<int>(soundVolume_ * MIX_MAX_VOLUME / 100));

    initialized_ = true;
    std::cout << "[AudioManager] Sistema de audio inicializado correctamente" << std::endl;
}

AudioManager::~AudioManager() {
    if (!initialized_) return;

    // Detener toda la música y sonidos
    Mix_HaltMusic();
    Mix_HaltChannel(-1);

    // Liberar todas las músicas
    for (auto& pair : musicTracks_) {
        if (pair.second) {
            Mix_FreeMusic(pair.second);
        }
    }
    musicTracks_.clear();

    // Liberar todos los efectos de sonido
    for (auto& pair : soundEffects_) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    soundEffects_.clear();

    // Cerrar SDL_mixer
    Mix_CloseAudio();
    
    std::cout << "[AudioManager] Sistema de audio cerrado" << std::endl;
}

bool AudioManager::loadMusic(MusicTrack track, const std::string& filepath) {
    if (!initialized_) return false;

    // Si ya existe, liberarla primero
    if (musicTracks_.count(track) && musicTracks_[track]) {
        Mix_FreeMusic(musicTracks_[track]);
        musicTracks_.erase(track);
    }

    Mix_Music* music = Mix_LoadMUS(filepath.c_str());
    if (!music) {
        std::cerr << "[AudioManager] Error al cargar música '" << filepath 
                  << "': " << Mix_GetError() << std::endl;
        return false;
    }

    musicTracks_[track] = music;
    std::cout << "[AudioManager] Música cargada: " << filepath << std::endl;
    return true;
}

bool AudioManager::loadSound(SoundEffect effect, const std::string& filepath) {
    if (!initialized_) return false;

    // Si ya existe, liberarlo primero
    if (soundEffects_.count(effect) && soundEffects_[effect]) {
        Mix_FreeChunk(soundEffects_[effect]);
        soundEffects_.erase(effect);
    }

    Mix_Chunk* chunk = Mix_LoadWAV(filepath.c_str());
    if (!chunk) {
        std::cerr << "[AudioManager] Error al cargar sonido '" << filepath 
                  << "': " << Mix_GetError() << std::endl;
        return false;
    }

    soundEffects_[effect] = chunk;
    std::cout << "[AudioManager] Sonido cargado: " << filepath << std::endl;
    return true;
}

void AudioManager::playMusic(MusicTrack track, int fadeInMs) {
    if (!initialized_) return;

    if (!musicTracks_.count(track) || !musicTracks_[track]) {
        std::cerr << "[AudioManager] Música no cargada para track ID: " << track << std::endl;
        return;
    }

    // Si ya está sonando esta misma pista, no hacer nada
    if (Mix_PlayingMusic() && currentTrack_ == track) {
        return;
    }

    // Detener música actual si hay alguna
    if (Mix_PlayingMusic()) {
        if (fadeInMs > 0) {
            Mix_FadeOutMusic(fadeInMs / 2); // Fade out más rápido
            SDL_Delay(fadeInMs / 2);
        } else {
            Mix_HaltMusic();
        }
    }

    // Reproducir nueva música con fade in
    if (fadeInMs > 0) {
        if (Mix_FadeInMusic(musicTracks_[track], -1, fadeInMs) == -1) {
            std::cerr << "[AudioManager] Error al reproducir música: " << Mix_GetError() << std::endl;
        }
    } else {
        if (Mix_PlayMusic(musicTracks_[track], -1) == -1) {
            std::cerr << "[AudioManager] Error al reproducir música: " << Mix_GetError() << std::endl;
        }
    }

    currentTrack_ = track;
    std::cout << "[AudioManager] Reproduciendo música track: " << track << std::endl;
}

void AudioManager::stopMusic(int fadeOutMs) {
    if (!initialized_) return;

    if (!Mix_PlayingMusic()) return;

    if (fadeOutMs > 0) {
        Mix_FadeOutMusic(fadeOutMs);
    } else {
        Mix_HaltMusic();
    }

    std::cout << "[AudioManager] Música detenida" << std::endl;
}

void AudioManager::pauseMusic() {
    if (!initialized_) return;
    
    if (Mix_PlayingMusic() && !Mix_PausedMusic()) {
        Mix_PauseMusic();
        std::cout << "[AudioManager] Música pausada" << std::endl;
    }
}

void AudioManager::resumeMusic() {
    if (!initialized_) return;
    
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
        std::cout << "[AudioManager] Música reanudada" << std::endl;
    }
}

int AudioManager::playSound(SoundEffect effect, int loops) {
    if (!initialized_) return -1;

    if (!soundEffects_.count(effect) || !soundEffects_[effect]) {
        std::cerr << "[AudioManager] Sonido no cargado para effect ID: " << effect << std::endl;
        return -1;
    }

    // Reproducir en el primer canal disponible
    int channel = Mix_PlayChannel(-1, soundEffects_[effect], loops);
    
    if (channel == -1) {
        std::cerr << "[AudioManager] Error al reproducir sonido: " << Mix_GetError() << std::endl;
    }

    return channel;
}

void AudioManager::stopSound(int channel) {
    if (!initialized_) return;
    Mix_HaltChannel(channel);
}

void AudioManager::setMusicVolume(int volume) {
    if (!initialized_) return;
    
    musicVolume_ = std::max(0, std::min(100, volume));
    Mix_VolumeMusic(static_cast<int>(musicVolume_ * MIX_MAX_VOLUME / 100));
    
    std::cout << "[AudioManager] Volumen de música: " << musicVolume_ << "%" << std::endl;
}

void AudioManager::setSoundVolume(int volume) {
    if (!initialized_) return;
    
    soundVolume_ = std::max(0, std::min(100, volume));
    Mix_Volume(-1, static_cast<int>(soundVolume_ * MIX_MAX_VOLUME / 100));
    
    std::cout << "[AudioManager] Volumen de efectos: " << soundVolume_ << "%" << std::endl;
}

bool AudioManager::isMusicPlaying() const {
    if (!initialized_) return false;
    return Mix_PlayingMusic() != 0;
}
