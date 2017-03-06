class CStrideLogger;

#ifndef __C_STRIDE_LOGGER
#define __C_STRIDE_LOGGER

#include "CHook.h"

using namespace std;

class CStrideLogger
{
private:
	typedef struct _STRIDE
	{
		INT BaseVertexIndex;
		UINT MinVertexIndex;
		UINT NumVertices;
		UINT StartIndex;
		UINT PrimCount;
		UINT Offset;
	}
	STRIDE, *PSTRIDE;

	STRIDE*				m_pStride;

	vector<DWORD>		m_Textures;

	LPD3DXFONT			m_pFont;

	LPDIRECT3DTEXTURE9	m_pTextureGreen;
	LPDIRECT3DTEXTURE9	m_pTextureRed;

	unsigned int		m_uiStrideIndex;
	unsigned int		m_uiTextureIndex;

	CHook*				m_pHook;

	bool				m_bEnabled;
	bool				m_bDisableZBuffer;

public:
	CStrideLogger( CHook* pHook );
	~CStrideLogger();

	void DoPulse( void );

	HRESULT WINAPI OnRenderPrimitive	( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount );
	HRESULT WINAPI OnRender				( LPDIRECT3DDEVICE9 pDev );
	HRESULT WINAPI OnReset				( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP );
};

#endif