#include <imgui.h>
#include "TitleBar.h"

void TitleBar::titleBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		fileMenu();
		editMenu();

		ImGui::EndMainMenuBar();
	}
}

void TitleBar::registerOnQuitEvent(const std::function<void()>& funct)
{
	m_on_quit_callback = funct;
}

void TitleBar::registerOnEditorSettingsEvent(const std::function<void()>& funct)
{
	m_on_editor_settings_callback = funct;
}

void TitleBar::registerOnSaveSceneEvent(const std::function<void()>& funct)
{
	m_on_save_scene_callback = funct;
}

void TitleBar::registerOnLoadSceneEvent(const std::function<void()>& funct)
{
	m_on_load_scene_callback = funct;
}

void TitleBar::registerOnNewSceneEvent(const std::function<void()>& funct)
{
	m_on_new_scene_callback = funct;
}

void TitleBar::registerOnCreateTilemapEvent(const std::function<void()>& funct)
{
	m_on_create_tilemap_callback = funct;
}

void TitleBar::fileMenu()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New scene"))
		{
			if (m_on_new_scene_callback)
			{
				m_on_new_scene_callback();
			}
		}

		if (ImGui::MenuItem("Load scene"))
		{
			if (m_on_load_scene_callback)
			{
				m_on_load_scene_callback();
			}
		}

		if (ImGui::MenuItem("Save scene"))
		{
			if (m_on_save_scene_callback)
			{
				m_on_save_scene_callback();
			}
		}

		if (ImGui::MenuItem("Quit"))
		{
			if (m_on_quit_callback)
			{
				m_on_quit_callback();
			}
		}
		ImGui::EndMenu();
	}
}

void TitleBar::editMenu()
{
	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Create tilemap"))
		{
			if (m_on_create_tilemap_callback)
			{
				m_on_create_tilemap_callback();
			}
		}

		if (ImGui::MenuItem("Editor settings"))
		{
			if (m_on_editor_settings_callback)
			{
				m_on_editor_settings_callback();
			}
		}
		ImGui::EndMenu();
	}
}
