#pragma once

#include "common.h"
#include <DirectXMath.h>

class CLight
{
	DirectX::XMFLOAT3 LightDir;
	common::Color DiffuseColor;
	common::Color AmbientColor;
	common::Color SpecularColor;
	float SpecularPower;
public:

	CLight()
	{
		ZeroMemory( this, sizeof( CLight ) );
	}

	~CLight()
	{
	}

	inline void SetSpecularColor( common::Color newColor )
	{
		SpecularColor = newColor;
	}

	inline void SetAmbientColor( common::Color newColor )
	{
		AmbientColor = newColor;
	}

	inline void SetDiffuseColor( common::Color newColor )
	{
		DiffuseColor = newColor;
	}

	inline void SetDirection( DirectX::XMFLOAT3 newDir )
	{
		LightDir = newDir;
	}

	inline void SetSpecularPower( float newPower )
	{
		SpecularPower = newPower;
	}

	inline common::Color GetSpecularColor()
	{
		return SpecularColor;
	}
	
	inline common::Color GetAmbientColor()
	{
		return AmbientColor;
	}

	inline common::Color GetDiffuseColor()
	{
		return DiffuseColor;
	}

	inline DirectX::XMFLOAT3 GetDirection()
	{
		return LightDir;
	}

	inline float GetSpecularPower()
	{
		return SpecularPower;
	}
};

