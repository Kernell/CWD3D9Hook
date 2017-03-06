#include "stdafx.h"
#include "CHook.h"

CHook *g_pHook;
HANDLE g_pThread = NULL;
HINSTANCE g_hModule = NULL;

void Main()
{
	if( g_pHook )
	{
		return;
	}
	
	new CHook();
	
	while( g_pHook )
	{
		Sleep( 100 );

		g_pHook->DoPulse();
	}
	
	CloseHandle( g_pThread );
	
	g_pThread = NULL;
}

int __stdcall DllMain( HINSTANCE hModule, DWORD dwReason, PVOID pvNothing )
{
	switch( dwReason )
	{
		case DLL_PROCESS_ATTACH:
		{
			if( !g_pThread )
			{
				g_pThread = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)Main, 0, 0, 0 );

				g_hModule = hModule;
			}

			break;
		}
		case DLL_PROCESS_DETACH:
		{
			if( g_pHook )
			{
				delete g_pHook;
				
				g_pHook = NULL;
			}

			if( g_pThread )
			{
				CloseHandle( g_pThread );
				
				g_pThread = NULL;

				g_hModule = NULL;
			}
			
			break;
		}
	}
	
	return 1;
}
