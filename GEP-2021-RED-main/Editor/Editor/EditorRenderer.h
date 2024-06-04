#pragma once

#include <pch.h>
#include <Camera2D.h>

namespace ECS { class ECS; }

class SpriteRendererSystem;
class SpriteAnimRendererSystem;
class Texture;

struct SpriteComponent;
struct SpriteAnimComponent;

class EditorRenderer
{
public:
	/** Initialises editor internals. */
	void init(ECS::ECS& ecs, ID3D11Device* device, ID3D11DeviceContext* device_context);

	/** Ticks necessary sub-systems. */
	void tick(float dt, ECS::ECS& ecs);

	/** Creates the shader resource view for the texture and generates a mipmap chain. */
	size_t loadTexture(ID3D11Device* device, ID3D11DeviceContext* device_context, const Texture& texture);

	/** These are used by the level importer and make assumptions for that use. Not to be used by user. */
	size_t loadSerializedTexture(ID3D11Device* device, const std::string& filepath, const std::string& level_open_path);
	void loadSerializedTexture(ID3D11Device* device, const std::string& filepath, const std::string& level_open_path, SpriteComponent& sprite_component);

	/** Returns the shader resource view at the texture id. */
	ID3D11ShaderResourceView* getTextureSRV(int texture_id) const;

	/** Returns the filepath at the texture id. */
	const std::string& getTextureFilepath(int texture_id) const;

	/** Sets up the sprite component's id and origin fields using the texture at the texture id. */
	void setupSpriteComponent(int texture_id, SpriteComponent& sprite_component);

	/** Renders the editor scene. */
	void render(ECS::ECS& ecs, ID3D11DeviceContext* device_context);

	void addCameraOffset(const DirectX::SimpleMath::Vector3& offset);
	void addCameraZoomDelta(float delta);
	void resetCameraZoom();
	void resetCameraPosition();

	const std::vector<std::string>& getTextureFilepaths() const;

	D3D11_TEXTURE2D_DESC getTexture2DDesc(size_t texture_id) const;

	void restartAnim(SpriteAnimComponent& anim);

	DirectX::SimpleMath::Vector2 getEditorCameraPosition2D() const;

private:
	DirectX::SimpleMath::Vector2 calculateSpriteOrigin(const D3D11_TEXTURE2D_DESC& desc);

private:
	// Internals.
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_srvs;
	std::vector<std::string> m_texture_filepaths;
	SpriteRendererSystem* m_sprite_renderer_system;
	SpriteAnimRendererSystem* m_sprite_anim_renderer_system;
	std::unique_ptr<DirectX::SpriteBatch> m_sprite_batch;
	std::unique_ptr<DirectX::CommonStates> m_states;
	Camera2D m_editor_camera;
};