static CWin32MiniDump s_Win32MiniDump("steam_api");

void SetMinidumpSteamID(uint64 SteamID)
{
	_cprintf_s("[Steam_API_Base] SetMinidumpSteamID\r\n");

	if (oBreakpad_SteamSetSteamID != nullptr)
		_cprintf_s("[Steam_API_Base] Steam_SetMinidumpSteamID:  Caching Steam ID:  %lld [API loaded yes]\r\n", SteamID);
	else
		_cprintf_s("[Steam_API_Base] Steam_SetMinidumpSteamID:  Caching Steam ID:  %lld [API loaded no]\r\n", SteamID);

	BreakpadSteamID = SteamID;

	if (UsingBreakpadCrashHandler == true)
	{
		if (oBreakpad_SteamSetSteamID != nullptr)
		{
			_cprintf_s("[Steam_API_Base] Steam_SetMinidumpSteamID:  Setting Steam ID:  %lld\r\n", SteamID);
			oBreakpad_SteamSetSteamID(SteamID);
		}
	}
	return;
}

void LoadBreakpad()
{
	if (UsingBreakpadCrashHandler == false)
		return;

	_cprintf_s("[Steam_API_Base] LoadBreakpad\r\n");

	HMODULE hSteamClient = nullptr;

	if (hSteamclient_Client != nullptr)
	{
		hSteamClient = hSteamclient_Client;
	}
	else
	{
		if (Is64bit == false)
			hSteamClient = GetModuleHandleA("steamclient.dll");
		else
			hSteamClient = GetModuleHandleA("steamclient64.dll");

		if (hSteamClient == nullptr)
			return;
	}

	_cprintf_s("[Steam_API_Base] Looking up breakpad interfaces from steamclient\r\n");

	oBreakpad_SteamWriteMiniDumpUsingExceptionInfoWithBuildId = (_Breakpad_SteamWriteMiniDumpUsingExceptionInfoWithBuildId)GetProcAddress(hSteamClient, "Breakpad_SteamWriteMiniDumpUsingExceptionInfoWithBuildId");

	if (oBreakpad_SteamWriteMiniDumpUsingExceptionInfoWithBuildId == nullptr)
		return;

	oBreakpad_SteamWriteMiniDumpSetComment = (_Breakpad_SteamWriteMiniDumpSetComment)GetProcAddress(hSteamClient, "Breakpad_SteamWriteMiniDumpSetComment");

	if (oBreakpad_SteamWriteMiniDumpSetComment == nullptr)
		return;

	oBreakpad_SteamSetSteamID = (_Breakpad_SteamSetSteamID)GetProcAddress(hSteamClient, "Breakpad_SteamSetSteamID");

	if (oBreakpad_SteamSetSteamID == nullptr)
		return;

	oBreakpad_SteamSetAppID = (_Breakpad_SteamSetAppID)GetProcAddress(hSteamClient, "Breakpad_SteamSetAppID");

	if (oBreakpad_SteamSetAppID == nullptr)
		return;

	oBreakpad_SteamMiniDumpInit = (_Breakpad_SteamMiniDumpInit)GetProcAddress(hSteamClient, "Breakpad_SteamMiniDumpInit");

	if (oBreakpad_SteamMiniDumpInit == nullptr)
		return;

	_cprintf_s("[Steam_API_Base] Calling BreakpadMiniDumpSystemInit\r\n");

	oBreakpad_SteamMiniDumpInit(BreakpadAppID, szBreakpadVersion, nullptr, FullMemoryDumps, BreakpadContext, BreakpadMinidumpCallback);

	if (BreakpadSteamID != 0)
		SetMinidumpSteamID(BreakpadSteamID);

	return;
}

S_API void S_CALLTYPE SteamAPI_SetBreakpadAppID(uint32 unAppID)
{
	_cprintf_s("[Steam_API_Base] SteamAPI_SetBreakpadAppID\r\n");

	if (BreakpadAppID != unAppID)
	{
		_cprintf_s("[Steam_API_Base] Setting breakpad minidump AppID --> %u\r\n", unAppID);
		BreakpadAppID = unAppID;
	}

	if (unAppID != 0 && oBreakpad_SteamWriteMiniDumpUsingExceptionInfoWithBuildId == nullptr && UsingBreakpadCrashHandler == true)
	{
		_cprintf_s("[Steam_API_Base] Forcing breakpad minidump interfaces to load\r\n");
		LoadBreakpad();
	}

	if (oBreakpad_SteamSetAppID != nullptr)
		oBreakpad_SteamSetAppID(BreakpadAppID);

	return;
}

S_API void S_CALLTYPE SteamAPI_SetMiniDumpComment(const char *pchMsg)
{
	_cprintf_s("[Steam_API_Base] SteamAPI_SetMiniDumpComment\r\n");

	if (pchMsg != nullptr && strlen(pchMsg) != 0)
	{
		s_Win32MiniDump.SetComment(pchMsg);

		LoadBreakpad();

		if (oBreakpad_SteamWriteMiniDumpSetComment != nullptr)
			oBreakpad_SteamWriteMiniDumpSetComment(pchMsg);
	}
	return;
}

S_API void S_CALLTYPE SteamAPI_UseBreakpadCrashHandler(char const *pchVersion, char const *pchDate, char const *pchTime, bool bFullMemoryDumps, void *pvContext, PFNPreMinidumpCallback m_pfnPreMinidumpCallback)
{
	_cprintf_s("[Steam_API_Base] SteamAPI_UseBreakpadCrashHandler\r\n");

	_cprintf_s("[Steam_API_Base] Using breakpad crash handler\r\n");

	SecureZeroMemory(szBreakpadVersion, sizeof(szBreakpadVersion));
	BreakpadContext = nullptr;
	BreakpadMinidumpCallback = nullptr;

	UsingBreakpadCrashHandler = true;
	FullMemoryDumps = bFullMemoryDumps;

	if (pchVersion != nullptr && strlen(pchVersion) != 0)
		_snprintf_s(szBreakpadVersion, 64, _TRUNCATE, "%s", pchVersion);

	BreakpadContext = pvContext;
	BreakpadMinidumpCallback = m_pfnPreMinidumpCallback;

	return;
}

S_API void S_CALLTYPE SteamAPI_WriteMiniDump(uint32 uStructuredExceptionCode, void* pvExceptionInfo, uint32 uBuildID)
{
	_cprintf_s("[Steam_API_Base] SteamAPI_WriteMiniDump\r\n");

	if (pvExceptionInfo != nullptr)
	{
		s_Win32MiniDump.WriteUsingExceptionInfo(uStructuredExceptionCode, (_EXCEPTION_POINTERS*)pvExceptionInfo);
		s_Win32MiniDump.ClearComment();

		LoadBreakpad();

		if (oBreakpad_SteamWriteMiniDumpUsingExceptionInfoWithBuildId != nullptr)
			oBreakpad_SteamWriteMiniDumpUsingExceptionInfoWithBuildId(pvExceptionInfo, uBuildID);
	}
	return;
}