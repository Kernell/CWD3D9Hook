#include <list>
#include <vector>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9core.h>
#include "detours.h"
#include <assert.h>

#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")

typedef HRESULT( WINAPI* DrawIndexedPrimitive )( LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, int, UINT, UINT, UINT, UINT );
typedef HRESULT( WINAPI* EndScene )( LPDIRECT3DDEVICE9 );
typedef HRESULT( WINAPI* Reset )( LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS* );

#define clamp( dMin, a, dMax ) \
	max( min( a, dMax ), dMin )

#define lerp( from, to, percent ) \
	from + ( to - from ) * clamp( 0.0, percent, 1.0 )

#define VSNPRINTF( buffer, count, format, argptr ) \
{ \
	int iResult = vsnprintf( buffer, count, format, argptr ); \
	if( iResult == -1 || iResult == ( count ) ) \
		( buffer )[ (count)-1 ] = 0; \
}
