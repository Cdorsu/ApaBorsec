#include "LightView.h"



CLightView::CLightView( )
{
	ZeroMemory( this, sizeof( CLightView ) );
}

bool CLightView::Initialize( )
{
	UpVector = DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f );
	return true;
}

void CLightView::GenerateViewMatrix( )
{
	LightView = DirectX::XMMatrixLookAtLH( DirectX::XMVectorSet( Position.x, Position.y, Position.z, 1.0f ),
		DirectX::XMVectorSet( Focus.x, Focus.y, Focus.z, 1.0f ), UpVector );
}

void CLightView::GenerateProjectionMatrix( )
{
	LightProjection = DirectX::XMMatrixPerspectiveFovLH( FOV, WindowWidth / WindowHeight, NearPlane, FarPlane );
}

void CLightView::SetView( DirectX::XMMATRIX& newView )
{
	this->LightView = newView;
}

void CLightView::SetProjection( DirectX::XMMATRIX& newProjection )
{
	this->LightProjection = newProjection;
}

void CLightView::SetPerspective( float FOV, float camNear, float camFar, float WindowWidth, float WindowHeight )
{
	this->FOV = FOV;
	this->NearPlane = camNear;
	this->FarPlane = camFar;
	this->WindowWidth = WindowWidth;
	this->WindowHeight = WindowHeight;
}

void CLightView::SetFocus( DirectX::XMFLOAT3 Focus )
{
	this->Focus = Focus;
}

void CLightView::SetPosition( DirectX::XMFLOAT3 Position )
{
	this->Position = Position;
}

void CLightView::SetAmbient( common::Color Ambient )
{
	this->Ambient = Ambient;
}

void CLightView::SetDiffuse( common::Color Diffuse )
{
	this->Diffuse = Diffuse;
}

DirectX::XMMATRIX& CLightView::GetView( )
{
	return LightView;
}

DirectX::XMMATRIX& CLightView::GetProjection( )
{
	return LightProjection;
}

DirectX::XMFLOAT3 CLightView::GetPosition( )
{
	return Position;
}

DirectX::XMFLOAT3 CLightView::GetFocus( )
{
	return Focus;
}

float CLightView::GetFOV( )
{
	return FOV;
}

float CLightView::GetNearPlane( )
{
	return NearPlane;
}

float CLightView::GetFarPlane( )
{
	return FarPlane;
}

common::Color& CLightView::GetDiffuse( )
{
	return Diffuse;
}

common::Color& CLightView::GetAmbient( )
{
	return Ambient;
}

void CLightView::Shutdown( )
{

}

void* CLightView::operator new( size_t size )
{
	return _mm_malloc( size, 16 );
}

void CLightView::operator delete( void* obj )
{
	_mm_free( obj );
}

CLightView::~CLightView( )
{
	Shutdown( );
}
