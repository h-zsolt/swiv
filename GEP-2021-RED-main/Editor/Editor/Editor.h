#pragma once

#include <pch.h>
#include <ScarleGame.h>

// Widgets.
#include "Widgets/TitleBar.h"
#include "EditorSettings.h"
#include "Widgets/SceneOutliner.h"
#include "Widgets/DetailsPanel.h"
#include "Widgets/CreateTilemapMenu.h"

// Assets.
#include "Assets/Texture.h"
#include "Assets/Tilemap.h"

#include "EditorRenderer.h"

// Class forward declarations.
class InputSystem;

// Struct forward declarations.
struct TilemapCreationInfo;

class Editor : public ScarleGame
{
public:
    Editor() noexcept = default;
    ~Editor() final = default;

    void start() override;
    void updateObjects(GameData* gd) override;
    void renderObjects() override;

    // Messages
    void onActivated() override;
    void onDeactivated() override;
    void onSuspending() override;
    void onResuming() override;
    void onWindowSizeChanged(int _width, int _height) override;

    // ImGui hook
    void RenderImGui() override;

    // Internals.
private:
    void quit();
    void openEditorSettings();
    void onEntityCreated();
    std::pair<std::string, ECS::Entity> createNewEntity();
    void onEntitySelected(const std::string& name);
    void onEntityDestroyed(const std::string& name);
    void onExportLevel();
    void onEntityDuplicated(const std::string& name);
    void onImportLevel();
    void onCreateTileMapMenu();
    void onTilemapSelected(const std::string& name);
    void onTilemapDestroyed(const std::string& name);

    void clearScene();
    void pickFromScene(int mouse_x, int mouse_y);
    bool isCursorInBounds(float mouse_x, float mouse_y, const DirectX::SimpleMath::Vector2& position, const DirectX::SimpleMath::Vector2& scale, RECT bounds);
    void resetCamera();

    // These return -1 if there was an error.
    int onImportTexture();
    int importTextureFile(const std::string& filepath);

    // These return empty strings if cancelled.
    std::string openFileDialog(const char* filters);
    std::string saveFileDialog(const char* filters);

    // Serializing.
    void exportLevel();
    void writeDDS(ID3D11Resource* resource, const std::string& out_filepath);

    // Tilemap.
    void createTilemap(const TilemapCreationInfo& creation_info);

private:
    std::unique_ptr<EditorRenderer> m_editor_renderer;
    InputSystem* m_input_system = nullptr;
    const char* m_texture_filters = "All\0*.*\0BMP\0*.BMP\0JPG\0*.JPG\0PNG\0*.PNG\0TGA\0*.TGA\0PSD\0*.PSD\0";
    const char* m_export_filters = "*.LEVEL";
    bool m_last_left_button_state = false;
    uint32_t m_tilemap_id = 0;

    static constexpr size_t M_FILE_DIALOG_BUFFER_SIZE = 260;
    const std::string M_LEVEL_FILE_EXTENSION = ".LEVEL";
    const DirectX::Keyboard::Keys M_RESET_CAMERA_KEY = DirectX::Keyboard::Keys::D1;

    // UI widgets.
    std::unique_ptr<TitleBar> m_title_bar;
    std::unique_ptr<EditorSettings> m_editor_settings;
    std::unique_ptr<SceneOutliner> m_scene_outliner;
    std::unique_ptr<DetailsPanel> m_details_panel;
    std::unique_ptr<CreateTilemapMenu> m_create_tilemap_menu;

    // Scene.
    std::unordered_map<std::string, ECS::Entity> m_scene_context; // Entity name with the entities in the scene.
    std::pair<std::string, ECS::Entity> m_selection_context;
    std::unordered_map<std::string, Tilemap> m_tilemaps; // Tilemap name with the tilemaps in the scene.
};

