#include "stdafx.h"
#include "CAimBot.h"

CAimBot::CAimBot( CHook* pHook )
{
	this->m_pHook		= pHook;
	this->m_bEnabled	= false;
	this->m_pTarget		= NULL;

	this->m_bTargetBEAR	= false;
	this->m_bTargetUSEC	= false;

	this->m_pFont		= NULL;
}

CAimBot::~CAimBot()
{
	this->m_pHook		= NULL;

	if( this->m_pTarget )
	{
		delete this->m_pTarget;

		this->m_pTarget = NULL;
	}

	if( this->m_pFont )
	{
		delete this->m_pFont;

		this->m_pFont = NULL;
	}
}

void CAimBot::DoPulse( void )
{
	if( GetAsyncKeyState( VK_NUMPAD3 ) )
	{
		this->m_bEnabled = !this->m_bEnabled;

		g_pHook->GetDebug()->Printf( "CAimBot::m_bEnabled = %s", this->m_bEnabled ? "true" : "false" );
	}

	if( this->m_bEnabled )
	{
		if( GetAsyncKeyState( VK_NUMPAD7 ) )
		{
			this->m_bTargetBEAR = !this->m_bTargetBEAR;

			g_pHook->GetDebug()->Printf( "CAimBot::m_bTargetBEAR = %s", this->m_bTargetBEAR ? "true" : "false" );
		}

		if( GetAsyncKeyState( VK_NUMPAD9 ) )
		{
			this->m_bTargetUSEC = !this->m_bTargetUSEC;

			g_pHook->GetDebug()->Printf( "CAimBot::m_bTargetUSEC = %s", this->m_bTargetUSEC ? "true" : "false" );
		}
	}
}

HRESULT WINAPI CAimBot::OnRenderPrimitive( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
{
	if( g_pHook->m_bScreenShot )
	{
		return -1;
	}

	if( !this->m_bEnabled )
	{
		return -1;
	}

	LPDIRECT3DVERTEXBUFFER9 pStreamData;

	unsigned int uiStrideIndex	= 0;
	unsigned int uiOffset		= 0;
		
	if( pDev->GetStreamSource( NULL, &pStreamData, &uiOffset, &uiStrideIndex ) == S_OK )
	{
		pStreamData->Release();
	}

	if( uiStrideIndex != 40 )
	{
		return -1;
	}

	if( this->IsValidTarget( NumVertices, primCount ) )
	{
		D3DVIEWPORT9 pViewPort;

		if( FAILED( pDev->GetViewport( &pViewPort ) ) )
		{
			return -1;
		}

		D3DMATRIX pMatrix;

		if( FAILED( pDev->GetVertexShaderConstantF( 0, (float *)&pMatrix, 4 ) ) )
		{
			return -1;
		}

		D3DXVECTOR4	vecPosition;
		D3DXVECTOR4	input;

		input.x = 0.0f;
		input.y = 0.0f;
		input.z = 0.0f;
		input.w = 1.0f;

		D3DXMatrixTranspose( (D3DXMATRIX *)&pMatrix, (D3DXMATRIX *)&pMatrix );
		
		vecPosition.x = input.x * pMatrix._11 + input.y * pMatrix._21 + input.z * pMatrix._31 + pMatrix._41;
		vecPosition.y = input.x * pMatrix._12 + input.y * pMatrix._22 + input.z * pMatrix._32 + pMatrix._42;
		vecPosition.z = input.x * pMatrix._13 + input.y * pMatrix._23 + input.z * pMatrix._33 + pMatrix._43;
		vecPosition.w = input.x * pMatrix._14 + input.y * pMatrix._24 + input.z * pMatrix._34 + pMatrix._44;
		
		if( vecPosition.w > 1.0f )
		{
			float fX = ( ( vecPosition.x / vecPosition.w ) * ( pViewPort.Width / 2 ) ) + pViewPort.X + ( pViewPort.Width / 2 );
			float fY = pViewPort.Y + ( pViewPort.Height / 2 ) - ( ( -vecPosition.y / vecPosition.w ) * ( pViewPort.Height / 2 ) );
			
			CTarget* pTarget = new CTarget();
			
			pTarget->Position		= new D3DXVECTOR4( fX, fY, 0.0f, vecPosition.w );
			pTarget->NumVertices	= NumVertices;
			pTarget->PrimCount		= primCount;
			
			if( this->m_pTarget == NULL || pTarget->GetDistance( pViewPort ) < this->m_pTarget->GetDistance( pViewPort ) )
			{
				if( this->m_pTarget )
				{
					delete this->m_pTarget;
					
					this->m_pTarget = NULL;
				}
				
				this->m_pTarget = pTarget;
			}
		}
	}
	
	return -1;
}

HRESULT WINAPI CAimBot::OnRender( LPDIRECT3DDEVICE9 pDev )
{
	if( g_pHook->m_bScreenShot )
	{
		return -1;
	}

	if( !this->m_bEnabled )
	{
		return -1;
	}

	if( this->m_pFont == NULL )
	{
		D3DXCreateFontA( pDev, 12, 0, 200, 0, 0, 1, 0, CLEARTYPE_NATURAL_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas", &this->m_pFont );
	}

	if( this->m_pTarget )
	{
		if( GetAsyncKeyState( VK_SHIFT ) )
		{
			D3DVIEWPORT9 pViewPort;

			if( FAILED( pDev->GetViewport( &pViewPort ) ) )
			{
				return -1;
			}

			float fX = this->m_pTarget->Position->x;
			float fY = this->m_pTarget->Position->y;

			//fX -= pViewPort.Width / 2.0f;
			//fY -= pViewPort.Height / 2.0f;

			//mouse_event( MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, fX, fY, NULL, NULL );

			//SetCursorPos( fX, fY );

			INPUT pInput;

			pInput.type				= INPUT_MOUSE;
			pInput.mi.dx			= ( 65535 * ( fX + 1 ) ) / GetSystemMetrics( SM_CXSCREEN );
			pInput.mi.dy			= ( 65535 * ( fY + 1 ) ) / GetSystemMetrics( SM_CYSCREEN );
			pInput.mi.dwFlags		= MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
			pInput.mi.mouseData		= 0;
			pInput.mi.time			= 0;
			pInput.mi.dwExtraInfo	= 0;

			SendInput( 1, &pInput, sizeof( INPUT ) );

			if( this->m_pFont )
			{
				RECT pRect =
				{
					(long)fX,
					(long)fY,
					pViewPort.Width,
					pViewPort.Height
				};

				char szStringBuffer[ 64 ];

				sprintf_s( szStringBuffer, "%0.2f", this->m_pTarget->Position->w );

				this->m_pFont->DrawTextA( 0, szStringBuffer, -1, &pRect, DT_LEFT | DT_TOP | DT_NOCLIP, D3DCOLOR_ARGB( 255, 255, 128, 0 ) );
			}
		}

		this->m_pTarget = NULL;
	}

	return -1;
}

HRESULT WINAPI CAimBot::OnReset( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP )
{
	if( this->m_pFont )
	{
		this->m_pFont->Release();
		this->m_pFont = NULL;
	}
	
	return -1;
}