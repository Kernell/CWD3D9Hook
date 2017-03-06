#include "stdafx.h"
#include "CDetect.h"

extern CDetect *g_pDetect;

CDetect::CDetect()
{
	HWND hWnd		= CreateWindowExA( 0, "STATIC", "dummy", 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	HMODULE hD3D9	= GetModuleHandleA( "d3d9.dll" );

	PBYTE
		hD3D9_EndScene	= NULL,
		hD3D9_Reset		= NULL;

	bool bSuccess = false;

	if( hD3D9 != NULL )
	{
		D3DPRESENT_PARAMETERS	d3dpp;
		IDirect3DDevice9		*pD3D9Device;

		IDirect3D9 *pD3D9	= Direct3DCreate9( D3D_SDK_VERSION );

		ZeroMemory( &d3dpp, sizeof( d3dpp ) );

		d3dpp.Windowed		= 1;
		d3dpp.SwapEffect	= D3DSWAPEFFECT_DISCARD;

		pD3D9->CreateDevice( 0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pD3D9Device );

		DWORD *vtable		= (PDWORD)( *( (PDWORD)pD3D9Device ) );

		hD3D9_EndScene		= (PBYTE)( (DWORD)hD3D9 + ( vtable[ 42 ] - (DWORD)hD3D9 ) ),
		hD3D9_Reset			= (PBYTE)( (DWORD)hD3D9 + ( vtable[ 16 ] - (DWORD)hD3D9 ) );

		pD3D9Device->Release();
		pD3D9->Release();

		bSuccess = true;
	}

	if( bSuccess )
	{
		this->m_pFont		= NULL;

		this->m_pEndScene		= reinterpret_cast< EndScene >( DetourFunction( hD3D9_EndScene, reinterpret_cast< PBYTE >( CDetect::OnRender ) ) );
		this->m_pReset			= reinterpret_cast< Reset >( DetourFunction( hD3D9_Reset, reinterpret_cast< PBYTE >( CDetect::OnReset ) ) );

		this->m_pLdrLoadDLL = NULL;

		if( !this->InstallHook() )
		{
			MessageBoxA( hWnd, "Failed to install hook", "Error", MB_OK | MB_ICONERROR );
		}
	}

	CloseHandle( hWnd );
}

CDetect::~CDetect()
{
	DetourRemove( reinterpret_cast< PBYTE >( this->m_pEndScene ),		reinterpret_cast< PBYTE >( CDetect::OnRender ) );
	DetourRemove( reinterpret_cast< PBYTE >( this->m_pReset ),			reinterpret_cast< PBYTE >( CDetect::OnReset ) );

	if( this->m_pFont )
	{
		this->m_pFont->Release();
		this->m_pFont = NULL;
	}
}

void CDetect::DoPulse()
{
}

bool CDetect::InstallHook()
{
	LPVOID lpLdrLoadDLLAddr = GetProcAddress( GetModuleHandleA( "ntdll.dll" ), "LdrLoadDll" );

	if( lpLdrLoadDLLAddr )
	{
		this->m_pLdrLoadDLL = (DWORD)lpLdrLoadDLLAddr + 5;

		DWORD ulOldProtect = { NULL };

		VirtualProtect( lpLdrLoadDLLAddr, 5, PAGE_EXECUTE_READWRITE, &ulOldProtect );

		*(BYTE*)( lpLdrLoadDLLAddr ) = 0xE9;
		*(DWORD*)( (LPBYTE)lpLdrLoadDLLAddr + 1 ) = ( (DWORD)CDetect::LdrLoadDLL - ( (DWORD)lpLdrLoadDLLAddr + 5 ) );

		return true;
	}

	return false;
}

HRESULT WINAPI CDetect::OnRender( LPDIRECT3DDEVICE9 pDev )
{
	if( SUCCEEDED( pDev->GetViewport( &g_pDetect->m_pViewPort ) ) )
	{
		if( g_pDetect->m_pFont == NULL )
		{
			D3DXCreateFontA( pDev, 12, 0, 200, 0, 0, 1, 0, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas", &g_pDetect->m_pFont );
		}

		if( g_pDetect->m_pFont )
		{
			RECT pRect =
			{
				10,
				10,
				g_pDetect->m_pViewPort.Width,
				g_pDetect->m_pViewPort.Height
			};

			g_pDetect->m_pFont->DrawTextA( 0, "Loaded Libs:", -1, &pRect, DT_LEFT | DT_TOP | DT_NOCLIP, 0xFFFF0000 );

			if( g_pDetect->m_List.size() > 0 )
			{
				list< string >::const_iterator iter = g_pDetect->m_List.begin();

				for( ; iter != g_pDetect->m_List.end(); iter++ )
				{
					pRect.top += 16;

					g_pDetect->m_pFont->DrawTextA( 0, iter->c_str(), -1, &pRect, DT_LEFT | DT_TOP | DT_NOCLIP, 0xFFFF0000 );
				}
			}
		}
	}

	return g_pDetect->m_pEndScene( pDev );
}

HRESULT WINAPI CDetect::OnReset( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP )
{
	if( g_pDetect->m_pFont )
	{
		g_pDetect->m_pFont->Release();
		g_pDetect->m_pFont = NULL;
	}

	return g_pDetect->m_pReset( pDev, PresP );
}

DWORD CDetect::LdrLoadDLL( IN PWCHAR pathToFile, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE hModule )
{
	DWORD ulLdrLoadDLL = g_pDetect->m_pLdrLoadDLL;

	__asm pushad

	char szDllName[ 1000 ] = { NULL };

	wcstombs( szDllName, ModuleFileName->Buffer, wcslen( ModuleFileName->Buffer ) );

	string strDllPath = (string)szDllName;

	g_pDetect->m_List.push_back( strDllPath );

	__asm popad
	__asm jmp [ulLdrLoadDLL]

	return 1;
}