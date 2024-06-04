#include "Texture.h"

Texture::Texture(unsigned char* pixels, uint32_t width, uint32_t height, uint8_t channels, const std::string& filepath)
	: m_pixels(pixels), m_width(width), m_height(height), m_channels(channels), m_filepath(filepath)
{
}

Texture::~Texture()
{
}

Texture::Texture(Texture&& other) noexcept
{
	m_pixels = other.m_pixels;
	other.m_pixels = nullptr;

	m_width = other.m_width;
	other.m_width = 0;

	m_height = other.m_height;
	other.m_height = 0;

	m_channels = other.m_channels;
	other.m_channels = 0;

	m_filepath = std::move(other.m_filepath);
	other.m_filepath.clear();
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	m_pixels = other.m_pixels;
	other.m_pixels = nullptr;

	m_width = other.m_width;
	other.m_width = 0;

	m_height = other.m_height;
	other.m_height = 0;

	m_channels = other.m_channels;
	other.m_channels = 0;

	m_filepath = std::move(other.m_filepath);
	other.m_filepath.clear();
	return *this;
}

unsigned char* Texture::getPixels() const
{
	return m_pixels;
}

int Texture::getWidth() const
{
	return m_width;
}

int Texture::getHeight() const
{
	return m_height;
}

int Texture::getChannels() const
{
	return m_channels;
}

const std::string& Texture::getFilepath() const
{
	return m_filepath;
}

bool Texture::isValid() const
{
	return m_pixels != nullptr; 
}
