#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "common.h"

class CPointLight
{

	DirectX::XMFLOAT3 Position;
	common::Color Diffuse;
	float Range;
	DirectX::XMFLOAT3 Attenuation;

public:

	CPointLight( )
	{
		ZeroMemory( this, sizeof( this ) );
	}

	void SetPosition( DirectX::XMFLOAT3 Position )
	{
		this->Position = Position;
	}

	void SetDiffuseColor( common::Color Color )
	{
		this->Diffuse = Color;
	}
	
	void SetRange( float range )
	{
		Range = range;
	}

	void SetAttenuation( float a0, float a1, float a2 )
	{
		Attenuation = DirectX::XMFLOAT3( a0, a1, a2 );
	}

	DirectX::XMFLOAT3 GetPosition( )
	{
		return Position;
	}

	common::Color GetDiffuseColor( )
	{
		return Diffuse;
	}

	float GetRange( )
	{
		return Range;
	}

	DirectX::XMFLOAT3 GetAttenuation( )
	{
		return Attenuation;
	}

	~CPointLight( )
	{
	}
};

