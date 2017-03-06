#include "stdafx.h"
#include "CScreenShot.h"

CScreenShot::CScreenShot( CHook* pHook )
{
	this->m_pHook			= pHook;

	this->m_lLastSaveTime	= 0;
	this->m_bIsSaving		= false;
}

CScreenShot::~CScreenShot()
{
	if( this->m_pSurface )
	{
		this->m_pSurface->Release();
		this->m_pSurface = NULL;
	}
	
	if( this->m_pRTSurface )
	{
		this->m_pRTSurface->Release();
		this->m_pRTSurface = NULL;
	}
}

void CScreenShot::DoPulse( void )
{
	if( GetAsyncKeyState( VK_F4 ) )
	{
		g_pHook->m_bScreenShot = true;
	}
}

HRESULT WINAPI CScreenShot::OnRenderPrimitive( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
{
	return -1;
}

HRESULT WINAPI CScreenShot::OnRender( LPDIRECT3DDEVICE9 pDev )
{
	if( this->CheckForScreenShot( pDev ) )
	{
		g_pHook->m_bScreenShot = false;
	}

	return -1;
}

HRESULT WINAPI CScreenShot::OnReset( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP )
{
	if( this->m_pSurface )
	{
		this->m_pSurface->Release();
		this->m_pSurface = NULL;
	}
	
	if( this->m_pRTSurface )
	{
		this->m_pRTSurface->Release();
		this->m_pRTSurface = NULL;
	}

	return -1;
}

bool CScreenShot::CheckForScreenShot( LPDIRECT3DDEVICE9 pDev )
{
    if( g_pHook->m_bScreenShot && !this->IsSaving() )
    {
		if( GetTickCount64() - this->m_lLastSaveTime < 1000 )
		{
            return false;
		}

		this->m_lLastSaveTime = GetTickCount64();

		D3DVIEWPORT9 pViewPort;

		if( FAILED( pDev->GetViewport( &pViewPort ) ) )
		{
			g_pHook->GetDebug()->Print( "GetViewport failed" );

			return true;
		}

		HRESULT iRes;

		if( ( iRes = pDev->CreateOffscreenPlainSurface( pViewPort.Width, pViewPort.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &this->m_pSurface, NULL ) ) != D3D_OK )
		{
			g_pHook->GetDebug()->Printf( "Error: CreateOffscreenPlainSurface failed = 0x%x", iRes );

			return true;
		}

		if( ( iRes = pDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &this->m_pRTSurface ) ) != D3D_OK )
		{
			g_pHook->GetDebug()->Printf( "Error: GetBackBuffer failed = 0x%x", iRes );

			return true;
		}

		if( ( iRes = pDev->GetRenderTargetData( this->m_pRTSurface, this->m_pSurface ) ) != D3D_OK )
		{
			g_pHook->GetDebug()->Printf( "Error: GetRenderTargetData failed = 0x%x", iRes );

			return true;
		}

		string strFileName = this->GetValidScreenshotFilename();

		if( SUCCEEDED( this->BeginSave( strFileName.c_str(), this->m_pSurface, pViewPort.Width, pViewPort.Height ) ) )
		{
			g_pHook->GetDebug()->Printf( "Screenshot saved: %s", strFileName.c_str() );
		}
		else
		{
			g_pHook->GetDebug()->Printf( "Error: CScreenShot::BeginSave( %s ) failed", strFileName.c_str() );
		}

		return true;
    }

	return false;
}

HRESULT CScreenShot::BeginSave( const char *szFileName, IDirect3DSurface9* pData, unsigned int uiWidth, unsigned int uiHeight )
{
	if( this->m_bIsSaving )
	{
        return -1;
	}

    this->m_bIsSaving = true;

	HRESULT pResult = D3DXSaveSurfaceToFileA( szFileName, D3DXIFF_BMP, pData, NULL, NULL );

    this->m_bIsSaving = false;

	return pResult;
}

string CScreenShot::GetValidScreenshotFilename( void )
{
    SYSTEMTIME sysTime;

    GetLocalTime( &sysTime );

	char szBuffer[ MAX_PATH ];

    GetModuleFileNameA( g_hModule, szBuffer, MAX_PATH );
    
	string strPath = string( szBuffer ).substr( 0, string( szBuffer ).find_last_of( "\\/" ) );

	char szStringBuffer[ 512 ];

	sprintf_s( szStringBuffer, "%s\\screen_%d-%02d-%02d_%02d-%02d-%02d.bmp", strPath.c_str(), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond );

    return string( szStringBuffer );
}

bool CScreenShot::IsSaving( void )
{
	return this->m_bIsSaving;
}