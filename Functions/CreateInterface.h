S_API void* S_CALLTYPE SteamInternal_CreateInterface(const char *ver)
{
	if (ver != nullptr)
	{
		_cprintf_s("[Steam_API_Base] SteamInternal_CreateInterface --> %s\r\n", ver);

		HMODULE hDll = hSteamclient_Client;

		if (hSteamclient_Server != nullptr)
			hDll = hSteamclient_Server;

		if (hDll != nullptr)
		{
			oCreateInterface = (_CreateInterface)GetProcAddress(hDll, "CreateInterface");

			if (oCreateInterface != nullptr)
				return oCreateInterface(ver, nullptr);
		}
	}

	_cprintf_s("[Steam_API_Base] SteamInternal_CreateInterface --> return nullptr!\r\n");

	return nullptr;
}

S_API void* S_CALLTYPE SteamGameServerInternal_CreateInterface(const char *Interface)
{
	if (Interface != nullptr)
	{
		_cprintf_s("[Steam_API_Base] SteamGameServerInternal_CreateInterface --> %s\r\n", Interface);

		if (hSteamclient_Server != nullptr)
		{
			oCreateInterface = (_CreateInterface)GetProcAddress(hSteamclient_Server, "CreateInterface");

			if (oCreateInterface != nullptr)
				return oCreateInterface(Interface, nullptr);
		}
	}

	_cprintf_s("[Steam_API_Base] SteamGameServerInternal_CreateInterface --> return nullptr!\r\n");

	return nullptr;
}

S_API void* S_CALLTYPE SteamInternal_FindOrCreateUserInterface(HSteamUser hSteamUser, const char *pszVersion)
{
	if (pszVersion != nullptr)
	{
		if (SteamClient_Client != nullptr && hPipe != 0)
			return SteamClient_Client->GetISteamGenericInterface(hSteamUser, hPipe, pszVersion);

		_cprintf_s("[Steam_API_Base] [S_API FAIL] Tried to access Steam interface %s before SteamAPI_Init succeeded!\r\n", pszVersion);
	}
	return nullptr;
}

S_API void* S_CALLTYPE SteamInternal_FindOrCreateGameServerInterface(HSteamUser hSteamUser, const char *pszVersion)
{
	if (pszVersion != nullptr)
	{
		if (SteamClient_Server != nullptr && hPipeServer != 0)
			return SteamClient_Server->GetISteamGenericInterface(hSteamUser, hPipeServer, pszVersion);

		_cprintf_s("[Steam_API_Base] [S_API FAIL] Tried to access Steam interface %s before SteamInternal_GameServer_Init succeeded!\r\n", pszVersion);
	}
	return nullptr;
}