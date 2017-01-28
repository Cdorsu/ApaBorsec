#pragma once

#include "common.h"
#include "Input.h"

#include <DirectXMath.h>
#include <Windows.h>

__declspec (align(16)) class CCamera
{
	static const DirectX::XMVECTOR DefaultForward;
	static const DirectX::XMVECTOR DefaultRight;
	DirectX::XMVECTOR camPos;
	DirectX::XMVECTOR camForward;
	DirectX::XMVECTOR camRight;
	DirectX::XMVECTOR camUp;
	DirectX::XMMATRIX camView;
	DirectX::XMMATRIX camProjection;
	std::vector<DirectX::XMFLOAT4> camFrustum;
	float camSpeed;
	float camYaw = 0.0f;
	float camPitch = 0.0f;
	float camRoll = 0.0f;
public:
	CCamera();
	~CCamera();
public:
	bool Initialize( DirectX::XMVECTOR& InitialPos, float FOV, float AspectRatio, float NearZ, float FarZ, float cameraSpeed );
	void Render();
	void Update(float FrameTime);
	bool isPointinFrustum( float x, float y, float z );
	bool isCubeinFrustum( float sortofradius, float x, float y, float z );
	bool isSphereinFrustum( float radius, float x, float y, float z );
	void Shutdown();
private:
	void ConstructFrustum();
public:
	void* operator new(size_t size);
	void operator delete(void* object);
public:
	inline DirectX::XMMATRIX GetView() { return camView; };
	inline DirectX::XMMATRIX GetProjection() { return camProjection; };
	inline DirectX::XMFLOAT3 GetCameraPosition() { DirectX::XMFLOAT3 ret; DirectX::XMStoreFloat3( &ret, camPos ); return ret; };
public:
	inline void SetYaw( float Value ) { camYaw = Value; };
	inline void SetPitch( float Value ) { camPitch = Value; };
};

