#include "pch.h"
#include "Camera2D.h"

void Camera2D::addOffset(const DirectX::SimpleMath::Vector3& offset)
{
	m_position += offset;
}

void Camera2D::setPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_position = position;
}

const DirectX::SimpleMath::Vector3& Camera2D::getPosition() const
{
	return m_position;
}

float Camera2D::getZoom() const
{
	return m_zoom;
}

void Camera2D::setZoom(float zoom)
{
	m_zoom = zoom < m_min_zoom ? m_min_zoom : zoom;
}

void Camera2D::addZoomDelta(float delta)
{
	m_zoom += delta;
	if (m_zoom < m_min_zoom) m_zoom = m_min_zoom;
}

DirectX::SimpleMath::Matrix Camera2D::getMatrix() const
{
	return DirectX::SimpleMath::Matrix::CreateScale(m_zoom) * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
}

void Camera2D::resetZoom()
{
	m_zoom = M_DEFAULT_ZOOM;
}
