#pragma once

#define VSYNC

#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _XM_NO_INTRINSICS_

#ifdef INLINE_SAFE_RELEASE
template <class type> inline void SafeRelease( type Object )
{
	if (Object)
	{
		Object->Release();
		Object = nullptr;
	}
}
#else
#define SafeRelease(Object) if ( (Object) ) { (Object)->Release(); (Object) = nullptr; }
#endif // INLINE_SAFE_RELEASE


#ifdef ERRORS_ON_SCREEN
#define IFFAILED(expr, msg) if ( FAILED( (expr) ) ) { MessageBox(NULL, (msg), L"Error", MB_OK | MB_ICONERROR ); }
#else // !ERRORS_ON_SCREEN
#define IFFAILED(expr, msg) if (FAILED( (expr) )) { OutputDebugString( (msg) ); return false; }
#endif // ERRORS_ON_SCREEN

#define FLOAT_PI 3.14159265359f
#define DEG2RAD(Angle) Angle * (FLOAT_PI / 180.0f)
#define RAD2DEG(Rad) Rad * (180.0f / FLOAT_PI)
#define ZeroMemoryAndDeclare(Type, Name)	Type Name;\
											ZeroMemory(&Name,sizeof(Type));

namespace common
{
	struct Color
	{
		float r;
		float g;
		float b;
		float a;
		inline operator float* ()
		{
			return (float*)&r;
		}
		Color() : r( 0.0f ), g( 0.0f ), b( 0.0f ), a( 0.0f ) {};
		Color( float r, float g, float b, float a )
			:r( r ), g( g ), b( b ), a( a ) {};
	};

	Color HexToRGB( int color );
}
