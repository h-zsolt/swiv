#pragma once

#include <pch.h>

class Texture
{
public:
	Texture(unsigned char* pixels, uint32_t width, uint32_t height, uint8_t channels, const std::string& filepath);
	~Texture();

	// Copying textures is disabled.
	Texture(const Texture& other) = delete;
	Texture& operator=(const Texture& other) = delete;

	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;

	unsigned char* getPixels() const;
	int getWidth() const;
	int getHeight() const;
	int getChannels() const;
	const std::string& getFilepath() const;

	bool isValid() const;

private:
	unsigned char* m_pixels;
	int m_width;
	int m_height;
	int m_channels;
	std::string m_filepath;
};