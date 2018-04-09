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
#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

namespace utils {

enum Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
	public:
		Camera();

		Camera(DirectX::XMFLOAT3 position);

		Camera(float x, float y, float z);

		void ProcessMovement(Movement direction, float delta_time);

		void ProcessRotation(float x_offset, float y_offset);

		void ProcessZoom(float y_offset);

		DirectX::XMMATRIX ViewMatrix() const;

		DirectX::XMMATRIX ProjectionMatrix(float width, float height) const;

		DirectX::XMFLOAT3 GetPosition() const;

	private:
		void UpdateVectors();

		DirectX::XMFLOAT3 m_Position;

		DirectX::XMFLOAT3 m_U;

		DirectX::XMFLOAT3 m_V;

		DirectX::XMFLOAT3 m_W;

		DirectX::XMFLOAT3 m_WorldUp;

		float m_Yaw;

		float m_Pitch;

		float m_MovementSpeed;

		float m_Sensitivity;

		float m_Zoom;
};

inline DirectX::XMMATRIX Camera::ViewMatrix() const {
	using namespace DirectX;
	XMVECTOR eye = XMLoadFloat3(&m_Position);
	XMVECTOR focus = XMLoadFloat3(&m_W);
	XMVECTOR up = XMLoadFloat3(&m_V);

	return XMMatrixLookAtRH(eye, eye - focus, up);
}

inline DirectX::XMMATRIX Camera::ProjectionMatrix(float width, float height) const {
	return DirectX::XMMatrixPerspectiveRH(width, height, 0.0001, 100.0);
}

inline DirectX::XMFLOAT3 Camera::GetPosition() const {
	return m_Position;
}

}

#endif