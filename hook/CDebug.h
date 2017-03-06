class CDebug;

#ifndef __C_DEBUG
#define __C_DEBUG

#include "CHook.h"

using namespace std;

class CDebug
{
private:
	CHook*				m_pHook;

	bool				m_bVisible;
	LPD3DXFONT			m_pFont;
	list< string >		m_List;

public:
	CDebug( CHook* pHook );
	~CDebug();

	void DoPulse( void );

	HRESULT WINAPI OnRenderPrimitive	( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount );
	HRESULT WINAPI OnRender				( LPDIRECT3DDEVICE9 pDev );
	HRESULT WINAPI OnReset				( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP );

	void Printf	( const char* szFormat, ... );
	void Print	( const char* szString );
};

#endif