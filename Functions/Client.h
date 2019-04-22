//#include "../FindPatternFunctions.h"
CRITICAL_SECTION CriticalSection;

S_API HSteamPipe S_CALLTYPE GetHSteamPipe()
{
	_cprintf_s("[Steam_API_Base] GetHSteamPipe\r\n");
	return hPipe;
}

S_API HSteamUser S_CALLTYPE GetHSteamUser()
{
	_cprintf_s("[Steam_API_Base] GetHSteamUser\r\n");
	return hUser;
}

S_API HSteamPipe S_CALLTYPE SteamAPI_GetHSteamPipe()
{
	_cprintf_s("[Steam_API_Base] SteamAPI_GetHSteamPipe\r\n");
	return hPipe;
}

S_API HSteamUser S_CALLTYPE SteamAPI_GetHSteamUser()
{
	_cprintf_s("[Steam_API_Base] SteamAPI_GetHSteamUser\r\n");
	return hUser;
}

S_API const char* S_CALLTYPE SteamAPI_GetSteamInstallPath()
{
	_cprintf_s("[Steam_API_Base] SteamAPI_GetSteamInstallPath\r\n");

	if (AlreadyHaveInstallPath == false)
	{
		DWORD ActiveProcessPID = 0;
		LSTATUS GetPID = GetRegistryDWORD("Software\\Valve\\Steam\\ActiveProcess", "pid", ActiveProcessPID);

		if (GetPID == ERROR_SUCCESS && ActiveProcessPID != 0)
		{
			HANDLE hPIDProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ActiveProcessPID);

			if (hPIDProcess != nullptr)
			{
				DWORD ExitCode = 0;
				BOOL bExitCode = GetExitCodeProcess(hPIDProcess, &ExitCode);

				CloseHandle(hPIDProcess);

				if (bExitCode == TRUE && ExitCode == 259)
				{
					LSTATUS GetDllString = ERROR_MORE_DATA;

					if (Is64bit == false)
						GetDllString = GetRegistryString("Software\\Valve\\Steam\\ActiveProcess", "SteamClientDll", szSteamInstallPath, MAX_PATH);
					else
						GetDllString = GetRegistryString("Software\\Valve\\Steam\\ActiveProcess", "SteamClientDll64", szSteamInstallPath, MAX_PATH);

					if (GetDllString != ERROR_MORE_DATA)
					{
						PathRemoveFileSpecA(szSteamInstallPath);

						AlreadyHaveInstallPath = true;

						_cprintf_s("[Steam_API_Base] Steam Install Path --> %s\r\n", szSteamInstallPath);

						return szSteamInstallPath;
					}
					else
					{
						_cprintf_s("[Steam_API_Base] Unable to get steamclient(64).dll path (SteamAPI_GetSteamInstallPath)!\r\n");
					}
				}
				else
				{
					_cprintf_s("[Steam_API_Base] GetExitCodeProcess failed (SteamAPI_GetSteamInstallPath)!\r\n");
				}
			}
			else
			{
				_cprintf_s("[Steam_API_Base] OpenProcess failed (SteamAPI_GetSteamInstallPath)!\r\n");
			}
		}
		else
		{
			_cprintf_s("[Steam_API_Base] Unable to get the PID of the Steam process (SteamAPI_GetSteamInstallPath)!\r\n");
		}
	}
	else
	{
		return szSteamInstallPath;
	}
	return "Invalid Path";
}

S_API bool S_CALLTYPE SteamAPI_Init()
{
	_cprintf_s("[Steam_API_Base] SteamAPI_Init\r\n");

	if (SteamClient_Client != nullptr)
		return true;

	SteamClient_Client = static_cast<ISteamClient*>(InternalAPI_Init(&hSteamclient_Client, false, STEAMCLIENT_INTERFACE_VERSION));

	if (SteamClient_Client == nullptr)
		return false;

	hPipe = SteamClient_Client->CreateSteamPipe();

	if (hPipe == 0)
	{
		_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; Create pipe failed!\r\n");
		SteamAPI_Shutdown();
		return false;
	}

	hUser = SteamClient_Client->ConnectToGlobalUser(hPipe);

	if (hUser != 0)
	{
		ISteamUtils *pSteamUtils = (ISteamUtils*)SteamClient_Client->GetISteamUtils(hPipe, STEAMUTILS_INTERFACE_VERSION);

		if (pSteamUtils != nullptr)
		{
			uint32 AppID = pSteamUtils->GetAppID();

			if (AppID != 0)
			{
				if (GetEnvironmentVariableA("SteamAppId", nullptr, 0) == 0)
				{
					char szAppID[MAX_PATH] = { 0 };
					_snprintf_s(szAppID, MAX_PATH, _TRUNCATE, "%u", AppID);

					SetEnvironmentVariableA("SteamAppId", szAppID);
				}

				if (GetEnvironmentVariableA("SteamGameId", nullptr, 0) == 0)
				{
					char szGameID[MAX_PATH] = { 0 };
					_snprintf_s(szGameID, MAX_PATH, _TRUNCATE, "%llu", CGameID(AppID).ToUint64());

					SetEnvironmentVariableA("SteamGameId", szGameID);
				}

				SteamAPI_SetBreakpadAppID(AppID);
				Steam_RegisterInterfaceFuncs(hSteamclient_Client);
				LoadBreakpad();

				char sOverlayDllPath[MAX_PATH] = { 0 };

				if (Is64bit == false)
					_snprintf_s(sOverlayDllPath, MAX_PATH, _TRUNCATE, "%s\\GameOverlayRenderer.dll", SteamAPI_GetSteamInstallPath());
				else
					_snprintf_s(sOverlayDllPath, MAX_PATH, _TRUNCATE, "%s\\GameOverlayRenderer64.dll", SteamAPI_GetSteamInstallPath());

				LoadLibraryExA(sOverlayDllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);

				ISteamUser *pSteamUser = (ISteamUser*)SteamClient_Client->GetISteamUser(hUser, hPipe, STEAMUSER_INTERFACE_VERSION);

				if (pSteamUser != nullptr)
					SetMinidumpSteamID(pSteamUser->GetSteamID().ConvertToUint64());

				InitClientPointers = GetInterfacePointers.Init();

				if (InitClientPointers == false)
				{
					_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; GetInterfacePointers.Init failed!\r\n");

					SteamAPI_Shutdown();
					return false;
				}

				//ISteamUser::BLoggedOn Patch (x86)
				//DWORD IsLoggedOnPattern = FindPattern("steamclient.dll", "\x8B\x49\x04\x8B\x01\x8B\x40\x18\xFF\xE0", "xxxxxxxxxx");
				//BYTE IsLoggedOnPatch[3] = { 0xB0, 0x01, 0xC3 };
				//WriteToMemory(IsLoggedOnPattern, IsLoggedOnPatch, 3);

				return true;
			}
			else
			{
				_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; No appID found!\r\nEither launch the game from Steam, or put the file steam_appid.txt containing the correct appID in your game folder.\r\n");
			}
		}
		else
		{
			_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; Failed to get pointer to ISteamUtils!\r\n");
		}
	}
	else
	{
		_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_Init() failed; Connect to global user failed!\r\n");
	}
	SteamAPI_Shutdown();
	return false;
}

S_API bool S_CALLTYPE SteamAPI_InitAnonymousUser()
{
	_cprintf_s("[Steam_API_Base] SteamAPI_InitAnonymousUser\r\n");

	if (SteamClient_Client != nullptr)
		return true;

	SteamClient_Client = static_cast<ISteamClient*>(InternalAPI_Init(&hSteamclient_Client, true, STEAMCLIENT_INTERFACE_VERSION));

	if (SteamClient_Client == nullptr)
		return false;

	hUser = SteamClient_Client->CreateLocalUser(&hPipe, k_EAccountTypeAnonUser);

	if (hUser != 0 && hPipe != 0)
	{
		ISteamUtils *pSteamUtils = (ISteamUtils*)SteamClient_Client->GetISteamUtils(hPipe, STEAMUTILS_INTERFACE_VERSION);

		if (pSteamUtils != nullptr)
		{
			uint32 AppID = pSteamUtils->GetAppID();

			if (AppID != 0)
			{
				if (GetEnvironmentVariableA("SteamAppId", nullptr, 0) == 0)
				{
					char szAppID[MAX_PATH] = { 0 };
					_snprintf_s(szAppID, MAX_PATH, _TRUNCATE, "%u", AppID);

					SetEnvironmentVariableA("SteamAppId", szAppID);
				}

				if (GetEnvironmentVariableA("SteamGameId", nullptr, 0) == 0)
				{
					char szGameID[MAX_PATH] = { 0 };
					_snprintf_s(szGameID, MAX_PATH, _TRUNCATE, "%llu", CGameID(AppID).ToUint64());

					SetEnvironmentVariableA("SteamGameId", szGameID);
				}

				SteamAPI_SetBreakpadAppID(AppID);
				Steam_RegisterInterfaceFuncs(hSteamclient_Client);
				LoadBreakpad();

				char sOverlayDllPath[MAX_PATH] = { 0 };

				if (Is64bit == false)
					_snprintf_s(sOverlayDllPath, MAX_PATH, _TRUNCATE, "%s\\GameOverlayRenderer.dll", SteamAPI_GetSteamInstallPath());
				else
					_snprintf_s(sOverlayDllPath, MAX_PATH, _TRUNCATE, "%s\\GameOverlayRenderer64.dll", SteamAPI_GetSteamInstallPath());

				LoadLibraryExA(sOverlayDllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);

				ISteamUser *pSteamUser = (ISteamUser*)SteamClient_Client->GetISteamUser(hUser, hPipe, STEAMUSER_INTERFACE_VERSION);

				if (pSteamUser != nullptr)
					SetMinidumpSteamID(pSteamUser->GetSteamID().ConvertToUint64());

				InitClientPointers = GetInterfacePointers.Init();

				if (InitClientPointers == false)
				{
					_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_InitAnonymousUser() failed; GetInterfacePointers.Init failed!\r\n");

					SteamAPI_Shutdown();
					return false;
				}
				return true;
			}
			else
			{
				_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_InitAnonymousUser() failed; No appID found!\r\nEither launch the game from Steam, or put the file steam_appid.txt containing the correct appID in your game folder.\r\n");
			}
		}
		else
		{
			_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_InitAnonymousUser() failed; Failed to get pointer to ISteamUtils!\r\n");
		}
	}
	else
	{
		_cprintf_s("[Steam_API_Base] [S_API FAIL] SteamAPI_InitAnonymousUser() failed; Create local user failed!\r\n");
	}
	SteamAPI_Shutdown();
	return false;
}

S_API bool S_CALLTYPE SteamAPI_InitSafe()
{
	_cprintf_s("[Steam_API_Base] SteamAPI_InitSafe\r\n");

	bool Init = SteamAPI_Init();

	if (Init == true && SteamClientSafe != nullptr)
		return true;

	_cprintf_s("[Steam_API_Base] SteamAPI_InitSafe Failed!\r\n");

	return false;
}

S_API bool S_CALLTYPE SteamAPI_IsSteamRunning()
{
	_cprintf_s("[Steam_API_Base] SteamAPI_IsSteamRunning\r\n");

	DWORD ActiveProcessPID = 0;
	LSTATUS GetPID = GetRegistryDWORD("Software\\Valve\\Steam\\ActiveProcess", "pid", ActiveProcessPID);

	if (GetPID == ERROR_SUCCESS && ActiveProcessPID != 0)
	{
		HANDLE hPIDProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ActiveProcessPID);

		if (hPIDProcess != nullptr)
		{
			DWORD ExitCode = 0;
			BOOL bExitCode = GetExitCodeProcess(hPIDProcess, &ExitCode);

			CloseHandle(hPIDProcess);

			if (bExitCode == TRUE && ExitCode == 259)
			{
				return true;
			}
			else
			{
				_cprintf_s("[Steam_API_Base] GetExitCodeProcess failed (SteamAPI_IsSteamRunning)!\r\n");
			}
		}
		else
		{
			_cprintf_s("[Steam_API_Base] OpenProcess failed (SteamAPI_IsSteamRunning)!\r\n");
		}
	}
	else
	{
		_cprintf_s("[Steam_API_Base] Unable to get the PID of the Steam process (SteamAPI_IsSteamRunning)!\r\n");
	}

	return false;
}

S_API void S_CALLTYPE SteamAPI_ReleaseCurrentThreadMemory()
{
	_cprintf_s("[Steam_API_Base] SteamAPI_ReleaseCurrentThreadMemory\r\n");

	if (oReleaseThreadLocalMemory != nullptr)
		oReleaseThreadLocalMemory(0);

	return;
}

S_API bool S_CALLTYPE SteamAPI_RestartAppIfNecessary(uint32 unOwnAppID)
{
	_cprintf_s("[Steam_API_Base] SteamAPI_RestartAppIfNecessary\r\n");

	if (unOwnAppID == k_uAppIdInvalid)
		return false;

	char Buffer[32] = { 0 };

	DWORD AppIDVariable = GetEnvironmentVariableA("SteamAppId", Buffer, 32);

	if (AppIDVariable - 1 <= 30 && strtol(Buffer, nullptr, 10) != 0)
		return false;

	FILE *AppIDFile;

	if (fopen_s(&AppIDFile, "steam_appid.txt", "rb") == 0)
	{
		char szAppId[MAX_PATH] = { 0 };
		if (fgets(szAppId, MAX_PATH, AppIDFile) != nullptr)
		{
			fclose(AppIDFile);

			if (strtol(szAppId, nullptr, 10) != 0)
				return false;
		}
	}

	_cprintf_s("[Steam_API_Base] SteamAPI_RestartAppIfNecessary Failed!\r\n");

	return true;
}

S_API void* S_CALLTYPE SteamInternal_ContextInit(void *pContextInitData)
{
	InitializeCriticalSection(&CriticalSection);

	_cprintf_s("[Steam_API_Base] SteamInternal_ContextInit\r\n");

	if (pContextInitData != nullptr)
	{
		if (Is64bit == false)
		{
			ContextInit *InitData = (ContextInit*)pContextInitData;

			char* pPointer = static_cast<char*>(pContextInitData);

			if (InitData->Counter == ContextCounter)
			{
				DeleteCriticalSection(&CriticalSection);
				return pPointer + 8;
			}

			EnterCriticalSection(&CriticalSection);

			if (InitData->Counter != ContextCounter)
			{
				InitData->pFn(pPointer + 8);
				InitData->Counter = ContextCounter;
			}

			LeaveCriticalSection(&CriticalSection);
			DeleteCriticalSection(&CriticalSection);

			return pPointer + 8;
		}
		else
		{
			ContextInit *InitData = (ContextInit*)pContextInitData;

			char* pPointer = static_cast<char*>(pContextInitData);

			if (InitData->Counter == ContextCounter)
			{
				DeleteCriticalSection(&CriticalSection);
				return pPointer + 16;
			}

			EnterCriticalSection(&CriticalSection);

			if (InitData->Counter != ContextCounter)
			{
				InitData->pFn(pPointer + 16);
				InitData->Counter = ContextCounter;
			}

			LeaveCriticalSection(&CriticalSection);
			DeleteCriticalSection(&CriticalSection);

			return pPointer + 16;
		}
	}

	_cprintf_s("[Steam_API_Base] SteamInternal_ContextInit Failed!\r\n");

	DeleteCriticalSection(&CriticalSection);
	return nullptr;

	//SDK 1.42
/*#if defined(_M_IX86)
	__asm
	{
	    mov esi, [pContextInitData]
	    mov eax, [esi + 4]
	    cmp eax, ContextCounter
	    lea eax, [esi + 8]
	    jz Skip
	    push eax
	    mov eax, [esi]
	    call eax
	    mov eax, ContextCounter
	    add esp, 4
	    mov[esi + 4], eax
	    lea eax, [esi + 8]
	    Skip:
	}
#endif*/
}