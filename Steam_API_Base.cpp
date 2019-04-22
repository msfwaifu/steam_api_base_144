#include <Windows.h>
#include <Shlwapi.h>
#include <conio.h>

#define STEAM_API_EXPORTS

#include "Steam\steam_api.h"
#include "Steam\steamclientpublic.h"
#include "Steam\steam_gameserver.h"

#include "RegistryFunctions.h"
#include "Win32MiniDump.h"
#include "CallbackManager.h"
#include "Steam_API_Base.h"

#include "Functions\SpinLock.h"
#include "Functions\Client.h"
#include "Functions\InterfacePointers.h"
#include "Functions\GameServer.h"
#include "Functions\Minidump.h"
#include "Functions\Callbacks.h"
#include "Functions\Shutdown.h"
#include "Functions\CreateInterface.h"
#include "Functions\Flat.h"

void* InternalAPI_Init(HMODULE *hSteamclient, bool bInitLocal, const char *Interface)
{
	SteamAPI_GetSteamInstallPath();

	if (hSteamclient == nullptr)
		return nullptr;

	SteamUtilsForCallbacks = nullptr;
	SteamControllerForCallbacks = nullptr;
	SteamInputForCallbacks = nullptr;

	char szSteamClientDll[MAX_PATH] = { 0 };
	LSTATUS GetDllString = ERROR_MORE_DATA;

	if (Is64bit == false)
		GetDllString = GetRegistryString("Software\\Valve\\Steam\\ActiveProcess", "SteamClientDll", szSteamClientDll, MAX_PATH);
	else
		GetDllString = GetRegistryString("Software\\Valve\\Steam\\ActiveProcess", "SteamClientDll64", szSteamClientDll, MAX_PATH);

	if (GetDllString == ERROR_MORE_DATA)
	{
		_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; Unable to get steamclient(64).dll path!\r\n");
		return nullptr;
	}

	if (SteamAPI_IsSteamRunning() == true)
	{
		*hSteamclient = LoadLibraryExA(szSteamClientDll, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);

		if (*hSteamclient == nullptr)
			_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; Failed to load steamclient(64).dll!\r\n");
	}
	else
	{
		_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; SteamAPI_IsSteamRunning() failed!\r\n");
	}

	if (*hSteamclient == nullptr)
	{
		if (bInitLocal == false)
		{
			_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; Unable to locate a running instance of Steam and bInitLocal is false!\r\n");
			return nullptr;
		}

		if (Is64bit == false)
			*hSteamclient = LoadLibraryA("steamclient.dll");
		else
			*hSteamclient = LoadLibraryA("steamclient64.dll");

		if (*hSteamclient == nullptr)
		{
			_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; Unable to locate a running instance of Steam, or a local steamclient(64).dll!\r\n");
			return nullptr;
		}
	}

	oCreateInterface = (_CreateInterface)GetProcAddress(*hSteamclient, "CreateInterface");

	if (oCreateInterface != nullptr)
	{
		SteamClientSafe = (ISteamClient *)oCreateInterface(STEAMCLIENT_INTERFACE_VERSION, nullptr);
		oReleaseThreadLocalMemory = (_ReleaseThreadLocalMemory)GetProcAddress(*hSteamclient, "Steam_ReleaseThreadLocalMemory");
		ContextCounter++;

		return oCreateInterface(Interface, nullptr);
	}
	else
	{
		_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; Unable to locate interface factory in steamclient(64).dll!\r\n");

		if (*hSteamclient != nullptr)
			FreeLibrary(*hSteamclient);

		*hSteamclient = nullptr;
	}

	return nullptr;
}

void MyInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
{
	MessageBoxA(nullptr, "steam_api(64).dll Crashed (Invalid Parameter Handler)!", "Invalid Parameter Handler", (MB_OK | MB_ICONERROR | MB_DEFBUTTON1 | MB_SETFOREGROUND));
	ExitProcess(0);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_invalid_parameter_handler OldHandler, NewHandler;
		NewHandler = MyInvalidParameterHandler;
		OldHandler = _set_invalid_parameter_handler(NewHandler);

		// Disable the message box for assertions.
		_CrtSetReportMode(_CRT_ASSERT, 0);

		AllocConsole();

		char szFullDllPath[MAX_PATH] = { 0 };

		DWORD ModuleFileName = GetModuleFileNameA(hModule, szFullDllPath, sizeof(szFullDllPath));

		if (ModuleFileName == 0)
		{
			MessageBoxA(nullptr, "Unable to get dll name (1)!", "Steam API Base", (MB_OK | MB_ICONERROR | MB_DEFBUTTON1 | MB_SETFOREGROUND));
			ExitProcess(0);
		}

		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			MessageBoxA(nullptr, "Unable to get dll name (2)!", "Steam API Base", (MB_OK | MB_ICONERROR | MB_DEFBUTTON1 | MB_SETFOREGROUND));
			ExitProcess(0);
		}

		_cprintf_s("[Steam_API_Base] Dll Path --> %s\r\n", szFullDllPath);

		if (StrStrIA(szFullDllPath, "steam_api.dll") != nullptr)
		{
			Is64bit = false;
		}
		else if (StrStrIA(szFullDllPath, "steam_api64.dll") != nullptr)
		{
			Is64bit = true;
		}

		PID = GetCurrentProcessId();
		_cprintf_s("[Steam_API_Base] PID --> %d\r\n", PID);
		_cprintf_s("[Steam_API_Base] ThreadID --> %d\r\n", GetCurrentThreadId());
	}
	return 1;
}