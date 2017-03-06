#include "stdafx.h"
#include "CDetect.h"

CDetect *g_pDetect;
HANDLE g_pThread = NULL;

void Main()
{
	if( g_pDetect )
	{
		return;
	}
	
	g_pDetect = new CDetect();
	
	while( g_pDetect )
	{
		Sleep( 100 );

		g_pDetect->DoPulse();
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
			}

			break;
		}
		case DLL_PROCESS_DETACH:
		{
			if( g_pDetect )
			{
				delete g_pDetect;
				
				g_pDetect = NULL;
			}

			if( g_pThread )
			{
				CloseHandle( g_pThread );
				
				g_pThread = NULL;
			}
			
			break;
		}
	}
	
	return 1;
}