#pragma once


#include <vector>
#include <array>

#include "D3DClass.h"
#include "Light.h"
#include "Texture.h"


class Terrain
{
	static constexpr const int CellsPerPath = 64;
	struct SHeightmap
	{
		DirectX::XMFLOAT3 Pos;
	};
	struct SPerObject
	{
		DirectX::XMMATRIX WVP;
	};
	struct SPerFrame
	{
		DirectX::XMFLOAT3 CamPos;
		float pad;
	};
	struct SCellInfo
	{
		float CellSpacingU;
		float CellSpacingV;
		DirectX::XMFLOAT2 pad;
	};
	struct SVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 Tex;
		DirectX::XMFLOAT2 Bounds;
	};
	struct SLight
	{
		common::Color Diffuse;
		common::Color Ambient;
		DirectX::XMFLOAT3 LightDir;
		float pad;
	};
private:
	std::vector<SHeightmap> mHeightmap;
	std::vector<DirectX::XMFLOAT2> mBoundsY;
	std::vector<SVertex> mVertices;
	std::vector<DWORD> mIndices;

	ID3D11VertexShader * mVertexShader;
	ID3D11HullShader * mHullShader;
	ID3D11DomainShader * mDomainShader;
	ID3D11PixelShader * mPixelShader;
	ID3D11InputLayout * mLayout;
	std::array<ID3DBlob*, 4> mBlobs;

	ID3D11Buffer * mPerObjectBuffer;
	ID3D11Buffer * mPerFrameBuffer;
	ID3D11Buffer * mCellSpacingBuffer;
	ID3D11Buffer * mLightBuffer;

	ID3D11SamplerState * mWrapSampler;

	ID3D11Buffer * mVertexBuffer;
	ID3D11Buffer * mIndexBuffer;

	CTexture * mTexture;

	int mNumRow, mNumCol;
	int mNumPathVertices, mNumQuadFaces;	
	int mNumRowVertices, mNumColVertices;

	float mCellTexSpacingU;
	float mCellTexSpacingV;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	Terrain( ID3D11Device * Device, ID3D11DeviceContext * Context );
	~Terrain( );
private:
	void InitShaders( );
	bool InitHeightmap( LPWSTR lpPath );
	bool InitBuffers( );
	void ComputeBounds( );
	void CalculateYBounds( int row, int col );
	void InitVertexBuffer( );
	void InitIndexBuffer( );
public:
	void Update( float frameTime );
	void Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection, DirectX::XMFLOAT3 const& EyePos, CLight* Light );
public:
	int GetWidth( )
	{
		return mNumCol;
	}
	int GetHeight( )
	{
		return mNumRow;
	}
	ID3D11ShaderResourceView * GetHeightmapAsSRV( )
	{
		return mTexture->GetTexture( );
	}
};

