#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>

#define GROUPWIDTH 256
#define GROUPHEIGHT 256
#define MAXBLURRADIUS 11
#define DEFAULTSIGMA 3

class BlurImage
{
private:
	struct BlurInfo
	{
		int radius;
		float weights[MAXBLURRADIUS];
	};
private:
	ID3D11Device * m_device;
	ID3D11DeviceContext * m_immediateContext;
	
	
	ID3D11ComputeShader * m_horizontalBlurShader;
	ID3D11ComputeShader * m_verticalBlurShader;

	ID3D11ShaderResourceView * m_inputTexture;
	ID3D11UnorderedAccessView * m_inputAccess;

	ID3D11ShaderResourceView * m_outputTexture;
	ID3D11UnorderedAccessView * m_outputAccess;

	ID3D11Buffer * m_blurInfo;

	BlurInfo m_blur;

	int m_horizontalGroups;
	int m_verticalGroups;

	int mWidth;
	int mHeight;
public:
	BlurImage();
	BlurImage(ID3D11Device*, ID3D11DeviceContext*, ID3D11ShaderResourceView*, ID3D11UnorderedAccessView*&);
	~BlurImage();
public:
	bool SetResource(ID3D11ShaderResourceView*, ID3D11UnorderedAccessView*&);
	void PerformBlur();
	void CalculateWeights(int radius, float sigma);
	void ScreenShot();
	inline ID3D11ShaderResourceView* GetTexture()
	{
		return m_outputTexture;
	}

private:
	void ClearResources();
};

