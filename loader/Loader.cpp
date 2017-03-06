#include "Loader.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;

using namespace loader;

struct handle_data
{
    unsigned long ulPID;
    LPWSTR lpName;
};

[STAThread]
void Main( array<String^>^ arg )
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault( false );
 
    Loader form;

    Application::Run( %form );
}

BOOL CALLBACK EnumWindowsCallback( HWND handle, LPARAM lParam )
{
    handle_data& data = *(handle_data*)lParam;

	unsigned long process_id = 0;

    GetWindowThreadProcessId( handle, &process_id );

    if( data.ulPID == process_id )
	{
		int iLength = GetWindowTextLength( handle );

		if( iLength == 0 )
		{
			return true;
		}

		char* buffer = new char[ iLength + 1 ];

		memset( buffer, 0, ( iLength + 1 ) * sizeof( char ) );

		GetWindowTextA( handle, buffer, iLength + 1 );

		wchar_t* wString = new wchar_t[ iLength + 1 ];

		MultiByteToWideChar( CP_ACP, 0, buffer, -1, wString, iLength + 1 );

		data.ulPID	= process_id;
		data.lpName = wString;

		delete [] buffer;

		return false;
    }

	return true;
}

Void Loader::Loader_Load( System::Object^  sender, System::EventArgs^  e )
{
	this->toolStripStatus->Text = L"Ready";

	this->LoadProcesses();
}

Void Loader::ButtonRefresh_Click( System::Object^  sender, System::EventArgs^  e )
{
	this->LoadProcesses();
}

Void Loader::LoadProcesses( Void )
{
	this->toolStripStatus->Text = L"Loading process list";

	this->ProcessList->Rows->Clear();

	PROCESSENTRY32 process;
	THREADENTRY32 thread;
	GUITHREADINFO guiInfo;

	HANDLE thSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	process.dwSize = sizeof( PROCESSENTRY32 );

	Process32First( thSnapShot, &process );

	HWND hWindow;

	do
	{
		GetGUIThreadInfo( thread.th32ThreadID, &guiInfo );

		hWindow = GetWindow( guiInfo.hwndActive, GW_HWNDFIRST );

		if( hWindow == NULL )
		{
			hWindow = guiInfo.hwndActive;
		}

		if( hWindow )
		{
			handle_data data;

			data.ulPID	= process.th32ProcessID;
			data.lpName	= L"";

			EnumWindows( EnumWindowsCallback, (LPARAM)&data );

			this->ProcessList->Rows->Add(
				gcnew array<String^>
				{
					Convert::ToString( (unsigned int)process.th32ProcessID ),
					gcnew String( process.szExeFile ),
					gcnew String( data.lpName )
				}
			);
		}
	}
	while( Process32Next( thSnapShot, &process ) );

	CloseHandle( thSnapShot );

	this->toolStripStatus->Text = L"Ready";
}

Void Loader::ButtonInject_Click( System::Object^  sender, System::EventArgs^  e )
{
	if( this->openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		int iRow = this->ProcessList->CurrentRow->Index;

		unsigned long ulPID = int::Parse( this->ProcessList[ 0, iRow ]->Value->ToString() );

		this->Inject( ulPID, this->openFileDialog->FileName );
	}
}

Void Loader::Inject( unsigned long ulPID, String^ strFilePath )
{
	if( ulPID == NULL )
	{
		return;
	}

	const char* szFilePath = (char*)(void*)Marshal::StringToHGlobalAnsi( strFilePath );

	HANDLE pHandle = OpenProcess( PROCESS_ALL_ACCESS, 0, ulPID );

	unsigned long loadLibAddr = (unsigned long)GetProcAddress( GetModuleHandle( L"kernel32.dll" ), "LoadLibraryA" );

	LPVOID rString = (LPVOID)VirtualAllocEx( pHandle, 0, strlen( szFilePath ), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

	WriteProcessMemory( pHandle, (LPVOID)rString, szFilePath, strlen( szFilePath ), 0 );

	CreateRemoteThread( pHandle, 0, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, (LPVOID)rString, 0, 0 );

	CloseHandle( pHandle );

	this->toolStripStatus->Text = L"Injected!";

	return;
}