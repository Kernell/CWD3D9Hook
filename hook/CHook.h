class CHook;

extern CHook *g_pHook;

#ifndef __C_HOOK
#define __C_HOOK

#include "CDebug.h"
#include "CScreenShot.h"
#include "CStrideLogger.h"
#include "CWallHack.h"
#include "CAimBot.h"
#include "CCrosshair.h"

using namespace std;

class CHook
{
private:
	bool					m_bHookInstalled;

	CDebug*					m_pDebug;
	CScreenShot*			m_pScreenShot;
	CStrideLogger*			m_pStrideLogger;
	CWallHack*				m_pWallHack;
	CAimBot*				m_pAimBot;
	CCrosshair*				m_pCrosshair;

public:
	DrawIndexedPrimitive	DrawIndexedPrimitive;
	EndScene				EndScene;
	Reset					Reset;

	bool					m_bScreenShot;

public:
	CHook();
	~CHook();

	void ApplyHook	( void );
	void RemoveHook	( void );
	void DoPulse	( void );

	CDebug* GetDebug()
	{
		return this->m_pDebug;
	}

private:
	static HRESULT WINAPI OnRenderPrimitive	( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount );
	static HRESULT WINAPI OnRender			( LPDIRECT3DDEVICE9 pDev );
	static HRESULT WINAPI OnReset			( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP );

public: // Helper functions

	IDirect3DTexture9* CreateTexture( LPDIRECT3DDEVICE9 pDev, DWORD Color, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE* pSharedHandle )
	{
		LPDIRECT3DTEXTURE9 pTexture;

		if( pDev->CreateTexture( Width, Height, Levels, Usage, Format, Pool, &pTexture, pSharedHandle ) == S_OK )
		{
			LPDIRECT3DTEXTURE9 pTempTexture;
			
			if( pDev->CreateTexture( Width, Height, Levels, Usage, Format, D3DPOOL_SYSTEMMEM, &pTempTexture, pSharedHandle ) == S_OK )
			{
				D3DLOCKED_RECT pRect;

				if( pTempTexture->LockRect( 0, &pRect, 0, D3DLOCK_DONOTWAIT | D3DLOCK_NOSYSLOCK ) == S_OK )
				{
					for( UINT xy = 0; xy < Width * Height; xy++ )
					{
						( (PDWORD)pRect.pBits )[ xy ] = Color;
					}

					pTempTexture->UnlockRect( 0 );

					pDev->UpdateTexture( pTempTexture, pTexture );

					pTempTexture->Release();

					return pTexture;
				}

				pTempTexture->Release();
			}
			
			pTexture->Release();
		}

		return NULL;
	}

	void DrawRectangle( IDirect3DDevice9 *pD3D9Device, int x, int y, int w, int h, DWORD ulFlags, D3DCOLOR pColor, float fZ = 0.0f, DWORD ulStencil = 0 )
	{
		if( pD3D9Device )
		{
			D3DRECT rec = { x, y, x + w, y + h };

			pD3D9Device->Clear( 1, &rec, ulFlags, pColor, fZ, ulStencil );
		}
	}

	void DrawBox( IDirect3DDevice9 *pD3D9Device, int x, int y, int w, int h, int d, DWORD ulFlags, D3DCOLOR pColor, float fZ = 0.0f, DWORD ulStencil = 0 )
	{
		this->DrawRectangle( pD3D9Device, x + d, y, w - 2 * d, d, ulFlags, pColor, fZ, ulStencil );
		this->DrawRectangle( pD3D9Device, x, y, d, h, ulFlags, pColor, fZ, ulStencil );
		this->DrawRectangle( pD3D9Device, x + w - d, y, d, h, ulFlags, pColor, fZ, ulStencil );
		this->DrawRectangle( pD3D9Device, x + d, y + h - d, w - 2 * d, d, ulFlags, pColor, fZ, ulStencil );
	}
};

#endif