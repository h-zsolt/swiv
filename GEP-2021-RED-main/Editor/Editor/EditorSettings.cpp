#include <imgui.h>

#include "EditorSettings.h"

void EditorSettings::dialog()
{
	ImGui::Begin("Editor settings", &m_show, ImGuiWindowFlags_NoCollapse);
	{
		ImGui::DragInt("Editor camera speed", &m_camera_speed, 0.025f, 1, 8);
		ImGui::DragInt("Editor zoom sensitivity", &m_zoom_sensivity, 0.025f, 1, 8);
	}
	ImGui::End();
}
