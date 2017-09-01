#pragma once

#include "D3DClass.h"

class SphereShader
{
private:
	struct SConstantBuffer
	{
		DirectX::XMMATRIX WVP;
		DirectX::XMFLOAT3 EyePos;
		float pad;
	};
private:
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11GeometryShader *GeometryShader;
	ID3D11InputLayout *InputLayout;
	ID3D11Buffer *ConstantBuffer;
	ID3D11SamplerState *Sampler;
public:
	SphereShader( );
	~SphereShader( );
public:
	bool Initialize( ID3D11Device* device );
	void Render( ID3D11DeviceContext * context, UINT IndexDrawAmount,
				 DirectX::XMMATRIX& World, DirectX::XMMATRIX& View,
				 DirectX::XMMATRIX& Projection, DirectX::XMFLOAT3 CamPos );
	void Shutdown( );
private:
	void OutputShaderError( ID3D10Blob * Error );
};

