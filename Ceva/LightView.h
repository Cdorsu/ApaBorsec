#pragma once

#include "common.h"


#include <Windows.h>
#include <DirectXMath.h>

_declspec( align( 16 ) ) class CLightView
{
	DirectX::XMMATRIX LightView;
	DirectX::XMMATRIX LightProjection;
	DirectX::XMVECTOR UpVector;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Focus;
	float FOV;
	float NearPlane;
	float FarPlane;
	float WindowWidth;
	float WindowHeight;
	common::Color Diffuse;
	common::Color Ambient;
public:
	CLightView( );
	~CLightView( );
public:
	bool Initialize( );
	void Shutdown( );
public:
	void GenerateViewMatrix( );
	void GenerateProjectionMatrix( );
public:
	void SetPerspective( float FOV, float NearPlane, float FarPlane, float WindowWidth, float WindowHeight );
	void SetPosition( DirectX::XMFLOAT3 Position );
	void SetFocus( DirectX::XMFLOAT3 Focus );
	void SetDiffuse( common::Color Diffuse );
	void SetAmbient( common::Color Ambient );
	DirectX::XMMATRIX& GetView( );
	DirectX::XMMATRIX& GetProjection( );
	DirectX::XMFLOAT3 GetPosition( );
	DirectX::XMFLOAT3 GetFocus( );
	float GetFOV( );
	float GetNearPlane( );
	float GetFarPlane( );
	common::Color& GetDiffuse( );
	common::Color& GetAmbient( );
public:
	void* operator new( size_t size );
	void operator delete( void* object );
};

