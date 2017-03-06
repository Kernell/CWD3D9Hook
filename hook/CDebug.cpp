#include "stdafx.h"
#include "CDebug.h"

CDebug::CDebug( CHook* pHook )
{
	this->m_pHook			= pHook;

	this->m_bVisible		= false;

	this->m_pFont = NULL;
}

CDebug::~CDebug()
{
	if( this->m_pFont )
	{
		this->m_pFont->Release();
		this->m_pFont = NULL;
	}

	this->m_pHook = NULL;
}

void CDebug::DoPulse( void )
{
	if( GetAsyncKeyState( VK_NUMPAD1 ) )
	{
		this->m_bVisible = !this->m_bVisible;
	}
}

HRESULT WINAPI CDebug::OnRenderPrimitive( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
{
	if( g_pHook->m_bScreenShot )
	{
		return -1;
	}

	return -1;
}

HRESULT WINAPI CDebug::OnRender( LPDIRECT3DDEVICE9 pDev )
{
	if( g_pHook->m_bScreenShot )
	{
		return -1;
	}

	if( this->m_bVisible )
	{
		D3DVIEWPORT9 pViewPort;

		if( SUCCEEDED( pDev->GetViewport( &pViewPort ) ) )
		{
			if( this->m_pFont == NULL )
			{
				D3DXCreateFontA( pDev, 12, 0, 200, 0, 0, 1, 0, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas", &this->m_pFont );
			}

			if( this->m_pFont )
			{
				RECT pRect =
				{
					10,
					10,
					pViewPort.Width,
					pViewPort.Height
				};

				if( this->m_List.size() > 0 )
				{
					list< string >::const_iterator iter = this->m_List.begin();

					for( ; iter != this->m_List.end(); iter++ )
					{
						pRect.top += 16;

						this->m_pFont->DrawTextA( 0, iter->c_str(), -1, &pRect, DT_LEFT | DT_TOP | DT_NOCLIP, 0xFFFF0000 );
					}
				}
			}
		}
	}

	return -1;
}

HRESULT WINAPI CDebug::OnReset( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP )
{
	if( this->m_pFont )
	{
		this->m_pFont->Release();
		this->m_pFont = NULL;
	}

	return -1;
}

void CDebug::Printf( const char* szFormat, ... )
{
    char szBuffer[ 1024 ];

    va_list ap;

    va_start( ap, szFormat );

    VSNPRINTF( szBuffer, 1024, szFormat, ap );

    va_end( ap );

    this->Print( szBuffer );
}

void CDebug::Print( const char* szText )
{
	this->m_List.push_back( szText );
}