class CAimBot;

#ifndef __C_AIMBOT
#define __C_AIMBOT

#include "CHook.h"

using namespace std;

class CAimBot
{
private:
	class CTarget
	{
	public:
		D3DXVECTOR4* Position;
		UINT NumVertices;
		UINT PrimCount;

		CTarget()
		{
			this->Position		= new D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f );

			this->NumVertices	= 0;
			this->PrimCount		= 0;
		}

		CTarget( float x, float y, float z, float w )
		{
			this->Position		= new D3DXVECTOR4( x, y, z, w );

			this->NumVertices	= 0;
			this->PrimCount		= 0;
		}

		~CTarget()
		{
			if( this->Position )
			{
				delete this->Position;
				
				this->Position = NULL;
			}
		}

		float GetDistance( CTarget* pTarget )
		{
			float fX = this->Position->x - pTarget->Position->x;
			float fY = this->Position->y - pTarget->Position->y;
			float fZ = this->Position->z - pTarget->Position->z;

			return sqrt( ( fX * fX ) + ( fY * fY ) + ( fZ * fZ ) );
		}

		float GetDistance( D3DVIEWPORT9 &pViewPort )
		{
			float fX = this->Position->x - ( pViewPort.Width / 2 );
			float fY = this->Position->y - ( pViewPort.Width / 2 );
			float fZ = 0.0f;

			return sqrt( ( fX * fX ) + ( fY * fY ) + ( fZ * fZ ) );
		}
	};

	CHook*		m_pHook;

	bool		m_bEnabled;

	CTarget*	m_pTarget;

	bool		m_bTargetBEAR;
	bool		m_bTargetUSEC;

	LPD3DXFONT	m_pFont;

	bool IsValidTarget( UINT NumVertices, UINT primCount )
	{
		if( this->m_bTargetBEAR )
			return NumVertices == 1135 && primCount == 1305;

		if( this->m_bTargetUSEC )
			return NumVertices == 1201 && primCount == 1279;

		return false;
	}

public:
	CAimBot( CHook* pHook );
	~CAimBot();

	void DoPulse( void );

	HRESULT WINAPI OnRenderPrimitive	( LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount );
	HRESULT WINAPI OnRender				( LPDIRECT3DDEVICE9 pDev );
	HRESULT WINAPI OnReset				( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP );
};

#endif