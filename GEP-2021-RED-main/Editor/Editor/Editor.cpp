#include <pch.h>
#include <imgui.h>
#include <Helper.h>
#include <Components/TransformComponent.h>
#include <Components/SpriteComponent.h>
#include <Components/SpriteAnimComponent.h>
#include <Systems/InputSystem.h>
#include <Data/InputData.h>
#include <Data/ButtonData.h>
#include <commdlg.h>
#include <Shlwapi.h>
#include <DirectXTex.h>
#include <Data/EnemySpawnerData.h>
#include <Data/EnvironmentTag.h>
#include <Components/BoundComponent.h>

#include "Editor.h"
#include "stb_image.h"
#include "ThirdParty/yaml-cpp/yaml.h"
#include "Assets/TilemapCreationInfo.h"
#include "Widgets/TitleBar.h"
#include "Widgets/SceneOutliner.h"
#include "Widgets/DetailsPanel.h"
#include "Widgets/CreateTilemapMenu.h"

// Serialization
YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::SimpleMath::Vector2& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::SimpleMath::Color& c)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << c.x << c.y << c.z << c.w << YAML::EndSeq;
	return out;
}

template <>
struct YAML::convert<DirectX::SimpleMath::Vector2>
{
	static Node encode(const DirectX::SimpleMath::Vector2& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}

	static bool decode(const Node& node, DirectX::SimpleMath::Vector2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
		{
			return false;
		}

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		return true;
	}
};

template <>
struct YAML::convert<DirectX::SimpleMath::Color>
{
	static Node encode(const DirectX::SimpleMath::Color& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		return node;
	}

	static bool decode(const Node& node, DirectX::SimpleMath::Color& rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
		{
			return false;
		}

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		rhs.w = node[3].as<float>();
		return true;
	}
};

void Editor::start()
{
	ShowCursor(true);

	auto* p_ecs = ecs();

	// Register components.
	p_ecs->registerComponent<TransformComponent>();
	p_ecs->registerComponent<SpriteComponent>();
	p_ecs->registerComponent<SpriteAnimComponent>();
	p_ecs->registerComponent<InputData>();
	p_ecs->registerComponent<MoveData>();
	p_ecs->registerComponent<ButtonData>();
	p_ecs->registerComponent<EnemySpawnerData>();
	p_ecs->registerComponent<EnvironmentTag>();
	p_ecs->registerComponent<BoundComponent>();

	// Initialize editor renderer.
	m_editor_renderer = std::make_unique<EditorRenderer>();
	m_editor_renderer->init(*ecs(), getDevice(), getDeviceContext());

	// Initialize input system.
	m_input_system = p_ecs->registerSystem<InputSystem>();
	m_input_system->init(*p_ecs, getWindow());

	// Initialize title bar widget.
	m_title_bar = std::make_unique<TitleBar>();
	m_title_bar->registerOnQuitEvent(std::bind(&Editor::quit, this));
	m_title_bar->registerOnEditorSettingsEvent(std::bind(&Editor::openEditorSettings, this));
	m_title_bar->registerOnSaveSceneEvent(std::bind(&Editor::onExportLevel, this));
	m_title_bar->registerOnLoadSceneEvent(std::bind(&Editor::onImportLevel, this));
	m_title_bar->registerOnNewSceneEvent(std::bind(&Editor::clearScene, this));
	m_title_bar->registerOnCreateTilemapEvent(std::bind(&Editor::onCreateTileMapMenu, this));

	// Initialize editor settings widget.
	m_editor_settings = std::make_unique<EditorSettings>();

	// Initialize scene outliner widget.
	m_scene_outliner = std::make_unique<SceneOutliner>(&m_scene_context, &m_tilemaps);
	m_scene_outliner->registerOnEntitySelectedEvent(std::bind(&Editor::onEntitySelected, this, std::placeholders::_1));
	m_scene_outliner->registerOnCreateEntityEvent(std::bind(&Editor::onEntityCreated, this));
	m_scene_outliner->registerOnDestroyEntityEvent(std::bind(&Editor::onEntityDestroyed, this, std::placeholders::_1));
	m_scene_outliner->registerOnDuplicateEntityEvent(std::bind(&Editor::onEntityDuplicated, this, std::placeholders::_1));
	m_scene_outliner->registerOnTilemapSelectedEvent(std::bind(&Editor::onTilemapSelected, this, std::placeholders::_1));

	// Initialize details panel widget.
	m_details_panel = std::make_unique<DetailsPanel>();
	m_details_panel->registerOnImportTextureEvent(std::bind(&Editor::onImportTexture, this));

	// Initialize create tilemap menu widget.
	m_create_tilemap_menu = std::make_unique<CreateTilemapMenu>();
	m_create_tilemap_menu->registerOnCreateEvent(std::bind(&Editor::createTilemap, this, std::placeholders::_1));
	m_create_tilemap_menu->registerOnLoadSpritesheetEvent(std::bind(&Editor::onImportTexture, this));
}

void Editor::updateObjects(GameData* gd)
{
	auto& r_ecs = *ecs();
	m_input_system->readInput(r_ecs);
	m_editor_renderer->tick(gd->m_dt, r_ecs);

	const auto& ms = m_input_system->mouseState();
	const auto& kbs = m_input_system->keyboardState();
	const auto& kbs_tracker = m_input_system->keyboardStateTracker();

	// If an entity is selected and delete is pressed, delete the entity.
	if (kbs_tracker.IsKeyPressed(DirectX::Keyboard::Keys::Delete))
	{
		if (!m_selection_context.first.empty())
		{
			onEntityDestroyed(m_selection_context.first);
		}
		else if (auto* selected_tilemap = m_scene_outliner->getTilemapSelectionContext(); selected_tilemap)
		{
			onTilemapDestroyed(selected_tilemap->getName());
		}
	}

	// Reset camera on 1 key.
	if (kbs_tracker.IsKeyPressed(M_RESET_CAMERA_KEY))
	{
		resetCamera();
	}

	// Generate single left button pressed event if cursor is in level area.
	if (!ImGui::IsAnyItemHovered() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		if (ms.leftButton != m_last_left_button_state)
		{
			if (ms.leftButton)
			{
				pickFromScene(ms.x, ms.y);
			}
		}
	}

	m_last_left_button_state = ms.leftButton;

	if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		// Update editor camera zoom.
		m_editor_renderer->addCameraZoomDelta(ms.scrollWheelValue * gd->m_dt * m_editor_settings->getZoomSensitivity());
	}
	m_input_system->mouse()->ResetScrollWheelValue();

	// Update editor camera position. Hold down right mouse button and use WASD to pan.
	if (ms.rightButton)
	{
		auto camera_speed = static_cast<float>(m_editor_settings->getCameraSpeed());
		if (kbs.A)
		{
			m_editor_renderer->addCameraOffset({ camera_speed * gd->m_dt, 0.f, 0.f });
		}

		if (kbs.D)
		{
			m_editor_renderer->addCameraOffset({ -camera_speed * gd->m_dt, 0.f, 0.f });
		}

		if (kbs.W)
		{
			m_editor_renderer->addCameraOffset({ 0.f, camera_speed * gd->m_dt, 0.f });
		}

		if (kbs.S)
		{
			m_editor_renderer->addCameraOffset({ 0.f, -camera_speed * gd->m_dt, 0.f });
		}
	}
}

void Editor::renderObjects()
{
	m_editor_renderer->render(*ecs(), getDeviceContext());
}

void Editor::onActivated()
{
	ScarleGame::onActivated();
	//To be implemented
}

void Editor::onDeactivated()
{
	ScarleGame::onDeactivated();
	//To be implemented
}

void Editor::onSuspending()
{
	ScarleGame::onSuspending();
	//To be implemented
}

void Editor::onResuming()
{
	ScarleGame::onResuming();
	//To be implemented
}

void Editor::onWindowSizeChanged(int _width, int _height)
{
	ScarleGame::onWindowSizeChanged(_width, _height);
	//To be implemented
}

void Editor::RenderImGui()
{
	// Always render these widgets.
	m_title_bar->titleBar();
	m_scene_outliner->sceneOutliner();
	m_details_panel->detailsPanel(*ecs(), *m_editor_renderer.get());

	// Render the editor settings window if it is shown.
	if (m_editor_settings->isShown())
	{
		m_editor_settings->dialog();
	}

	// Render the create tilemap menu if it is shown.
	if (m_create_tilemap_menu->isShown())
	{
		m_create_tilemap_menu->createTilemapMenu(*m_editor_renderer.get());
	}
}

void Editor::openEditorSettings()
{
	m_editor_settings->show(true);
}

void Editor::onEntityCreated()
{
	createNewEntity();
}

std::pair<std::string, ECS::Entity> Editor::createNewEntity()
{
	// Create an entity with a default name and transform component.
	auto* p_ecs = ecs();
	auto entity = p_ecs->createEntity();
	std::string name = "Entity_" + std::to_string(entity);
	m_scene_context.emplace(name, entity);
	p_ecs->addComponent(entity, TransformComponent());
	return { name, entity };
}

void Editor::onEntitySelected(const std::string& name)
{
	if (name.empty())
	{
		m_selection_context = { "", 9817239 };
		m_details_panel->setSelectionContext(nullptr);
		m_scene_outliner->setSelectionContext(9817239);
		return;
	}

	 //Disallow selecting entities that are a part of a tilemap.
	for (const auto& pair : m_tilemaps)
	{
		if (pair.second.contains({ name, m_scene_context.at(name) }))
		{
			m_selection_context = { "", 9817239 };
			m_details_panel->setSelectionContext(nullptr);
			m_scene_outliner->setSelectionContext(9817239);

			onTilemapSelected(pair.first);

			return;
		}
	}

	m_selection_context = { name, m_scene_context.at(name) };
	m_details_panel->setSelectionContext(&m_selection_context);
	m_scene_outliner->setSelectionContext(m_scene_context.at(name));
	m_scene_outliner->setTilemapSelectionContext(nullptr);
	m_details_panel->setSelectedTilemap(nullptr);
}

void Editor::onEntityDestroyed(const std::string& name)
{
	ecs()->destroyEntity(m_scene_context.at(name));
	m_scene_context.erase(name);

	if (m_selection_context.first == name)
	{
		m_selection_context = {};
		m_details_panel->setSelectionContext(nullptr);
	}
}

void Editor::onExportLevel()
{
	exportLevel();
}

void Editor::onEntityDuplicated(const std::string& name)
{
	const auto& other_entity = m_scene_context.at(name);
	auto new_entity = createNewEntity().second;
	auto* p_ecs = ecs();

	// Duplicate transform component.
	p_ecs->getComponent<TransformComponent>(new_entity) = p_ecs->getComponent<TransformComponent>(other_entity);

	// Duplicate sprite component.
	if (p_ecs->hasComponent<SpriteComponent>(other_entity))
	{
		SpriteComponent sprite = p_ecs->getComponent<SpriteComponent>(other_entity);
		p_ecs->addComponent<SpriteComponent>(new_entity, sprite);
	}

	// Duplicate sprite anim component.
	if (p_ecs->hasComponent<SpriteAnimComponent>(other_entity))
	{
		SpriteAnimComponent sprite_anim = p_ecs->getComponent<SpriteAnimComponent>(other_entity);
		p_ecs->addComponent<SpriteAnimComponent>(new_entity, sprite_anim);
	}

	// Duplicate enemy spawner component.
	if (p_ecs->hasComponent<EnemySpawnerData>(other_entity))
	{
		EnemySpawnerData enemy_spawn = p_ecs->getComponent<EnemySpawnerData>(other_entity);
		p_ecs->addComponent<EnemySpawnerData>(new_entity, enemy_spawn);
	}

	// Duplicate environment tag.
	if (p_ecs->hasComponent<EnvironmentTag>(other_entity))
	{
		p_ecs->addComponent<EnvironmentTag>(new_entity, {});
	}

	// TODO: Duplicate other components.
}

void Editor::onImportLevel()
{
	auto open_file = openFileDialog("");
	if (!open_file.empty())
	{
		// Cleanup previous scene.
		clearScene();

		// Import new scene.
		std::ifstream in_file(open_file);
		assert(in_file.good() && "Failed to read level file on import.");
		std::stringstream str_stream;
		str_stream << in_file.rdbuf();

		// Check level node exists to say this is a level file.
		YAML::Node dat = YAML::Load(str_stream.str());
		if (!dat["Level"])
		{
			// TODO: Logging system.
#ifdef _DEBUG
			std::cout << "Error: Attempting to import a .LEVEL file with incorrect formatting.";
#endif
			return;
		}

		// Deserialize entities and their components. Load any asset resources required.
		auto entities = dat["Entities"];
		if (entities)
		{
			auto* p_ecs = ecs();
			for (const auto& entity_node : entities)
			{
				auto deserialized_entity = createNewEntity().second;

				// Transform component.
				const auto& transform_component = entity_node["TransformComponent"];
				auto& transform = p_ecs->getComponent<TransformComponent>(deserialized_entity);

				transform.m_position = transform_component["Position"].as<DirectX::SimpleMath::Vector2>();
				transform.m_scale = transform_component["Scale"].as<DirectX::SimpleMath::Vector2>();
				transform.m_rotation_radians = transform_component["RotationRadians"].as<float>();

				// Sprite component.
				const auto& sprite_component = entity_node["SpriteComponent"];
				if (sprite_component)
				{
					SpriteComponent sprite;

					sprite.m_relative_position = sprite_component["RelativePosition"].as<DirectX::SimpleMath::Vector2>();
					sprite.m_relative_rotation_radians = sprite_component["RelativeRotationRadians"].as<float>();
					sprite.m_relative_scale = sprite_component["RelativeScale"].as<DirectX::SimpleMath::Vector2>();
					sprite.m_colour = sprite_component["Colour"].as<DirectX::SimpleMath::Color>();
					sprite.m_z_order = sprite_component["ZOrder"].as<float>();

					auto texture_filepath = sprite_component["Texture"].as<std::string>();
					if (!texture_filepath.empty())
					{
						m_editor_renderer->loadSerializedTexture(getDevice(), texture_filepath, open_file, sprite);
					}

					sprite.m_source_rect.top = sprite_component["SourceRectTop"].as<LONG>();
					sprite.m_source_rect.right = sprite_component["SourceRectRight"].as<LONG>();
					sprite.m_source_rect.bottom = sprite_component["SourceRectBottom"].as<LONG>();
					sprite.m_source_rect.left = sprite_component["SourceRectLeft"].as<LONG>();

					sprite.m_origin = sprite_component["Origin"].as<DirectX::SimpleMath::Vector2>();

					p_ecs->addComponent(deserialized_entity, sprite);
				}

				// Sprite anim component.
				const auto& sprite_anim_component = entity_node["SpriteAnimComponent"];
				if (sprite_anim_component)
				{
					SpriteAnimComponent sprite_anim;

					sprite_anim.m_relative_position = sprite_anim_component["RelativePosition"].as<DirectX::SimpleMath::Vector2>();
					sprite_anim.m_relative_rotation_radians = sprite_anim_component["RelativeRotationRadians"].as<float>();
					sprite_anim.m_relative_scale = sprite_anim_component["RelativeScale"].as<DirectX::SimpleMath::Vector2>();
					sprite_anim.m_colour = sprite_anim_component["Colour"].as<DirectX::SimpleMath::Color>();
					sprite_anim.m_z_order = sprite_anim_component["ZOrder"].as<float>();

					auto texture_filepath = sprite_anim_component["Texture"].as<std::string>();
					if (!texture_filepath.empty())
					{
						sprite_anim.m_texture_id = static_cast<int>(m_editor_renderer->loadSerializedTexture(getDevice(), texture_filepath, open_file));
					}

					// Read frames.
					const YAML::Node& frames_node = sprite_anim_component["Frames"];
					for (size_t i = 0; i < frames_node.size(); i++)
					{
						RECT frame_rect;
						frame_rect.top = frames_node["Frame" + std::to_string(i)]["SourceRectTop"].as<LONG>();
						frame_rect.right = frames_node["Frame" + std::to_string(i)]["SourceRectRight"].as<LONG>();
						frame_rect.bottom = frames_node["Frame" + std::to_string(i)]["SourceRectBottom"].as<LONG>();
						frame_rect.left = frames_node["Frame" + std::to_string(i)]["SourceRectLeft"].as<LONG>();

						sprite_anim.m_frames.emplace_back(frame_rect);
					}

					sprite_anim.m_playback_rate_seconds = sprite_anim_component["PlaybackRate"].as<float>();
					sprite_anim.m_looping = sprite_anim_component["Looping"].as<bool>();
					sprite_anim.m_origin = sprite_anim_component["Origin"].as<DirectX::SimpleMath::Vector2>();

					p_ecs->addComponent(deserialized_entity, sprite_anim);
				}

				// Enemy spawner component.
				const auto& enemy_spawner_component = entity_node["EnemySpawnerComponent"];
				if (enemy_spawner_component)
				{
					EnemySpawnerData enemy_spawner;

					enemy_spawner.relative_position = enemy_spawner_component["RelativePosition"].as<DirectX::SimpleMath::Vector2>();
					enemy_spawner.type = static_cast<EnemyType>(enemy_spawner_component["EnemyType"].as<int>());
					enemy_spawner.spawn_count = enemy_spawner_component["SpawnCount"].as<int>();
					enemy_spawner.spawn_interval = enemy_spawner_component["SpawnInterval"].as<float>();

					p_ecs->addComponent(deserialized_entity, enemy_spawner);
				}

				// Environment tag.
				const auto& environment_tag = entity_node["EnvironmentTag"];
				if (environment_tag)
				{
					p_ecs->addComponent(deserialized_entity, EnvironmentTag());
				}

				// TODO: Deserialize other types of components.
			}
		}
	}
}

void Editor::onCreateTileMapMenu()
{
	m_create_tilemap_menu->setShown(true);
}

void Editor::onTilemapSelected(const std::string& name)
{
	auto* tilemap = &m_tilemaps.at(name);
	m_scene_outliner->setTilemapSelectionContext(tilemap);
	m_details_panel->setSelectedTilemap(tilemap);
}

void Editor::onTilemapDestroyed(const std::string& name)
{
	const auto& entities = m_tilemaps.at(name).getEntities();

	for (const auto& entity : entities)
	{
		onEntityDestroyed(entity.first);
	}

	m_tilemaps.erase(name);
	m_details_panel->setSelectedTilemap(nullptr);
	m_scene_outliner->setTilemapSelectionContext(nullptr);
}

void Editor::clearScene()
{
	// TODO: Warn user they are about to lose the scene they are working on.
	auto* p_ecs = ecs();
	for (const auto& pair : m_scene_context)
	{
		p_ecs->destroyEntity(pair.second);
	}
	m_scene_context.clear();
	m_selection_context = {};
	m_details_panel->setSelectionContext(nullptr);
	// TODO: Cleanup renderer, deleting any textures loaded.
}

void Editor::pickFromScene(int mouse_x, int mouse_y)
{
	// TODO: Replace with pixel picking from a render target.

	auto* p_ecs = ecs();

	for (const auto& pair : m_scene_context)
	{
		const auto& entity = pair.second;

		auto transform = p_ecs->getComponent<TransformComponent>(entity);

		// Entity cannot be picked from the scene without a visible component.
		auto editor_cam_pos = m_editor_renderer->getEditorCameraPosition2D();
		if (p_ecs->hasComponent<SpriteComponent>(entity))
		{
			const auto& sprite = p_ecs->getComponent<SpriteComponent>(entity);
			transform.m_position += sprite.m_relative_position;
			transform.m_scale *= sprite.m_relative_scale;
			if (isCursorInBounds(static_cast<float>(mouse_x) + editor_cam_pos.x, static_cast<float>(mouse_y) + editor_cam_pos.y, transform.m_position, transform.m_scale, sprite.m_source_rect))
			{
				onEntitySelected(pair.first);
			}
		}

		if (p_ecs->hasComponent<SpriteAnimComponent>(entity))
		{
			const auto& anim = p_ecs->getComponent<SpriteAnimComponent>(entity);
			if (anim.m_frames.size() > 0)
			{
				transform.m_position += anim.m_relative_position;
				transform.m_scale *= anim.m_relative_scale;
				if (isCursorInBounds(static_cast<float>(mouse_x) + editor_cam_pos.x, static_cast<float>(mouse_y) + editor_cam_pos.y, transform.m_position, transform.m_scale, anim.m_frames[0]))
				{
					onEntitySelected(pair.first);
				}
			}
		}
	}
}

bool Editor::isCursorInBounds(float mouse_x, float mouse_y, const DirectX::SimpleMath::Vector2& position, const DirectX::SimpleMath::Vector2& scale, RECT bounds)
{
	auto width = ((bounds.right - bounds.left) / 2.f) * scale.x;
	auto height = ((bounds.bottom - bounds.top) / 2.f) * scale.y;

	return mouse_x < (position.x + width) && mouse_x >(position.x - width) &&
		mouse_y < (position.y + height) && mouse_y >(position.y - height);
}

void Editor::resetCamera()
{
	m_editor_renderer->resetCameraZoom();
	m_editor_renderer->resetCameraPosition();
}

std::string Editor::openFileDialog(const char* filters)
{
	OPENFILENAMEA ofn;
	CHAR szFile[M_FILE_DIALOG_BUFFER_SIZE] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = getWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filters;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}
	return std::string();
}

std::string Editor::saveFileDialog(const char* filters)
{
	OPENFILENAMEA ofn;
	CHAR szFile[M_FILE_DIALOG_BUFFER_SIZE] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = getWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filters;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}
	return std::string();
}

void Editor::exportLevel()
{
	// Get the destination export path
	auto target_file = saveFileDialog(m_export_filters);
	if (!target_file.empty())
	{
		// Serialize entities and their components and export level file. Serialize any resources used by a component for the game.
		auto* p_ecs = ecs();
		std::vector<size_t> exported_texture_ids;

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Level" << YAML::Value << "Untitled";
		//out << YAML::Key << "NumTextures" << YAML::Value << std::to_string(m_asset_list->getTextures().size());
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		for (const auto& pair : m_scene_context)
		{
			const auto& entity = pair.second;

			out << YAML::BeginMap; // Entity
			out << YAML::Key << "Entity" << YAML::Value << entity;

			// Transform component.
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // Transform component.

			const auto& transform = ecs()->getComponent<TransformComponent>(entity);
			out << YAML::Key << "Position" << YAML::Value << transform.m_position;
			out << YAML::Key << "Scale" << YAML::Value << transform.m_scale;
			out << YAML::Key << "RotationRadians" << YAML::Value << transform.m_rotation_radians;

			out << YAML::EndMap; // Transform component.

			// Sprite component.
			if (p_ecs->hasComponent<SpriteComponent>(entity))
			{
				out << YAML::Key << "SpriteComponent";
				out << YAML::BeginMap; // Sprite component

				const auto& sprite = p_ecs->getComponent<SpriteComponent>(entity);

				out << YAML::Key << "RelativePosition" << YAML::Value << sprite.m_relative_position;
				out << YAML::Key << "RelativeRotationRadians" << YAML::Value << sprite.m_relative_rotation_radians;
				out << YAML::Key << "RelativeScale" << YAML::Value << sprite.m_relative_scale;
				out << YAML::Key << "Colour" << YAML::Value << sprite.m_colour;
				out << YAML::Key << "ZOrder" << YAML::Value << sprite.m_z_order;

				if (sprite.m_texture_id != -1)
				{
					std::string filename = PathFindFileNameA(m_editor_renderer->getTextureFilepath(sprite.m_texture_id).c_str());
					ID3D11Resource* resource;
					m_editor_renderer->getTextureSRV(sprite.m_texture_id)->GetResource(&resource);
					assert(resource != nullptr && "Failed to get d3d11 resource.");
					auto texture_export_path = target_file.substr(0, target_file.find_last_of("\\")) + "\\" + filename.substr(0, filename.find_last_of('.')) + ".dds";
					writeDDS(resource, texture_export_path);
					out << YAML::Key << "Texture" << YAML::Value << "Assets\\" + std::string(PathFindFileNameA(texture_export_path.c_str()));
				}
				else
				{
					out << YAML::Key << "texture" << YAML::Value << "";
				}

				out << YAML::Key << "SourceRectTop" << YAML::Value << sprite.m_source_rect.top;
				out << YAML::Key << "SourceRectRight" << YAML::Value << sprite.m_source_rect.right;
				out << YAML::Key << "SourceRectBottom" << YAML::Value << sprite.m_source_rect.bottom;
				out << YAML::Key << "SourceRectLeft" << YAML::Value << sprite.m_source_rect.left;

				out << YAML::Key << "Origin" << YAML::Value << sprite.m_origin;

				out << YAML::EndMap; // Sprite component.
			}

			// Sprite anim component.
			if (p_ecs->hasComponent<SpriteAnimComponent>(entity))
			{
				out << YAML::Key << "SpriteAnimComponent";
				out << YAML::BeginMap; // Sprite anim component

				const auto& sprite_anim = p_ecs->getComponent<SpriteAnimComponent>(entity);

				out << YAML::Key << "RelativePosition" << YAML::Value << sprite_anim.m_relative_position;
				out << YAML::Key << "RelativeRotationRadians" << YAML::Value << sprite_anim.m_relative_rotation_radians;
				out << YAML::Key << "RelativeScale" << YAML::Value << sprite_anim.m_relative_scale;
				out << YAML::Key << "Colour" << YAML::Value << sprite_anim.m_colour;
				out << YAML::Key << "ZOrder" << YAML::Value << sprite_anim.m_z_order;

				if (sprite_anim.m_texture_id != -1)
				{
					std::string filename = PathFindFileNameA(m_editor_renderer->getTextureFilepath(sprite_anim.m_texture_id).c_str());
					ID3D11Resource* resource;
					m_editor_renderer->getTextureSRV(sprite_anim.m_texture_id)->GetResource(&resource);
					assert(resource != nullptr && "Failed to get d3d11 resource.");
					auto texture_export_path = target_file.substr(0, target_file.find_last_of("\\")) + "\\" + filename.substr(0, filename.find_last_of('.')) + ".dds";
					writeDDS(resource, texture_export_path);
					out << YAML::Key << "Texture" << YAML::Value << "Assets\\" + std::string(PathFindFileNameA(texture_export_path.c_str()));
				}
				else
				{
					out << YAML::Key << "texture" << YAML::Value << "";
				}

				// Write out each frame.
				out << YAML::Key << "Frames" << YAML::Value << YAML::BeginMap; // Frames
				for (size_t i = 0; i < sprite_anim.m_frames.size(); i++)
				{
					out << YAML::Key << "Frame" + std::to_string(i) << YAML::Value << YAML::BeginMap; // Frame
					out << YAML::Key << "SourceRectTop" << YAML::Value << sprite_anim.m_frames[i].top;
					out << YAML::Key << "SourceRectRight" << YAML::Value << sprite_anim.m_frames[i].right;
					out << YAML::Key << "SourceRectBottom" << YAML::Value << sprite_anim.m_frames[i].bottom;
					out << YAML::Key << "SourceRectLeft" << YAML::Value << sprite_anim.m_frames[i].left;
					out << YAML::EndMap; // Frame
				}
				out << YAML::EndMap; // Frames

				out << YAML::Key << "PlaybackRate" << YAML::Value << sprite_anim.m_playback_rate_seconds;
				out << YAML::Key << "Looping" << YAML::Value << sprite_anim.m_looping;
				out << YAML::Key << "Origin" << YAML::Value << sprite_anim.m_origin;

				out << YAML::EndMap; // Sprite anim component.
			}

			// Enemy spawner component.
			if (p_ecs->hasComponent<EnemySpawnerData>(entity))
			{
				out << YAML::Key << "EnemySpawnerComponent";
				out << YAML::BeginMap; // Enemy spawner component

				const auto& enemy_spawner = p_ecs->getComponent<EnemySpawnerData>(entity);

				out << YAML::Key << "RelativePosition" << YAML::Value << enemy_spawner.relative_position;
				out << YAML::Key << "EnemyType" << YAML::Value << static_cast<int>(enemy_spawner.type);
				out << YAML::Key << "SpawnCount" << YAML::Value << enemy_spawner.spawn_count;
				out << YAML::Key << "SpawnInterval" << YAML::Value << enemy_spawner.spawn_interval;

				out << YAML::EndMap; // Enemy spawner component.
			}

			// Environment tag.
			if (p_ecs->hasComponent<EnvironmentTag>(entity))
			{
				out << YAML::Key << "EnvironmentTag";
				out << YAML::BeginMap; // Environment tag
				out << YAML::EndMap; // Environment tag
			}

			// TODO: Other components...

			out << YAML::EndMap; // Entity
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		// Write the level file.
		std::ofstream out_file(target_file + M_LEVEL_FILE_EXTENSION);
		out_file << out.c_str();
	}
}

void Editor::writeDDS(ID3D11Resource* resource, const std::string& out_filepath)
{
	DirectX::ScratchImage image;
	HRESULT hr = DirectX::CaptureTexture(getDevice(), getDeviceContext(), resource, image);
	assert(hr == S_OK && "Failed to capture texture.");

	//DirectX::Blob // Binary blob - get buffer pointer and size to write binary data if needed.
	//DirectX::SaveToDDSMemory()
	hr = DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, Helper::charToWChar(out_filepath.c_str()));
	assert(hr == S_OK && "Failed to save dds.");
}

void Editor::createTilemap(const TilemapCreationInfo& creation_info)
{
	std::string name = "Tilemap_" + std::to_string(m_tilemap_id);
	m_tilemaps.emplace(std::make_pair( name, Tilemap(creation_info, name, ecs())));
	auto& tilemap = m_tilemaps.at(name);

	auto width = tilemap.getWidth(); 
	auto height = tilemap.getHeight();
	auto* p_ecs = ecs();

	// Spawn an entity with a sprite component at each tile in the map.
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			auto pair = createNewEntity();
			auto ent = pair.second;
			tilemap.push_back_entity(pair);

			auto& transform = p_ecs->getComponent<TransformComponent>(ent);
			auto tile_size = tilemap.getTileSize();
			transform.m_position = tilemap.getPosition() + DirectX::SimpleMath::Vector2{ static_cast<float>(x * tile_size), static_cast<float>(y * tile_size) };

			SpriteComponent sprite;

			sprite.m_texture_id = tilemap.getSourceTextureId();
			sprite.m_source_rect.right = static_cast<LONG>(tile_size);
			sprite.m_source_rect.bottom = static_cast<LONG>(tile_size);
			sprite.m_origin = 0.5f * DirectX::SimpleMath::Vector2(static_cast<float>((sprite.m_source_rect.right - sprite.m_source_rect.left)),
				static_cast<float>((sprite.m_source_rect.bottom - sprite.m_source_rect.top)));
			sprite.m_z_order = creation_info.z_order;

			p_ecs->addComponent(ent, sprite);
		}
	}

	m_tilemap_id++;
}

void Editor::quit()
{
	PostQuitMessage(0);
}

int Editor::onImportTexture()
{
	std::string file = openFileDialog("");
	if (!file.empty())
	{
		const auto& textures = m_editor_renderer->getTextureFilepaths();
		auto textures_begin_it = textures.begin();
		auto textures_end_it = textures.end();
		auto find_it = std::find(textures_begin_it, textures_end_it, file);
		if (find_it != textures_end_it)
		{
			// Asset already exists.
			return static_cast<int>(find_it - textures_begin_it);
		}

		return importTextureFile(file);
	}
	return -1;
}

int Editor::importTextureFile(const std::string& filepath)
{
	int width, height, channels;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	Texture texture(data, width, height, channels, filepath);
	if (!texture.isValid())
	{
		MessageBox(getWindow(), Helper::charToWChar(("Failed to load texture " + filepath).c_str()), L"Import Error", MB_ICONERROR | MB_OK);
		return -1;
	}

	// Texture was imported succesfully.
	auto texture_id = m_editor_renderer->loadTexture(getDevice(), getDeviceContext(), texture);
	stbi_image_free(texture.getPixels());

	return static_cast<int>(texture_id);
}
