#pragma once

#include "Model.h"
#include "Light.h"

class DisplacementMapping
{
	struct VSBuffer
	{
		DirectX::XMMATRIX World;
		DirectX::XMFLOAT3 EyePos;
		float pad;
	};
	struct DSBuffer
	{
		DirectX::XMMATRIX ViewProjection;
		DirectX::XMFLOAT3 EyePos;
		float pad;
	};
	struct PSBuffer
	{
		common::Color Diffuse;
		common::Color Ambient;
		DirectX::XMFLOAT3 LightDir;
		float pad;
	};
private:
	ID3D11VertexShader* mVertexShader;
	ID3D11HullShader* mHullShader;
	ID3D11DomainShader* mDomainShader;
	ID3D11PixelShader* mPixelShader;

	ID3D11InputLayout* mInputLayout;

	ID3D11Buffer* mVSBuffer;
	ID3D11Buffer* mDSBuffer;
	ID3D11Buffer* mPSBuffer;
	ID3D11SamplerState* mWrapSampler;

	CModel * mModel;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	DisplacementMapping( ID3D11Device* Device, ID3D11DeviceContext * Context );
	~DisplacementMapping( );
public:
	void Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection, DirectX::XMFLOAT3 EyePos, CLight* Light );
};

