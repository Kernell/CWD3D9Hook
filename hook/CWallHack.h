class CWallHack;

#ifndef __C_WALLHACK
#define __C_WALLHACK

#include "CHook.h"

using namespace std;

class CWallHack
{
private:
	struct DebugInfo
	{
		float X;
		float Y;
		float Z;
		float W;
		string String;
		D3DCOLOR Color;
	};

	struct Grenade
	{
		float X;
		float Y;
		float Z;
		float W;
	};
	
	LPD3DXFONT				m_pFont;

	list<DebugInfo>			m_Debug;
	list<Grenade>			m_Grenades;
	vector<DWORD>		    m_Textures;

	CHook*					m_pHook;

	bool					m_bEnabled;

	int			            m_iStartRegister;

public:
	CWallHack( CHook* pHook );
	~CWallHack();

	void DoPulse( void );

	HRESULT WINAPI OnRenderPrimitive	( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount );
	HRESULT WINAPI OnRender				( LPDIRECT3DDEVICE9 pDev );
	HRESULT WINAPI OnReset				( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP );

	static inline void Matrix2Position( D3DMATRIX& pMatrix, D3DXVECTOR4& vecPosition )
	{
		D3DXVECTOR4	input;

		input.x = 0.0f;
		input.y = 0.0f;
		input.z = 0.0f;
		input.w = 1.0f;

		D3DXMatrixTranspose( (D3DXMATRIX*)&pMatrix, (D3DXMATRIX*)&pMatrix );

		vecPosition.x = input.x * pMatrix._11 + input.y * pMatrix._21 + input.z * pMatrix._31 + pMatrix._41;
		vecPosition.y = input.x * pMatrix._12 + input.y * pMatrix._22 + input.z * pMatrix._32 + pMatrix._42;
		vecPosition.z = input.x * pMatrix._13 + input.y * pMatrix._23 + input.z * pMatrix._33 + pMatrix._43;
		vecPosition.w = input.x * pMatrix._14 + input.y * pMatrix._24 + input.z * pMatrix._34 + pMatrix._44;
	}
};

#endif