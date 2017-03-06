#include "stdafx.h"
#include "CWallHack.h"

#define BEAR_NUM_VERTICES 1201
#define USEC_NUM_VERTICES 1135
#define GRENADE_NUM_VERTICES 1766

#define BEAR_PRIM_COUNT 1279
#define USEC_PRIM_COUNT 1305
#define GRENADE_PRIM_COUNT 1842

CWallHack::CWallHack( CHook* pHook )
{
	this->m_pHook = pHook;
	this->m_pFont = NULL;
	this->m_bEnabled = false;

	this->m_iStartRegister = 0;
}

CWallHack::~CWallHack()
{
	if( this->m_pFont )
	{
		this->m_pFont->Release();
		this->m_pFont = NULL;
	}

	this->m_pHook = NULL;
}

void CWallHack::DoPulse( void )
{
	if( GetAsyncKeyState( VK_NUMPAD0 ) )
	{
		this->m_bEnabled = !this->m_bEnabled;
	}

	if( this->m_bEnabled && GetAsyncKeyState( VK_SHIFT ) )
	{
		if( GetAsyncKeyState( VK_UP ) )
		{
			this->m_iStartRegister++;

			g_pHook->GetDebug()->Printf( "CWallHack::m_uiStartRegister = %d", this->m_iStartRegister );
		}
		else if( GetAsyncKeyState( VK_DOWN ) )
		{
			if( --this->m_iStartRegister < 0 )
			{
				this->m_iStartRegister = 0;
			}

			g_pHook->GetDebug()->Printf( "CWallHack::m_uiStartRegister = %d", this->m_iStartRegister );
		}
	}
}

HRESULT WINAPI CWallHack::OnRenderPrimitive( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
{
	if( g_pHook->m_bScreenShot )
	{
		return -1;
	}

	if( !this->m_bEnabled )
	{
		return -1;
	}

	LPDIRECT3DVERTEXBUFFER9 Stream_Data;

	unsigned int uiStrideIndex = 0;
	unsigned int uiOffset = 0;

	if( pDev->GetStreamSource( NULL, &Stream_Data, &uiOffset, &uiStrideIndex ) == S_OK )
	{
		Stream_Data->Release();
	}

	if( uiStrideIndex == 40 || uiStrideIndex == 56 )
	{
		if( ( NumVertices == BEAR_NUM_VERTICES && primCount == BEAR_PRIM_COUNT )
			|| ( NumVertices == USEC_NUM_VERTICES && primCount == USEC_PRIM_COUNT )
			|| ( NumVertices == GRENADE_NUM_VERTICES && primCount == GRENADE_PRIM_COUNT ) )
		{
			D3DVIEWPORT9 pViewPort;

			if( SUCCEEDED( pDev->GetViewport( &pViewPort ) ) )
			{
				{
					D3DMATRIX pMatrix;

					if( FAILED( pDev->GetVertexShaderConstantF( 1, (float *)&pMatrix, 4 ) ) )
					{
						return -1;
					}

					D3DXVECTOR4	vecPosition;

					Matrix2Position( pMatrix, vecPosition );

					if( vecPosition.w <= 1.0f )
					{
						return -1;
					}
				}

				D3DMATRIX pMatrix;

				if( FAILED( pDev->GetVertexShaderConstantF( this->m_iStartRegister, (float *)&pMatrix, 4 ) ) )
				{
					return -1;
				}

				D3DXVECTOR4	vecPosition;

				Matrix2Position( pMatrix, vecPosition );

				if( vecPosition.w > 1.0f && vecPosition.w < 200.0f )
				{
					if( primCount == GRENADE_PRIM_COUNT )
					{
						Grenade pGrenade;

						pGrenade.X = vecPosition.x;
						pGrenade.Y = vecPosition.y;
						pGrenade.Z = vecPosition.z;
						pGrenade.W = vecPosition.w;

						this->m_Grenades.push_back( pGrenade );
					}
					else
					{
						unsigned int uiAlpha = lerp( 255, 100, vecPosition.w * 0.015 );

						char szStringBuffer[ 64 ];

						sprintf_s( szStringBuffer, "%0.2f\n", vecPosition.w );

						DebugInfo pDebugInfo;

						pDebugInfo.X = vecPosition.x;
						pDebugInfo.Y = vecPosition.y;
						pDebugInfo.Z = vecPosition.z;
						pDebugInfo.W = vecPosition.w;
						pDebugInfo.String = szStringBuffer;
						pDebugInfo.Color = NumVertices != 1201 ? D3DCOLOR_ARGB( uiAlpha, 255, 0, 0 ) : D3DCOLOR_ARGB( uiAlpha, 0, 128, 255 );

						this->m_Debug.push_back( pDebugInfo );
					}
				}
			}
		}
	}

	return -1;
}

HRESULT WINAPI CWallHack::OnRender( LPDIRECT3DDEVICE9 pDev )
{
	if( g_pHook->m_bScreenShot )
	{
		return -1;
	}

	if( !this->m_bEnabled )
	{
		return -1;
	}

	D3DVIEWPORT9 pViewPort;

	if( SUCCEEDED( pDev->GetViewport( &pViewPort ) ) )
	{
		if( this->m_pFont == NULL )
		{
			D3DXCreateFontA( pDev, 12, 0, 200, 0, 0, 1, 0, CLEARTYPE_NATURAL_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas", &this->m_pFont );
		}

		if( this->m_pFont )
		{
			if( this->m_Debug.size() > 0 )
			{
				list< DebugInfo >::const_iterator iter = this->m_Debug.begin();

				for( ; iter != this->m_Debug.end(); iter++ )
				{
					float fX = ( ( iter->X / iter->W ) * ( pViewPort.Width / 2 ) ) + pViewPort.X + ( pViewPort.Width / 2 );
					float fY = pViewPort.Y + ( pViewPort.Height / 2 ) - ( ( -iter->Y / iter->W ) * ( pViewPort.Height / 2 ) );

					RECT pRect =
					{
						(long)fX,
						(long)fY,
						pViewPort.Width,
						pViewPort.Height
					};

					this->m_pFont->DrawTextA( 0, iter->String.c_str(), -1, &pRect, DT_LEFT | DT_TOP | DT_NOCLIP, iter->Color );
				}

				this->m_Debug.clear();
			}

			if( this->m_Grenades.size() > 0 )
			{
				list< Grenade >::const_iterator iter = this->m_Grenades.begin();

				for( ; iter != this->m_Grenades.end(); iter++ )
				{
					if( GetTickCount() % 200 < 100 )
					{
						float fX = ( ( iter->X / iter->W ) * ( pViewPort.Width / 2 ) ) + pViewPort.X + ( pViewPort.Width / 2 );
						float fY = pViewPort.Y + ( pViewPort.Height / 2 ) - ( ( -iter->Y / iter->W ) * ( pViewPort.Height / 2 ) );

						RECT pRect =
						{
							(long)fX,
							(long)fY,
							pViewPort.Width,
							pViewPort.Height
						};

						this->m_pFont->DrawTextA( 0, "!!!", -1, &pRect, DT_LEFT | DT_TOP | DT_NOCLIP, D3DCOLOR_ARGB( 255, 255, 128, 0 ) );
					}
				}

				this->m_Grenades.clear();
			}
		}
	}

	return -1;
}

HRESULT WINAPI CWallHack::OnReset( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP )
{
	if( this->m_pFont )
	{
		this->m_pFont->Release();
		this->m_pFont = NULL;
	}

	return -1;
}