#pragma once

// Includes.
#include <pch.h>
#include <Camera2D.h>
#include "IRenderer.h"

// Forward declarations.
namespace ECS 
{
	class ECS;
}

class SpriteRendererSystem;
class TextRendererSystem;
class HUDImageRendererSystem;
class HUDTextRendererSystem;
class SpriteAnimRendererSystem;

struct SpriteComponent;
struct TextRenderComponent;
struct HUDImageComponent;
struct TransformComponent;

// Class.
class GameRenderer : public IRenderer
{
	static const size_t M_NUM_SPRITE_BATCHES = 2;
	static const size_t M_MAX_FONTS = 4;
	static const size_t M_MAX_TEXTURES = 64;

public:
	GameRenderer();

	/** Registers necessary components and systems with the ecs and creates renderer internals. */
	void init(ECS::ECS& ecs, ID3D11Device* device, ID3D11DeviceContext* device_context);

	/** Ticks necessary sub-systems. */
	void tick(float dt, ECS::ECS& ecs);

	/** Loads a .dds texture from disk. Expects the full filepath to the texture file. Returns the loaded textures unique id. */
	size_t loadTexture(ID3D11Device* device, const std::string& filepath) override;

	/** Loads a .dds texture from disk into the sprite component. The m_origin of sprite is calculated based on the
		dimensions of the texture. Expects the full filepath to the texture file. */
	void loadTexture(ID3D11Device* device, const std::string& filepath, SpriteComponent& sprite_component);

	/** Sets up a sprite component's m_origin and texture id from an already loaded texture. */
	void setupSpriteComponent(size_t texture_id, SpriteComponent& sprite_component) override;

	/** Sets up a HUD image component's m_origin and texture id from an already loaded texture. */
	void setupHUDImageComponent(size_t texture_id, HUDImageComponent& hud_image_component);

	/** Creates a new SpriteFont object with the font at the filepath. Expects a full filepath to a .spritefont file. Returns the loaded 
	    font's unique id. */
	size_t loadFont(ID3D11Device* device, const std::string& filepath);

	/** Creates a new SpriteFont object with the font at the filepath. Expects a full filepath to a .spritefont file. The text render 
	    component's font id is set to the loaded font. */
	void loadFont(ID3D11Device* device, const std::string& filepath, TextRenderComponent& text_render_component);

	/** Renders the sprite and text sub systems. */
	void render(ECS::ECS& ecs, ID3D11DeviceContext* device_context);

	/** Adds the offset to the game camera's position. */
	void addGameCameraOffset(const DirectX::SimpleMath::Vector3& offset);

	/** Sets the position of the game camera. */
	void setGameCameraPosition(const DirectX::SimpleMath::Vector3& position);
	const DirectX::SimpleMath::Vector3& getGameCameraPosition() const;
	DirectX::SimpleMath::Vector2 getGameCameraPosition2D() const override;
	DirectX::SimpleMath::Vector2 calculateSpriteExtent(const TransformComponent& parent_transform, const SpriteComponent& sprite) override;

private:
	/** Returns the centre of the texture as the m_origin. */
	DirectX::SimpleMath::Vector2 calculateSpriteOrigin(const D3D11_TEXTURE2D_DESC& desc);

	/** Returns the texture 2D description of the texture. */
	D3D11_TEXTURE2D_DESC getTexture2DDesc(size_t texture_id);

private:
	// Internals.
	SpriteRendererSystem* m_sprite_renderer_system;
	TextRendererSystem* m_text_renderer_system;
	HUDImageRendererSystem* m_hud_image_renderer_system;
	HUDTextRendererSystem* m_hud_text_renderer_system;
	SpriteAnimRendererSystem* m_sprite_anim_renderer_system;
	std::array<std::unique_ptr<DirectX::SpriteBatch>, M_NUM_SPRITE_BATCHES> m_sprite_batches;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::vector<std::unique_ptr<DirectX::SpriteFont>> m_fonts;
	std::vector<std::string> m_font_filepaths;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textures;
	std::vector<std::string> m_texture_filepaths;
	Camera2D m_game_camera;
	Camera2D m_hud_camera;
};