#pragma once
#include <Audio.h>
#include <vector>
#include <string>
#include <codecvt>
#include "ECS.h"

class AudioManager
{
public:
	AudioManager() = default;
	~AudioManager() = default;

	static bool init();
	static DirectX::AudioEngine* getEngine();

	static bool playSound(DirectX::SoundEffectInstance* instance);
	static bool playSound(DirectX::SoundEffectInstance* instance, bool loop);
	static bool playSound(DirectX::SoundEffectInstance* instance, float volume);
	static bool playSound(DirectX::SoundEffectInstance* instance, bool loop, float volume);

	static bool stopSound(DirectX::SoundEffectInstance* instance, bool immediate);
	static void stopAll();
	static int loadSoundEffect(const std::string& file_path);

	static void setEngineReverb(DirectX::AUDIO_ENGINE_REVERB reverb);
	static DirectX::SoundEffectInstance* createEffectInstance(uint32_t index);
	static void clearInstances();
	static bool isPlaying(const char* file);


private:
	static inline std::unique_ptr<DirectX::AudioEngine> m_audEngine = std::unique_ptr<DirectX::AudioEngine>();
	// a vector of all sounds loaded into the manager
	static inline std::vector<std::unique_ptr<DirectX::SoundEffect>> m_sounds = std::vector<std::unique_ptr<
		DirectX::SoundEffect>>();
	// a vector of the file name and its index in the sounds vector 
	static inline std::vector<std::pair<std::string, uint32_t>> m_files_loaded = std::vector<std::pair<
		std::string, uint32_t>>();
	// a vector of all effect instances generated
	static inline std::vector<std::unique_ptr<DirectX::SoundEffectInstance >> m_effects = std::vector<std::unique_ptr<DirectX::SoundEffectInstance>>();
};
