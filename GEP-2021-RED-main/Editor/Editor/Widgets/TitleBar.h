#pragma once

#include <pch.h>

class TitleBar
{
public:
    void titleBar();

    void registerOnQuitEvent(const std::function<void()>& funct);
    void registerOnEditorSettingsEvent(const std::function<void()>& funct);
    void registerOnSaveSceneEvent(const std::function<void()>& funct);
    void registerOnLoadSceneEvent(const std::function<void()>& funct);
    void registerOnNewSceneEvent(const std::function<void()>& funct);
    void registerOnCreateTilemapEvent(const std::function<void()>& funct);

private:
    void fileMenu();
    void editMenu();

private:
    std::function<void()> m_on_quit_callback;
    std::function<void()> m_on_editor_settings_callback;
    std::function<void()> m_on_save_scene_callback;
    std::function<void()> m_on_load_scene_callback;
    std::function<void()> m_on_new_scene_callback;
    std::function<void()> m_on_create_tilemap_callback;
};