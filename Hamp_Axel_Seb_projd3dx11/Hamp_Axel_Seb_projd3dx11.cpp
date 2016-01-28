// Hamp_Axel_Seb_projd3dx11.cpp : Defines the entry point for the console application.
//
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

int WINAPI main
(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
	)
{
	
		// create a "Hello World" message box using MessageBox()
		MessageBox(NULL,
			L"Hello World!",
			L"Just another Hello World program!",
			MB_ICONEXCLAMATION | MB_OK);

		// return 0 to Windows
		return 0;
}

