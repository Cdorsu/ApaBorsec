#include "Camera.h"

const DirectX::XMVECTOR CCamera::DefaultForward = DirectX::XMVectorSet( 0.0f, 0.0f, 1.0f, 1.0f );
const DirectX::XMVECTOR CCamera::DefaultRight = DirectX::XMVectorSet( 1.0f, 0.0f, 0.0f, 1.0f );

CCamera::CCamera()
{
	ZeroMemory( this, sizeof( this ) );
}

bool CCamera::Initialize( DirectX::XMVECTOR& InitialPos, float FOV, float AspectRatio, float NearZ, float FarZ, float cameraSpeed )
{
	using namespace DirectX; // For XMVECTOR Operators
	camSpeed = cameraSpeed;
	camPos = InitialPos;
	camForward = DefaultForward;
	camRight = DefaultRight;
	camUp = DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f );
	camView = DirectX::XMMatrixLookToLH( camPos, camForward, camUp );
	camProjection = DirectX::XMMatrixPerspectiveFovLH( FOV, AspectRatio, NearZ, FarZ );
	camFrustum.resize( 6 );
	return true;
}

void CCamera::ConstructFrustum()
{
	using namespace DirectX;
	XMMATRIX ViewProj = camView * camProjection;
	XMFLOAT4 Plane;
	// Near plane
	Plane.x = ViewProj._13;
	Plane.y = ViewProj._23;
	Plane.z = ViewProj._33;
	Plane.w = ViewProj._43;
	camFrustum[0] = Plane;
	// Far plane
	Plane.x = ViewProj._14 - ViewProj._13;
	Plane.y = ViewProj._24 - ViewProj._23;
	Plane.z = ViewProj._34 - ViewProj._33;
	Plane.w = ViewProj._44 - ViewProj._43;
	camFrustum[1] = Plane;
	// Left plane
	Plane.x = ViewProj._14 + ViewProj._11;
	Plane.y = ViewProj._24 + ViewProj._21;
	Plane.z = ViewProj._34 + ViewProj._31;
	Plane.w = ViewProj._44 + ViewProj._41;
	camFrustum[2] = Plane;
	// Right plane
	Plane.x = ViewProj._14 - ViewProj._11;
	Plane.y = ViewProj._24 - ViewProj._21;
	Plane.z = ViewProj._34 - ViewProj._31;
	Plane.w = ViewProj._44 - ViewProj._41;
	camFrustum[3] = Plane;
	// Top plane
	Plane.x = ViewProj._14 - ViewProj._12;
	Plane.y = ViewProj._24 - ViewProj._22;
	Plane.z = ViewProj._34 - ViewProj._32;
	Plane.w = ViewProj._44 - ViewProj._42;
	camFrustum[4] = Plane;
	// Bottom plane
	Plane.x = ViewProj._14 + ViewProj._12;
	Plane.y = ViewProj._24 + ViewProj._22;
	Plane.z = ViewProj._34 + ViewProj._32;
	Plane.w = ViewProj._44 + ViewProj._42;
	camFrustum[5] = Plane;

	for (UINT i = 0; i < camFrustum.size(); ++i)
	{
		float length = sqrt( (camFrustum[i].x * camFrustum[i].x) + (camFrustum[i].y * camFrustum[i].y) + (camFrustum[i].z * camFrustum[i].z) );
		camFrustum[i].x /= length;
		camFrustum[i].y /= length;
		camFrustum[i].z /= length;
		camFrustum[i].w /= length;
	}
}

bool CCamera::isPointinFrustum( float x, float y, float z )
{
	using namespace DirectX;
	for (UINT i = 0; i < camFrustum.size(); ++i)
	{
		XMVECTOR planeNormal = XMVectorSet( camFrustum[i].x, camFrustum[i].y, camFrustum[i].z, 0.0f );
		float planeConstant = camFrustum[i].w;
		if (XMVectorGetX( XMVector3Dot( planeNormal, XMVectorSet( x, y, z, 0.0f ) ) ) + planeConstant < 0.0f)
			return false;
	}
	return true;
}

bool CCamera::isCubeinFrustum( float sortofradius, float x, float y, float z )
{
	if (isPointinFrustum( x + sortofradius, y + sortofradius, z + sortofradius ))
		return true;
	if (isPointinFrustum( x + sortofradius, y + sortofradius, z - sortofradius ))
		return true;
	if (isPointinFrustum( x + sortofradius, y - sortofradius, z - sortofradius ))
		return true;
	if (isPointinFrustum( x + sortofradius, y - sortofradius, z + sortofradius ))
		return true;
	if (isPointinFrustum( x - sortofradius, y + sortofradius, z + sortofradius ))
		return true;
	if (isPointinFrustum( x - sortofradius, y + sortofradius, z - sortofradius ))
		return true;
	if (isPointinFrustum( x - sortofradius, y - sortofradius, z - sortofradius ))
		return true;
	if (isPointinFrustum( x - sortofradius, y - sortofradius, z + sortofradius ))
		return true;
	return false;
}

bool CCamera::isSphereinFrustum( float radius, float x, float y, float z )
{
	using namespace DirectX;
	for (UINT i = 0; i < camFrustum.size(); ++i)
	{
		XMVECTOR planeNormal = XMVectorSet( camFrustum[i].x, camFrustum[i].y, camFrustum[i].z, 0.0f );
		float planeConstant = camFrustum[i].w;
		if (XMVectorGetX( XMVector3Dot( planeNormal, XMVectorSet( x, y, z, 0.0f ) ) ) + planeConstant < -radius)
			return false;
	}
	return true;
}

void CCamera::Update(float frametime)
{
	using namespace DirectX;
	camPitch += INPUT_INSTANCE->GetMouseVerticalMove() * 0.001f;
	camYaw += INPUT_INSTANCE->GetMouseHorizontalMove() * 0.001f;

	if (INPUT_INSTANCE->isKeyPressed( DIK_W ))
		camPos += camSpeed * frametime * camForward;
	if (INPUT_INSTANCE->isKeyPressed( DIK_S ))
		camPos -= camSpeed * frametime * camForward;
	if (INPUT_INSTANCE->isKeyPressed( DIK_D ))
		camPos += camSpeed * frametime * camRight;
	if (INPUT_INSTANCE->isKeyPressed( DIK_A ))
		camPos -= camSpeed * frametime * camRight;
}

void CCamera::Render()
{
	using namespace DirectX;
	if ( camPitch <= -1.5f )
		camPitch = -1.5f;
	else if ( camPitch >= 1.5f )
		camPitch = 1.5f;
	DirectX::XMMATRIX RotationMatrix;
	RotationMatrix = DirectX::XMMatrixRotationRollPitchYaw( camPitch, camYaw, camRoll );
	camForward = DirectX::XMVector3TransformCoord( DefaultForward, RotationMatrix );
	camForward = DirectX::XMVector3Normalize( camForward );

	camRight = DirectX::XMVector3TransformCoord( DefaultRight, RotationMatrix );
	camRight = DirectX::XMVector3Normalize( camRight );

	camUp = XMVector3Cross( camForward, camRight );

	camView = DirectX::XMMatrixLookToLH( camPos, camForward, camUp );

	ConstructFrustum();
}

void CCamera::Shutdown()
{

}

CCamera::~CCamera()
{
	Shutdown();
}

void* CCamera::operator new (size_t size)
{
	return _mm_malloc( size,16 );
}

void CCamera::operator delete(void* object)
{
	return _mm_free( object );
}
