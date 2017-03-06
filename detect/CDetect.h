#pragma comment(lib, "d3d9")
#pragma comment (lib, "d3dx9")

class CDetect;

typedef HRESULT( WINAPI* DrawIndexedPrimitive )( LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, int, UINT, UINT, UINT, UINT );
typedef HRESULT( WINAPI* EndScene )( LPDIRECT3DDEVICE9 );
typedef HRESULT( WINAPI* Reset )( LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS* );

#ifndef __C_DETECT
#define __C_DETECT

using namespace std;

class CDetect
{
private:
	EndScene				m_pEndScene;
	Reset					m_pReset;

	DWORD					m_pLdrLoadDLL;

	LPD3DXFONT				m_pFont;
	D3DVIEWPORT9			m_pViewPort;

	list< string >			m_List;

public:
	CDetect();
	~CDetect();

	void DoPulse( void );

	bool InstallHook();

private:
	static HRESULT WINAPI OnRender				( LPDIRECT3DDEVICE9 pDev );
	static HRESULT WINAPI OnReset				( LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP );

	static DWORD LdrLoadDLL						( IN PWCHAR pathToFile, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE hModule );
};

#endif