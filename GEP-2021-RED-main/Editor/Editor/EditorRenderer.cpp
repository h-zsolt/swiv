#include <Systems/SpriteRendererSystem.h>
#include <Systems/SpriteAnimRendererSystem.h>
#include <ECS.h>
#include <Components/SpriteComponent.h>
#include <Components/SpriteAnimComponent.h>
#include <Helper.h>
#include <DDSTextureLoader.h>

#include "EditorRenderer.h"
#include "stb_image.h"
#include "Assets/Texture.h"

void EditorRenderer::init(ECS::ECS& ecs, ID3D11Device* device, ID3D11DeviceContext* device_context)
{
	// Register and initialise game renderer systems.
	m_sprite_renderer_system = ecs.registerSystem<SpriteRendererSystem>();
	m_sprite_renderer_system->init(ecs);
	m_sprite_anim_renderer_system = ecs.registerSystem<SpriteAnimRendererSystem>();
	m_sprite_anim_renderer_system->init(ecs);

	// Initialise internals.
	m_sprite_batch = std::make_unique<DirectX::SpriteBatch>(device_context);
	m_states = std::make_unique<DirectX::CommonStates>(device);
}

void EditorRenderer::tick(float dt, ECS::ECS& ecs)
{
	m_sprite_anim_renderer_system->tick(dt, ecs);
}

size_t EditorRenderer::loadTexture(ID3D11Device* device, ID3D11DeviceContext* device_context, const Texture& texture)
{
	auto width = static_cast<UINT>(texture.getWidth());
	auto height = static_cast<UINT>(texture.getHeight());
	auto* data = reinterpret_cast<const void*>(texture.getPixels());

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = data;
	initData.SysMemPitch = width * 4;
	initData.SysMemSlicePitch = width * height * 4;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;

	auto size = m_srvs.size();
	m_srvs.emplace_back(nullptr);
	m_texture_filepaths.emplace_back(texture.getFilepath());
	ID3D11Texture2D* d3d11_texture = nullptr;
	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &d3d11_texture);
	assert(hr == S_OK && "Failed to create texture 2d.");
	hr = device->CreateShaderResourceView(d3d11_texture, &srvDesc, &m_srvs[size]);
	assert(hr == S_OK && "Failed to create srv.");
	device_context->UpdateSubresource(d3d11_texture, 0, 0, data, initData.SysMemPitch, initData.SysMemSlicePitch);
	device_context->GenerateMips(m_srvs[size].Get());

	return size;
}

size_t EditorRenderer::loadSerializedTexture(ID3D11Device* device, const std::string& filepath, const std::string& level_open_path)
{
	// Replace first 'Assets' with full path.
	std::string file = filepath;
	std::string str_to_remove = "Assets\\";
	std::string::size_type i = filepath.find(str_to_remove);
	if (i != std::string::npos)
	{
		file.erase(i, str_to_remove.size());
	}
	std::string path = level_open_path;
	size_t filename_start = level_open_path.find_last_of("\\") + 1;
	if (filename_start != std::string::npos)
	{
		path.erase(filename_start, level_open_path.size() - filename_start);
	}

	auto load_path = path + file;

	const auto& textures = getTextureFilepaths();
	auto textures_begin_it = textures.begin();
	auto textures_end_it = textures.end();
	auto find_it = std::find(textures_begin_it, textures_end_it, load_path);
	if (find_it != textures_end_it)
	{	
		// Asset already exists.
		return static_cast<size_t>(find_it - textures_begin_it);
	}
	else
	{
		auto size = m_srvs.size();
		m_srvs.emplace_back(nullptr);
		m_texture_filepaths.emplace_back(load_path);
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, Helper::charToWChar(load_path.c_str()), nullptr, &m_srvs[size]);
		assert(hr == S_OK);
		return size;
	}
}

void EditorRenderer::loadSerializedTexture(ID3D11Device* device, const std::string& filepath, const std::string& level_open_path, SpriteComponent& sprite_component)
{
	setupSpriteComponent(static_cast<int>(loadSerializedTexture(device, filepath, level_open_path)), sprite_component);
}

ID3D11ShaderResourceView* EditorRenderer::getTextureSRV(int texture_id) const
{
	assert(texture_id >= 0 && "Attempting to get a texture SRV with an invalid id.");
	assert(static_cast<size_t>(texture_id) < m_srvs.size() && "Attempting to get a texture SRV with an invalid id.");
	return m_srvs[static_cast<size_t>(texture_id)].Get();
}

const std::string& EditorRenderer::getTextureFilepath(int texture_id) const
{
	assert(texture_id >= 0 && "Invalid texture id when getting texture filepath.");
	assert(static_cast<size_t>(texture_id) < m_texture_filepaths.size() && "Invalid texture id when getting texture filepath.");
	return m_texture_filepaths[static_cast<size_t>(texture_id)];
}

void EditorRenderer::setupSpriteComponent(int texture_id, SpriteComponent& sprite_component)
{
	assert(texture_id >= 0 && "Attempting to setup sprite component with invalid texture id.");
	sprite_component.m_texture_id = texture_id;
	auto desc = getTexture2DDesc(static_cast<size_t>(texture_id));
	sprite_component.m_origin = calculateSpriteOrigin(desc);
	sprite_component.m_source_rect.top = 0;
	sprite_component.m_source_rect.bottom = desc.Height;
	sprite_component.m_source_rect.left = 0;
	sprite_component.m_source_rect.right = desc.Width;
}

void EditorRenderer::render(ECS::ECS& ecs, ID3D11DeviceContext* device_context)
{
	m_sprite_batch->Begin(
		DirectX::SpriteSortMode::SpriteSortMode_BackToFront,
		m_states->NonPremultiplied(),
		m_states->PointWrap(),
		m_states->DepthDefault(),
		m_states->CullCounterClockwise(),
		nullptr,
		m_editor_camera.getMatrix());
	{
		auto* sprite_batch = m_sprite_batch.get();
		m_sprite_renderer_system->render(ecs, sprite_batch, m_srvs, m_srvs.size());
		m_sprite_anim_renderer_system->render(ecs, sprite_batch, m_srvs, m_srvs.size());
	}
	m_sprite_batch->End();
}

void EditorRenderer::addCameraOffset(const DirectX::SimpleMath::Vector3& offset)
{
	m_editor_camera.addOffset(offset);
}

void EditorRenderer::addCameraZoomDelta(float delta)
{
	m_editor_camera.addZoomDelta(delta);
}

void EditorRenderer::resetCameraZoom()
{
	m_editor_camera.resetZoom();
}

void EditorRenderer::resetCameraPosition()
{
	m_editor_camera.setPosition({ 0.f, 0.f, 0.f });
}

const std::vector<std::string>& EditorRenderer::getTextureFilepaths() const
{
	return m_texture_filepaths;
}

DirectX::SimpleMath::Vector2 EditorRenderer::calculateSpriteOrigin(const D3D11_TEXTURE2D_DESC& desc)
{
	return 0.5f * DirectX::SimpleMath::Vector2(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
}

D3D11_TEXTURE2D_DESC EditorRenderer::getTexture2DDesc(size_t texture_id) const
{
	assert(texture_id < m_srvs.size() && "Attempting to calculate sprite origin on an invalid texture _id.");

	ID3D11Resource* p_resource;
	D3D11_TEXTURE2D_DESC desc;
	m_srvs[texture_id]->GetResource(&p_resource);
	((ID3D11Texture2D*)p_resource)->GetDesc(&desc);
	return desc;
}

void EditorRenderer::restartAnim(SpriteAnimComponent& anim)
{
	anim.m_completed = false;
	anim.m_current_frame = 0;
}

DirectX::SimpleMath::Vector2 EditorRenderer::getEditorCameraPosition2D() const
{
	const auto& cam_pos = m_editor_camera.getPosition();
	return DirectX::SimpleMath::Vector2(cam_pos.x, cam_pos.y);
}
