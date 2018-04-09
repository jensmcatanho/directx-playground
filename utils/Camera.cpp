/*
-----------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2018 Jean Michel Catanho

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "utils/Camera.h"

namespace utils {

Camera::Camera() :
	m_Position(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }),
	m_WorldUp(DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f }),
	m_Yaw(YAW),
	m_Pitch(PITCH), 
	m_MovementSpeed(SPEED),
	m_Sensitivity(SENSITIVITY),
	m_Zoom(ZOOM) {

	UpdateVectors();
}

Camera::Camera(DirectX::XMFLOAT3 position) :
	m_Position(position),
	m_WorldUp(DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f }),
	m_Yaw(YAW),
	m_Pitch(PITCH), 
	m_MovementSpeed(SPEED),
	m_Sensitivity(SENSITIVITY),
	m_Zoom(ZOOM) {

	UpdateVectors();
}

Camera::Camera(float x, float y, float z) :
	m_Position(DirectX::XMFLOAT3{ x, y, z }),
	m_WorldUp(DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f }),
	m_Yaw(YAW),
	m_Pitch(PITCH), 
	m_MovementSpeed(SPEED),
	m_Sensitivity(SENSITIVITY),
	m_Zoom(ZOOM) {

	UpdateVectors();
}

void Camera::ProcessMovement(Movement direction, float delta_time) {
	using namespace DirectX;
	float velocity = m_MovementSpeed * delta_time;
	XMVECTOR position = XMLoadFloat3(&m_Position);

	if (direction == FORWARD) {
		XMVECTOR w = XMLoadFloat3(&m_W);
		position = XMVectorSubtract(position, XMVectorScale(w, velocity));

	} else if (direction == BACKWARD) {
		XMVECTOR w = XMLoadFloat3(&m_W);
		position = XMVectorAdd(position, XMVectorScale(w, velocity));

	} else if (direction == LEFT) {
		XMVECTOR u = XMLoadFloat3(&m_U);
		position = XMVectorSubtract(position, XMVectorScale(u, velocity));

	} else {
		XMVECTOR u = XMLoadFloat3(&m_U);
		position = XMVectorAdd(position, XMVectorScale(u, velocity));
	}
}

void Camera::ProcessRotation(float x_offset, float y_offset) {
	m_Yaw += x_offset * m_Sensitivity;
	m_Pitch += y_offset * m_Sensitivity;

	if (m_Pitch > 89.9f)
		m_Pitch = 89.9f;
	if (m_Pitch < -89.9f)
		m_Pitch = -89.9f;

	UpdateVectors();
}

void Camera::ProcessZoom(float y_offset) {
	if (m_Zoom >= 1.0f && m_Zoom <= 45.0f)
		m_Zoom -= y_offset;

	if (m_Zoom <= 1.0f)
		m_Zoom = 1.0f;
	if (m_Zoom >= 45.0f)
		m_Zoom = 45.0f;
}

void Camera::UpdateVectors() {
	using namespace DirectX;
	XMFLOAT3 lookAt;
	lookAt.x = cos(XMConvertToRadians(m_Yaw)) * cos(XMConvertToRadians(m_Pitch));
	lookAt.y = sin(XMConvertToRadians(m_Pitch));
	lookAt.z = sin(XMConvertToRadians(m_Yaw)) * cos(XMConvertToRadians(m_Pitch));
	
	XMVECTOR W = -XMVector3Normalize(XMLoadFloat3(&lookAt));
	XMStoreFloat3(&m_W, W);

	XMVECTOR U = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_WorldUp), W));
	XMStoreFloat3(&m_U, U);

	XMVECTOR V = XMVector3Cross(W, U);
	XMStoreFloat3(&m_V, V);
}

}