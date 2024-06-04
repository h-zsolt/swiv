#include "GameRenderer.h"
#include <Components/SpriteComponent.h>
#include <Components/HUDImageComponent.h>
#include <Components/TextRenderComponent.h>
#include <ECS.h>
#include <Systems/SpriteRendererSystem.h>
#include <Systems/TextRendererSystem.h>
#include <Systems/HUDImageRendererSystem.h>
#include <Systems/HUDTextRendererSystem.h>
#include <DDSTextureLoader.h>
#include <Helper.h>
#include <Components/TransformComponent.h>
#include <Systems/SpriteAnimRendererSystem.h>

GameRenderer::GameRenderer()
	: m_sprite_renderer_system(nullptr),
	m_text_renderer_system(nullptr),
	m_hud_image_renderer_system(nullptr),
	m_hud_text_renderer_system(nullptr),
	m_sprite_anim_renderer_system(nullptr)
{
}

void GameRenderer::init(ECS::ECS& ecs, ID3D11Device* device, ID3D11DeviceContext* device_context)
{
	// Register and initialise game renderer systems.
	m_sprite_renderer_system = ecs.registerSystem<SpriteRendererSystem>();
	m_sprite_renderer_system->init(ecs);

	m_text_renderer_system = ecs.registerSystem<TextRendererSystem>();
	m_text_renderer_system->init(ecs);

	m_hud_image_renderer_system = ecs.registerSystem<HUDImageRendererSystem>();
	m_hud_image_renderer_system->init(ecs);

	m_hud_text_renderer_system = ecs.registerSystem<HUDTextRendererSystem>();
	m_hud_text_renderer_system->init(ecs);

	m_sprite_anim_renderer_system = ecs.registerSystem<SpriteAnimRendererSystem>();
	m_sprite_anim_renderer_system->init(ecs);

	// Initialise internals.
	for (auto& batch : m_sprite_batches)
	{
		batch = std::make_unique<DirectX::SpriteBatch>(device_context);
	}
	m_states = std::make_unique<DirectX::CommonStates>(device);
	m_fonts.reserve(M_MAX_FONTS);
	m_textures.reserve(M_MAX_TEXTURES);
}

void GameRenderer::tick(float dt, ECS::ECS& ecs)
{
	m_sprite_anim_renderer_system->tick(dt, ecs);
}

size_t GameRenderer::loadTexture(ID3D11Device* device, const std::string& filepath)
{
	assert(m_textures.size() < M_MAX_TEXTURES && "Maximum number of supported textures have already been loaded.");

	auto textures_begin_it = m_texture_filepaths.begin();
	auto textures_end_it = m_texture_filepaths.end();
	auto find_it = std::find(textures_begin_it, textures_end_it, filepath);
	if (find_it != textures_end_it)
	{
		// Asset already exists.
		return static_cast<int>(find_it - textures_begin_it);
	}

	auto size = m_textures.size();
	m_textures.emplace_back(nullptr);
	HRESULT hr = DirectX::CreateDDSTextureFromFile(device, Helper::charToWChar(filepath.c_str()), nullptr, &m_textures[size]);
	assert(hr == S_OK);
	m_texture_filepaths.emplace_back(filepath);
	return size;
}

void GameRenderer::loadTexture(ID3D11Device* device, const std::string& filepath, SpriteComponent& sprite_component)
{
	setupSpriteComponent(loadTexture(device, filepath), sprite_component);
}

void GameRenderer::setupSpriteComponent(size_t texture_id, SpriteComponent& sprite_component)
{
	sprite_component.m_texture_id = static_cast<int>(texture_id);
	auto desc = getTexture2DDesc(texture_id);
	sprite_component.m_source_rect.top = 0;
	sprite_component.m_source_rect.right = desc.Width;
	sprite_component.m_source_rect.bottom = desc.Height;
	sprite_component.m_source_rect.left = 0;
	sprite_component.m_origin = calculateSpriteOrigin(desc);
}

void GameRenderer::setupHUDImageComponent(size_t texture_id, HUDImageComponent& hud_image_component)
{
	hud_image_component.m_texture_id = static_cast<int>(texture_id);
	hud_image_component.m_origin = calculateSpriteOrigin(getTexture2DDesc(texture_id));
}

size_t GameRenderer::loadFont(ID3D11Device* device, const std::string& filepath)
{
	// Does not check if the font has already been loaded. This would require storing the filepath.
	assert(m_fonts.size() < M_MAX_FONTS && "Maximum number of supported fonts have already been loaded.");

	auto font_begin_it = m_font_filepaths.begin();
	auto font_end_it = m_font_filepaths.end();
	auto find_it = std::find(font_begin_it, font_end_it, filepath);
	if (find_it != font_end_it)
	{
		// Asset already exists.
		return static_cast<int>(find_it - font_begin_it);
	}

	auto size = m_fonts.size();
	m_fonts.emplace_back(std::make_unique<DirectX::SpriteFont>(device, Helper::charToWChar(filepath.c_str())));
	m_font_filepaths.push_back(filepath);
	return size;
}

void GameRenderer::loadFont(ID3D11Device* device, const std::string& filepath, TextRenderComponent& text_render_component)
{
	text_render_component.m_font_id = loadFont(device, filepath);
}

void GameRenderer::render(ECS::ECS& ecs, ID3D11DeviceContext* device_context)
{
	// Game world pass.
	m_sprite_batches[0]->Begin(
		DirectX::SpriteSortMode::SpriteSortMode_BackToFront,
		m_states->NonPremultiplied(),
		m_states->PointWrap(),
		m_states->DepthDefault(),
		m_states->CullCounterClockwise(),
		nullptr,
		m_game_camera.getMatrix());
	{
		auto* sprite_batch = m_sprite_batches[0].get();
		m_sprite_renderer_system->render(ecs, sprite_batch, m_textures, m_textures.size());
		m_text_renderer_system->render(ecs, sprite_batch, m_fonts, m_fonts.size());
		m_sprite_anim_renderer_system->render(ecs, sprite_batch, m_textures, m_textures.size());
	}
	m_sprite_batches[0]->End();

	// Hud pass.
	m_sprite_batches[1]->Begin(
		DirectX::SpriteSortMode::SpriteSortMode_BackToFront,
		m_states->NonPremultiplied(),
		m_states->LinearWrap(),
		m_states->DepthDefault(),
		m_states->CullCounterClockwise(),
		nullptr,
		m_hud_camera.getMatrix());
	{
		auto* sprite_batch = m_sprite_batches[1].get();
		m_hud_image_renderer_system->render(ecs, sprite_batch, m_textures, m_textures.size());
		m_hud_text_renderer_system->render(ecs, sprite_batch, m_fonts, m_fonts.size());
	}
	m_sprite_batches[1]->End();
}

void GameRenderer::addGameCameraOffset(const DirectX::SimpleMath::Vector3& offset)
{
	m_game_camera.addOffset(offset);
}

void GameRenderer::setGameCameraPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_game_camera.setPosition(position);
}

const DirectX::SimpleMath::Vector3& GameRenderer::getGameCameraPosition() const
{
	return m_game_camera.getPosition();
}

DirectX::SimpleMath::Vector2 GameRenderer::getGameCameraPosition2D() const
{
	const auto& cam_pos = m_game_camera.getPosition();
	return { cam_pos.x, cam_pos.y };
}

DirectX::SimpleMath::Vector2 GameRenderer::calculateSpriteExtent(const TransformComponent& parent_transform, const SpriteComponent& sprite)
{
	return DirectX::SimpleMath::Vector2(
		(DirectX::SimpleMath::Vector2(static_cast<float>(sprite.m_source_rect.right - sprite.m_source_rect.left), static_cast<float>(sprite.m_source_rect.bottom - sprite.m_source_rect.top))
			* parent_transform.m_scale * sprite.m_relative_scale) / 2.f);
}

DirectX::SimpleMath::Vector2 GameRenderer::calculateSpriteOrigin(const D3D11_TEXTURE2D_DESC& desc)
{
	// Calculate the m_origin.
	return 0.5f * DirectX::SimpleMath::Vector2(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
}

D3D11_TEXTURE2D_DESC GameRenderer::getTexture2DDesc(size_t texture_id)
{
	assert(texture_id < m_textures.size() && "Attempting to calculate sprite m_origin on an invalid texture _id.");

	ID3D11Resource* p_resource;
	D3D11_TEXTURE2D_DESC desc;
	m_textures[texture_id]->GetResource(&p_resource);
	((ID3D11Texture2D*)p_resource)->GetDesc(&desc);
	return desc;
}
