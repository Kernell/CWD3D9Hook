#include "stdafx.h"
#include "CCrosshair.h"

CCrosshair::CCrosshair( CHook* pHook )
{
	this->m_pHook		= pHook;
	this->m_bEnabled	= false;

	this->m_ulColor		= D3DCOLOR_ARGB( 200, 200, 200, 200 );
}

CCrosshair::~CCrosshair()
{
	this->m_pHook		= NULL;
}

void CCrosshair::DoPulse( void )
{
	if( GetAsyncKeyState( VK_SHIFT ) && GetAsyncKeyState( VK_F11 ) )
	{
		this->m_bEnabled = !this->m_bEnabled;
	}
}

HRESULT WINAPI CCrosshair::OnRender( LPDIRECT3DDEVICE9 pDev )
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
		DWORD fX = ( pViewPort.Width / 2 ) - 1;
		DWORD fY = ( pViewPort.Height / 2 ) - 1;

		this->m_pHook->DrawRectangle( pDev, fX, fY, 1, 1, D3DCLEAR_TARGET, this->m_ulColor );

		this->m_pHook->DrawRectangle( pDev, fX - ( this->m_uiSize * 2 ), fY, this->m_uiSize, 1, D3DCLEAR_TARGET, this->m_ulColor );
		this->m_pHook->DrawRectangle( pDev, fX, fY - ( this->m_uiSize * 2 ), 1, this->m_uiSize, D3DCLEAR_TARGET, this->m_ulColor );

		this->m_pHook->DrawRectangle( pDev, fX + this->m_uiSize + 1, fY, this->m_uiSize, 1, D3DCLEAR_TARGET, this->m_ulColor );
		this->m_pHook->DrawRectangle( pDev, fX, fY + this->m_uiSize + 1, 1, this->m_uiSize, D3DCLEAR_TARGET, this->m_ulColor );
	}

	return -1;
}

HRESULT WINAPI CCrosshair::OnReset( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP )
{
	return -1;
}