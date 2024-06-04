#pragma once

class EditorSettings
{
public:
	void dialog();

	void show(bool show) { m_show = show; }
	bool isShown() const { return m_show; }

	int getCameraSpeed() { return m_camera_speed * 100; }
	float getZoomSensitivity() { return static_cast<float>(m_zoom_sensivity) * 0.1f; }

private:
	bool m_show = false;

	// Editor settings.
	int m_camera_speed = 2;
	int m_zoom_sensivity = 1;
};