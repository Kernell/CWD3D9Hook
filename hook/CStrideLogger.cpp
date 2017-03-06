#include "stdafx.h"
#include "CStrideLogger.h"

CStrideLogger::CStrideLogger( CHook* pHook )
{
	this->m_pHook = pHook;

	this->m_Textures.clear();

	this->m_uiStrideIndex				= 40;
	this->m_uiTextureIndex				= 0;

	this->m_pStride						= new STRIDE;

	this->m_pStride->BaseVertexIndex	= 0;
	this->m_pStride->MinVertexIndex		= 0;
	this->m_pStride->NumVertices		= 0;
	this->m_pStride->StartIndex			= 0;
	this->m_pStride->PrimCount			= 0;

	this->m_bEnabled					= false;
	this->m_bDisableZBuffer				= false;

	this->m_pFont						= NULL;

	this->m_pTextureRed					= NULL;
	this->m_pTextureGreen				= NULL;
}

CStrideLogger::~CStrideLogger()
{
	if( this->m_pFont )
	{
		this->m_pFont->Release();
		this->m_pFont = NULL;
	}

	if( this->m_pTextureRed )
	{
		this->m_pTextureRed->Release();
		this->m_pTextureRed = NULL;
	}

	if( this->m_pTextureGreen )
	{
		this->m_pTextureGreen->Release();
		this->m_pTextureGreen = NULL;
	}

	if( this->m_pStride )
	{
		delete this->m_pStride;

		this->m_pStride = NULL;
	}

	this->m_pHook = NULL;
}

void CStrideLogger::DoPulse( void )
{
	if( GetAsyncKeyState( VK_NUMPAD5 ) )
	{
		this->m_bEnabled = !this->m_bEnabled;
	}

	if( GetAsyncKeyState( VK_NUMPAD4 ) )
	{
		if( this->m_uiStrideIndex > 0 )
		{
			this->m_uiStrideIndex--;

			this->m_uiTextureIndex = 0;

			this->m_Textures.clear();
		}
	}

	if( GetAsyncKeyState( VK_NUMPAD6 ) )
	{
		this->m_uiStrideIndex++;

		this->m_uiTextureIndex = 0;

		this->m_Textures.clear();
	}

	if( GetAsyncKeyState( VK_NUMPAD2 ) )
	{
		if( this->m_uiTextureIndex > 0 )
		{
			this->m_uiTextureIndex--;
		}
	}

	if( GetAsyncKeyState( VK_NUMPAD8 ) )
	{
		if( this->m_uiTextureIndex < this->m_Textures.size() - 1 )
		{
			this->m_uiTextureIndex++;
		}
	}
}

HRESULT WINAPI CStrideLogger::OnRenderPrimitive( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
{
	if( g_pHook->m_bScreenShot )
	{
		return -1;
	}

	if( !this->m_bEnabled )
	{
		return -1;
	}

	HRESULT hResult = -1;

	LPDIRECT3DVERTEXBUFFER9 Stream_Data;

	unsigned int uiStrideIndex	= 0;
	unsigned int uiOffset		= 0;
		
	if( pDev->GetStreamSource( NULL, &Stream_Data, &uiOffset, &uiStrideIndex ) == S_OK )
	{
		Stream_Data->Release();
	}
	
	if( this->m_pTextureRed && this->m_pTextureGreen )
	{
		if( uiStrideIndex == this->m_uiStrideIndex )
		{
			LPDIRECT3DBASETEXTURE9 pBaseTexture = NULL;
			
			if( SUCCEEDED( pDev->GetTexture( NULL, &pBaseTexture ) ) )
			{
				bool bFound = false;

				for( UINT i = 0; i < this->m_Textures.size(); i++ )
				{
					if( this->m_Textures[ i ] == (DWORD)pBaseTexture )
					{
						bFound = true;
					}
				}

				if( bFound == false )
				{
					this->m_Textures.push_back( (DWORD)pBaseTexture );
				}

				if( this->m_Textures[ this->m_uiTextureIndex ] == (DWORD)pBaseTexture )
				{
					this->m_pStride->BaseVertexIndex	= BaseVertexIndex;
					this->m_pStride->MinVertexIndex		= MinVertexIndex;
					this->m_pStride->NumVertices		= NumVertices;
					this->m_pStride->StartIndex			= startIndex;
					this->m_pStride->PrimCount			= primCount;
					this->m_pStride->Offset             = uiOffset;

					if( this->m_bDisableZBuffer )
					{
						pDev->SetRenderState( D3DRS_ZENABLE, 0 );
					//	pDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
						pDev->SetTexture( 0, this->m_pTextureRed );

						hResult = this->m_pHook->DrawIndexedPrimitive( pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );

						pDev->SetRenderState( D3DRS_ZENABLE, 1 );
					}

				//	pDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
					pDev->SetTexture( 0, this->m_pTextureGreen );

					hResult = this->m_pHook->DrawIndexedPrimitive( pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
				}

				pBaseTexture->Release();
			}
		}
	}

	return hResult;
}

HRESULT WINAPI CStrideLogger::OnRender( LPDIRECT3DDEVICE9 pDev )
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

		if( this->m_pTextureRed == NULL )
		{
			this->m_pTextureRed = this->m_pHook->CreateTexture( pDev, 0x80FF0000, 8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 0 );
		}

		if( this->m_pTextureGreen == NULL )
		{
			this->m_pTextureGreen = this->m_pHook->CreateTexture( pDev, 0xFF00FF00, 8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 0 );
		}

		if( this->m_pFont )
		{
			RECT FRect =
			{
				pViewPort.Width - 250,
				pViewPort.Height - 300,
				pViewPort.Width,
				pViewPort.Height
			};

			char szStringBuffer[ 255 ];

			sprintf_s( szStringBuffer,
				"Stride: %i\n" \
				"Textures: %i\n" \
				"Current Texture: %i\n" \
				"Base Vertex Index: %i\n" \
				"Min Vertex Index: %i\n" \
				"Num Vertices: %i\n" \
				"Start Index: %i\n" \
				"Prim Count: %i\n",
				this->m_uiStrideIndex, this->m_Textures.size(), this->m_uiTextureIndex + 1,
				this->m_pStride->BaseVertexIndex,
				this->m_pStride->MinVertexIndex,
				this->m_pStride->NumVertices,
				this->m_pStride->StartIndex,
				this->m_pStride->PrimCount
			);

			this->m_pFont->DrawTextA( 0, szStringBuffer, -1, &FRect, DT_CENTER | DT_NOCLIP, 0xFF00FF00 );
		}
	}

	return -1;
}

HRESULT WINAPI CStrideLogger::OnReset( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP )
{
	if( this->m_pFont )
	{
		this->m_pFont->Release();
		this->m_pFont = NULL;
	}

	if( this->m_pTextureRed )
	{
		this->m_pTextureRed->Release();
		this->m_pTextureRed = NULL;
	}

	if( this->m_pTextureGreen )
	{
		this->m_pTextureGreen->Release();
		this->m_pTextureGreen = NULL;
	}

	return -1;
}