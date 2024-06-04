#pragma once

#include <pch.h>

class Camera2D
{
public:
	void addOffset(const DirectX::SimpleMath::Vector3& offset); 

	void setPosition(const DirectX::SimpleMath::Vector3& position);
	const DirectX::SimpleMath::Vector3& getPosition() const;

	float getZoom() const;
	void setZoom(float zoom);
	void addZoomDelta(float delta);
	void resetZoom();

	DirectX::SimpleMath::Matrix getMatrix() const;

private:
	static constexpr float M_DEFAULT_ZOOM = 1.f;

	DirectX::SimpleMath::Vector3 m_position;
	float m_zoom = M_DEFAULT_ZOOM;
	float m_min_zoom = 0.1f;
};