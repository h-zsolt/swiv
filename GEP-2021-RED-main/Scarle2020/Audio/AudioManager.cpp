#include "pch.h"
#include "AudioManager.hpp"

/** @brief initilises the audio engine by creating a DirectX::AudioEngine unique_ptr.
*
*	used to initilise the audio engine and make sure it was loaded correctly.
*
*	@return returns true if loaded correctly and false if not.
*/
bool AudioManager::init()
{
	m_audEngine = std::make_unique<DirectX::AudioEngine>();

	{
		auto test = m_audEngine->IsAudioDevicePresent();
	}

	{
		// auto test = m_audEngine->AllocateVoice();
	}

	if (m_audEngine != nullptr)
	{
		return true;
	}
	return false;
}

/**	@brief returns the audio engine ptr.
*
*	@return returns a DirectX::AudioEngine*.
*/
DirectX::AudioEngine* AudioManager::getEngine()
{
	return m_audEngine.get();
}

/** @brief used to play a sound effect.
*
*	pass through a sound effect ptr with various overloads to control how it is played.
*
*	@param instance the ptr to the sound effect instance.
*	@return returns true if the sound is played.
*/
bool AudioManager::playSound(DirectX::SoundEffectInstance* instance)
{
	if (instance != nullptr)
	{
		if (instance->GetState() != DirectX::PLAYING)
		{
			instance->Play();
			return true;
		}
		return false;
	}
	return false;
}

/** @brief used to play a sound effect.
*
*	pass through a sound effect ptr with various overloads to control how it is played.
*
*	@param instance the ptr to the sound effect instance.
*	@param loop a boolean for whether to loop the effect.
*	@return returns true if the sound is played.
*/
bool AudioManager::playSound(DirectX::SoundEffectInstance* instance, bool loop)
{
	if (instance != nullptr)
	{
		if (instance->GetState() != DirectX::PLAYING)
		{
			instance->Play(loop);
			return true;
		}
		return false;
	}
	return false;
}

/** @brief used to play a sound effect.
*
*	pass through a sound effect ptr with various overloads to control how it is played.
*
*	@param instance the ptr to the sound effect instance.
*	@param volume a float for the volume of the effect 0.0 - 1.0.
*	@return returns true if the sound is played.
*/
bool AudioManager::playSound(DirectX::SoundEffectInstance* instance, float volume)
{
	if (instance != nullptr)
	{
		if (instance->GetState() != DirectX::PLAYING)
		{
			instance->SetVolume(volume);
			instance->Play();
			return true;
		}
		return false;
	}
	return false;
}

/** @brief used to play a sound effect.
*
*	pass through a sound effect ptr with various overloads to control how it is played.
*
*	@param instance the ptr to the sound effect instance.
*	@param loop a boolean for whether to loop the effect.
*	@param volume a float for the volume of the effect 0.0 - 1.0.
*	@return returns true if the sound is played.
*/
bool AudioManager::playSound(DirectX::SoundEffectInstance* instance, bool loop, float volume)
{
	if (instance != nullptr)
	{
		if (instance->GetState() != DirectX::PLAYING)
		{
			instance->SetVolume(volume);
			instance->Play(loop);
			return true;
		}
		return false;
	}
	return false;
}

/** @brief used to stop a sound effect from playing.
*
*	by passing through a sound effect instance ptr and a bool the supplied sound effect
*	can be either told to stop playing once it completes or stopped immediately.
*
*	@param instance the ptr to the DirectX::SoundEffectInstance to control.
*	@param immediate the bool value of whether to stop the instance from playing immediately or not.
*	@return returns true if the sound was stopped correctly.
*/
bool AudioManager::stopSound(DirectX::SoundEffectInstance* instance, bool immediate)
{
	if (instance != nullptr)
	{
		if (instance->GetState() == DirectX::PLAYING)
		{
			instance->Stop(immediate);
			return true;
		}
	}
	return false;
}

/** @brief loads a sound effect from a file.
*
*	function will convert the file path string into a wstring
*	which can be read by DirectX::SoundEffect
*	if the provided file path has already been used the index to the sound
*	which has already been loaded will be returned instead of loading it again.
*
*	@param file_path the path to the file to be loaded.
*	@return an int index to the location in a sounds array of the loaded sound, -1 = error.
*/
int AudioManager::loadSoundEffect(const std::string& file_path)
{
	std::unique_ptr<DirectX::SoundEffect> sound_effect;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring wFilename = converter.from_bytes(file_path.c_str());
	if (!m_files_loaded.empty())
	{
		for (auto& file : m_files_loaded)
		{
			if (file.first == file_path)
			{
				return file.second;
			}
		}
	}
	try
	{
		if (sound_effect = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), wFilename.c_str()))
		{
			// file correctly loaded
			m_sounds.emplace_back(std::move(sound_effect));
			m_files_loaded.emplace_back(std::make_pair(file_path, (static_cast<uint32_t>(m_sounds.size() - 1))));
			return m_sounds.size() - 1;
		}
	}
	catch (...)
	{
		return -1;
	}
	return -1;
}

/** @brief creates an instance of a sound effect.
*
*	by passing through the index to a sound already
*	loaded in the sounds vector a new DirectX::SoundEffectInstance is created
*	and returned.
*
*	@param index the index of the array where the sound is located.
*	@return a unique_ptr of the instance.
*/
DirectX::SoundEffectInstance* AudioManager::createEffectInstance(uint32_t index)
{
	if (index <= m_sounds.size())
	{
		std::unique_ptr<DirectX::SoundEffectInstance> effect = m_sounds[index]->CreateInstance();
		m_effects.emplace_back(std::move(effect));
		return m_effects.back().get();
	}
	return nullptr;
}

/** @brief modifies the reverb setting of the audio engine.
*
*	Pass through a DirectX::AUDIO_ENGINE_REVERB enum (see Audio.h Ln 146) to change the reverb.
*
*	@param reverb the enum to set the reverb.
*	@return void.
*/
void AudioManager::setEngineReverb(DirectX::AUDIO_ENGINE_REVERB reverb)
{
	m_audEngine->SetReverb(reverb);
}

/** @brief Stops all effect instances from playing.
*
*	Can be used instead of stopSound() to stop all effects instead of just one.
*
*	@return void
*/
void AudioManager::stopAll()
{
	for (auto& effect : m_effects)
	{
		if (effect != nullptr)
		{
			if (effect->GetState() == DirectX::PLAYING)
			{
				effect->Stop(true);
			}
		}
	}
}

/** @brief frees any instances that have stopped playing from memory
 *
 *
 * @return void
 */
void AudioManager::clearInstances()
{
	if (m_effects.size() >= 15) // just an arbitrary size to check so it's not clearing it all the time
	{
		int i = m_effects.size() - 1;
		while (i >= 0)
		{
			if (m_effects.at(i).get()->GetState() == 0)
			{
				m_effects.erase(m_effects.begin() + i);
			}

			i--;
		}
	}
}
