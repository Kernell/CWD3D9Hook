#include "stdafx.h"
#include "CHook.h"

CHook::CHook()
{
	g_pHook = this;

	this->m_bHookInstalled				= false;

	this->m_bScreenShot					= false;

	this->DrawIndexedPrimitive			= NULL;
	this->EndScene						= NULL;
	this->Reset							= NULL;

	this->ApplyHook();
}

CHook::~CHook()
{
	this->RemoveHook();

	g_pHook = NULL;
}

void CHook::ApplyHook()
{
	if( this->m_bHookInstalled )
	{
		return;
	}

	HMODULE hD3D9	= GetModuleHandleA( "d3d9.dll" );

	PBYTE
		hD3D9_DIP		= NULL,
		hD3D9_EndScene	= NULL,
		hD3D9_Reset		= NULL;

	if( hD3D9 )
	{
		HWND hWnd		= CreateWindowExA( 0, "STATIC", "CWD3D9", 0, 0, 0, 0, 0, 0, 0, 0, 0 );

		D3DPRESENT_PARAMETERS	d3dpp;
		IDirect3DDevice9		*pD3D9Device;

		IDirect3D9 *pD3D9	= Direct3DCreate9( D3D_SDK_VERSION );

		assert( pD3D9 );

		ZeroMemory( &d3dpp, sizeof( d3dpp ) );

		d3dpp.Windowed		= 1;
		d3dpp.SwapEffect	= D3DSWAPEFFECT_DISCARD;

		if( SUCCEEDED( pD3D9->CreateDevice( 0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pD3D9Device ) ) )
		{
			DWORD *vtable		= (PDWORD)( *( (PDWORD)pD3D9Device ) );

			hD3D9_DIP			= (PBYTE)( (DWORD)hD3D9 + ( vtable[ 82 ] - (DWORD)hD3D9 ) ),
			hD3D9_EndScene		= (PBYTE)( (DWORD)hD3D9 + ( vtable[ 42 ] - (DWORD)hD3D9 ) ),
			hD3D9_Reset			= (PBYTE)( (DWORD)hD3D9 + ( vtable[ 16 ] - (DWORD)hD3D9 ) );

			pD3D9Device->Release();
		}

		pD3D9->Release();

		CloseHandle( hWnd );
	}

	if( hD3D9_DIP && hD3D9_EndScene && hD3D9_Reset )
	{
#if DETOURS_VERSION >= 30000
		this->DrawIndexedPrimitive			= (::DrawIndexedPrimitive)hD3D9_DIP;
		this->EndScene						= (::EndScene)hD3D9_EndScene;
		this->Reset							= (::Reset)hD3D9_Reset;

		DetourTransactionBegin();

		DetourAttach( (PVOID*)(&hD3D9_DIP), OnRenderPrimitive );
		DetourAttach( (PVOID*)(&hD3D9_EndScene), OnRender );
		DetourAttach( (PVOID*)(&hD3D9_Reset), OnReset );

		DetourTransactionCommit();
#else
		if( !this->DrawIndexedPrimitive )
			this->DrawIndexedPrimitive		= reinterpret_cast< ::DrawIndexedPrimitive >( DetourFunction( hD3D9_DIP, reinterpret_cast< PBYTE >( OnRenderPrimitive ) ) );

		if( !this->EndScene )
			this->EndScene					= reinterpret_cast< ::EndScene >( DetourFunction( hD3D9_EndScene, reinterpret_cast< PBYTE >( OnRender ) ) );

		if( !this->Reset )
			this->Reset						= reinterpret_cast< ::Reset >( DetourFunction( hD3D9_Reset, reinterpret_cast< PBYTE >( OnReset ) ) );
#endif
		this->m_pDebug						= new CDebug( this );
		this->m_pScreenShot					= new CScreenShot( this );
		this->m_pStrideLogger				= new CStrideLogger( this );
		this->m_pWallHack					= new CWallHack( this );
		this->m_pAimBot						= new CAimBot( this );
		this->m_pCrosshair					= new CCrosshair( this );

		this->m_bHookInstalled				= true;
	}
}

void CHook::RemoveHook()
{
	if( !this->m_bHookInstalled )
	{
		return;
	}

	this->m_bHookInstalled = false;

#if DETOURS_VERSION >= 30000
	DetourDetach( (PVOID*)(&this->DrawIndexedPrimitive),	OnRenderPrimitive );
	DetourDetach( (PVOID*)(&this->EndScene),				OnRender );
	DetourDetach( (PVOID*)(&this->Reset),					OnReset );
#else
	DetourRemove( reinterpret_cast< PBYTE >( this->DrawIndexedPrimitive ),	reinterpret_cast< PBYTE >( OnRenderPrimitive ) );
	DetourRemove( reinterpret_cast< PBYTE >( this->EndScene ),				reinterpret_cast< PBYTE >( OnRender ) );
	DetourRemove( reinterpret_cast< PBYTE >( this->Reset ),					reinterpret_cast< PBYTE >( OnReset ) );
#endif

	this->DrawIndexedPrimitive			= NULL;
	this->EndScene						= NULL;
	this->Reset							= NULL;
	
	if( this->m_pDebug )
	{
		delete this->m_pDebug;

		this->m_pDebug = NULL;
	}

	if( this->m_pScreenShot )
	{
		delete this->m_pScreenShot;
		
		this->m_pScreenShot = NULL;
	}

	if( this->m_pStrideLogger )
	{
		delete this->m_pStrideLogger;
		
		this->m_pStrideLogger = NULL;
	}

	if( this->m_pWallHack )
	{
		delete this->m_pWallHack;

		this->m_pWallHack = NULL;
	}

	if( this->m_pAimBot )
	{
		delete this->m_pAimBot;

		this->m_pAimBot = NULL;
	}

	if( this->m_pCrosshair )
	{
		delete this->m_pCrosshair;

		this->m_pCrosshair = NULL;
	}
}

void CHook::DoPulse( void )
{
	if( GetAsyncKeyState( VK_CONTROL ) )
	{
		if( GetAsyncKeyState( VK_F1 ) )
		{
			delete this;

			return;
		}
		else if( GetAsyncKeyState( VK_F2 ) )
		{
			this->ApplyHook();

			return;
		}
		else if( GetAsyncKeyState( VK_F3 ) )
		{
			this->RemoveHook();

			return;
		}
	}
	
	if( !this->m_bHookInstalled )
	{
		return;
	}

	if( this->m_pDebug )
	{
		this->m_pDebug->DoPulse();
	}

	if( this->m_pScreenShot )
	{
		this->m_pScreenShot->DoPulse();
	}

	if( this->m_pStrideLogger )
	{
		this->m_pStrideLogger->DoPulse();
	}

	if( this->m_pWallHack )
	{
		this->m_pWallHack->DoPulse();
	}

	if( this->m_pAimBot )
	{
		this->m_pAimBot->DoPulse();
	}

	if( this->m_pCrosshair )
	{
		this->m_pCrosshair->DoPulse();
	}
}

HRESULT WINAPI CHook::OnRenderPrimitive( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
{
	HRESULT hResult = -1;
	
	if( g_pHook->m_pDebug )
	{
		hResult = g_pHook->m_pDebug->OnRenderPrimitive( pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
	}

	if( g_pHook->m_pScreenShot )
	{
		hResult = g_pHook->m_pScreenShot->OnRenderPrimitive( pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
	}

	if( g_pHook->m_pStrideLogger )
	{
		hResult = g_pHook->m_pStrideLogger->OnRenderPrimitive( pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
	}

	if( g_pHook->m_pWallHack )
	{
		g_pHook->m_pWallHack->OnRenderPrimitive( pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
	}

	if( g_pHook->m_pAimBot )
	{
		g_pHook->m_pAimBot->OnRenderPrimitive( pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
	}
#if 0
	if( g_pHook->m_pCrosshair )
	{
		g_pHook->m_pCrosshair->OnRenderPrimitive( pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
	}
#endif
	if( FAILED( hResult ) )
	{
		hResult = g_pHook->DrawIndexedPrimitive( pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
	}
	
	return hResult;
}

HRESULT WINAPI CHook::OnRender( LPDIRECT3DDEVICE9 pDev )
{
	if( g_pHook->m_pDebug )
	{
		g_pHook->m_pDebug->OnRender( pDev );
	}

	if( g_pHook->m_pScreenShot )
	{
		g_pHook->m_pScreenShot->OnRender( pDev );
	}

	if( g_pHook->m_pStrideLogger )
	{
		g_pHook->m_pStrideLogger->OnRender( pDev );
	}
	
	if( g_pHook->m_pWallHack )
	{
		g_pHook->m_pWallHack->OnRender( pDev );
	}

	if( g_pHook->m_pAimBot )
	{
		g_pHook->m_pAimBot->OnRender( pDev );
	}

	if( g_pHook->m_pCrosshair )
	{
		g_pHook->m_pCrosshair->OnRender( pDev );
	}

	return g_pHook->EndScene( pDev );
}

HRESULT WINAPI CHook::OnReset( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP )
{
	if( g_pHook->m_pDebug )
	{
		g_pHook->m_pDebug->OnReset( pDev, PresP );
	}

	if( g_pHook->m_pScreenShot )
	{
		g_pHook->m_pScreenShot->OnReset( pDev, PresP );
	}

	if( g_pHook->m_pStrideLogger )
	{
		g_pHook->m_pStrideLogger->OnReset( pDev, PresP );
	}

	if( g_pHook->m_pWallHack )
	{
		g_pHook->m_pWallHack->OnReset( pDev, PresP );
	}

	if( g_pHook->m_pAimBot )
	{
		g_pHook->m_pAimBot->OnReset( pDev, PresP );
	}

	if( g_pHook->m_pCrosshair )
	{
		g_pHook->m_pCrosshair->OnReset( pDev, PresP );
	}

	return g_pHook->Reset( pDev, PresP );
}