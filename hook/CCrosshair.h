class CCrosshair;

#ifndef __C_CROSSHAIR
#define __C_CROSSHAIR

#include "CHook.h"

using namespace std;

class CCrosshair
{
private:
	CHook*			m_pHook;

	bool			m_bEnabled;

	D3DCOLOR		m_ulColor;

	unsigned int	m_uiSize = 8;

public:
	CCrosshair( CHook* pHook );
	~CCrosshair();

	void DoPulse( void );

//	HRESULT WINAPI OnRenderPrimitive( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount );
	HRESULT WINAPI OnRender( LPDIRECT3DDEVICE9 pDev );
	HRESULT WINAPI OnReset( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP );
};

#endif