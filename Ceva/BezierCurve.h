#pragma once

#include "D3DClass.h"
#include "Camera.h"


class BezierCurve
{
public:
	static constexpr const UINT MaxControlPoints = 16;
	struct SVertex
	{
		DirectX::XMFLOAT4 Position;
	};
	struct SControlPoints
	{
		UINT Number;
		DirectX::XMFLOAT3 Pad;
		DirectX::XMFLOAT4 ControlPoints[ 100 ];
	};
	struct SPerObject
	{
		DirectX::XMMATRIX WVP;
	};
private:
	ID3D11VertexShader * mVertexShader;
	ID3D11HullShader * mHullShader; 
	ID3D11DomainShader * mDomainShader;
	ID3D11PixelShader * mPixelShader;

	std::vector<ID3DBlob*> mShaders;

	ID3D11InputLayout * mLayout;
	
	ID3D11Buffer * mVertexBuffer;
	ID3D11Buffer * mPointsBuffer;
	ID3D11Buffer * mPerObjectBuffer;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	BezierCurve( ID3D11Device * Device, ID3D11DeviceContext * Context, SControlPoints const& Points );
	~BezierCurve( );
public:
	void __vectorcall Render( DirectX::FXMMATRIX& view, DirectX::FXMMATRIX& projection );
private:
	void InitializeBuffers( SControlPoints const& Points );
	void InitializeShaders( );
};

