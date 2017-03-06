class CScreenShot;

extern HMODULE g_hModule;

#ifndef __C_SCREENSHOT
#define __C_SCREENSHOT

#include "CHook.h"

using namespace std;

class CScreenShot
{
private:
	CHook*				m_pHook;

	long long			m_lLastSaveTime;

	bool				m_bIsSaving;

	IDirect3DSurface9*	m_pSurface;
	IDirect3DSurface9*	m_pRTSurface;

public:
	CScreenShot( CHook* pHook );
	~CScreenShot();

	void DoPulse( void );

	HRESULT WINAPI OnRenderPrimitive	( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount );
	HRESULT WINAPI OnRender				( LPDIRECT3DDEVICE9 pDev );
	HRESULT WINAPI OnReset				( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP );

	bool CheckForScreenShot( LPDIRECT3DDEVICE9 pDev );

	HRESULT BeginSave( const char *szFileName, IDirect3DSurface9* pData, unsigned int uiWidth, unsigned int uiHeight );

	string GetValidScreenshotFilename( void );

	bool IsSaving( void );
};

#endif