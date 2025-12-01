#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL_mixer.h>
#include <string>
#include <unordered_map>


class AudioManager {
public:
    // IDs para las distintas músicas
    enum MusicTrack {
        MUSIC_LOBBY = 0,
        MUSIC_GAMEPLAY = 1,
    };

    // IDs para los efectos de sonido
    enum SoundEffect {
        SFX_ACCELERATION = 0,
        SFX_BRAKE = 1,
        SFX_COLLISION_CAR = 2,
        SFX_COLLISION_BUILDING = 3,
    };

    /**
     * @brief Constructor - Inicializa SDL_mixer
     * @throws std::runtime_error si no se puede inicializar SDL_mixer
     */
    AudioManager();

    /**
     * @brief Destructor - Libera todos los recursos de audio
     */
    ~AudioManager();

    // Prevenir copia
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    /**
     * @brief Carga un archivo de música
     * @param track ID de la pista musical
     * @param filepath Ruta al archivo de música (MP3, OGG, WAV)
     * @return true si se cargó correctamente
     */
    bool loadMusic(MusicTrack track, const std::string& filepath);

    /**
     * @brief Carga un efecto de sonido
     * @param effect ID del efecto de sonido
     * @param filepath Ruta al archivo de sonido (WAV)
     * @return true si se cargó correctamente
     */
    bool loadSound(SoundEffect effect, const std::string& filepath);

    /**
     * @brief Reproduce una pista musical en loop
     * @param track ID de la pista a reproducir
     * @param fadeInMs Milisegundos de fade in (0 = sin fade)
     */
    void playMusic(MusicTrack track, int fadeInMs = 1000);

    /**
     * @brief Detiene la música actual
     * @param fadeOutMs Milisegundos de fade out (0 = detener inmediatamente)
     */
    void stopMusic(int fadeOutMs = 1000);

    /**
     * @brief Pausa la música actual
     */
    void pauseMusic();

    /**
     * @brief Resume la música pausada
     */
    void resumeMusic();

    /**
     * @brief Reproduce un efecto de sonido
     * @param effect ID del efecto a reproducir
     * @param loops Número de repeticiones (-1 = infinito, 0 = una vez)
     * @return Canal en el que se reproduce (para detenerlo después si es necesario)
     */
    int playSound(SoundEffect effect, int loops = 0);

    /**
     * @brief Detiene un efecto de sonido en un canal específico
     * @param channel Canal a detener (-1 = todos los canales)
     */
    void stopSound(int channel = -1);

    /**
     * @brief Establece el volumen de la música
     * @param volume Volumen de 0 a 100
     */
    void setMusicVolume(int volume);

    /**
     * @brief Establece el volumen de los efectos de sonido
     * @param volume Volumen de 0 a 100
     */
    void setSoundVolume(int volume);

    /**
     * @brief Verifica si la música está reproduciéndose
     * @return true si hay música sonando
     */
    bool isMusicPlaying() const;

private:
    std::unordered_map<MusicTrack, Mix_Music*> musicTracks_;
    std::unordered_map<SoundEffect, Mix_Chunk*> soundEffects_;
    
    MusicTrack currentTrack_;
    bool initialized_;
    
    int musicVolume_;   // 0-100
    int soundVolume_;   // 0-100

    // Constantes para SDL_mixer
    static constexpr int AUDIO_FREQUENCY = 44100;
    static constexpr int AUDIO_CHANNELS = 2;
    static constexpr int AUDIO_CHUNKSIZE = 2048;
    static constexpr int MAX_SOUND_CHANNELS = 16;
};

#endif // AUDIO_MANAGER_H
