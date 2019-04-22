S_API bool S_CALLTYPE SteamInternal_GameServer_Init(uint32 unIP, uint16 usPort, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString)
{
	_cprintf_s("[Steam_API_Base] SteamInternal_GameServer_Init\r\n");

	eGameServerMode = eServerMode;

	SteamClient_Server = static_cast<ISteamClient*>(InternalAPI_Init(&hSteamclient_Server, true, STEAMCLIENT_INTERFACE_VERSION));

	if (SteamClient_Server == nullptr)
		return false;

	SteamClient_Server->SetLocalIPBinding(unIP, usPort);
	hUserServer = SteamClient_Server->CreateLocalUser(&hPipeServer, k_EAccountTypeGameServer);

	if (hUserServer != 0 && hPipeServer != 0)
	{
		pSteamGameServer = (ISteamGameServer *)SteamClient_Server->GetISteamGameServer(hUserServer, hPipeServer, STEAMGAMESERVER_INTERFACE_VERSION);

		if (pSteamGameServer != nullptr)
		{
			ISteamUtils *pSteamUtils = (ISteamUtils*)SteamClient_Server->GetISteamUtils(hPipeServer, STEAMUTILS_INTERFACE_VERSION);

			if (pSteamUtils != nullptr)
			{
				uint32 unServerFlags = k_unServerFlagNone;

				if (eGameServerMode == eServerModeAuthenticationAndSecure)
					unServerFlags |= k_unServerFlagSecure;

				if (eGameServerMode == eServerModeNoAuthentication)
					unServerFlags |= k_unServerFlagPrivate;

				uint32 AppID = pSteamUtils->GetAppID();

				if (AppID != 0)
				{
					bool InitServer = pSteamGameServer->InitGameServer(unIP, usGamePort, usQueryPort, unServerFlags, AppID, pchVersionString);

					if (InitServer == true)
					{
						SteamUtilsServer = pSteamUtils;
						Steam_RegisterInterfaceFuncs(hSteamclient_Server);
						SteamAPI_SetBreakpadAppID(AppID);
						LoadBreakpad();

						InitGameServerPointers = GetGameServerInterfacePointers.Init();

						if (InitGameServerPointers == false)
						{
							_cprintf_s("[Steam_API_Base] GetGameServerInterfacePointers.Init failed!\r\n");

							SteamGameServer_Shutdown();
							return false;
						}

						g_pSteamClientGameServer_Latest = SteamClient_Server;
						return true;
					}
				}
			}
		}
	}
	_cprintf_s("[Steam_API_Base] SteamInternal_GameServer_Init failed!\r\n");
	SteamGameServer_Shutdown();
	return false;
}

S_API bool S_CALLTYPE SteamGameServer_InitSafe(uint32 unIP, uint16 usPort, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString)
{
	_cprintf_s("[Steam_API_Base] SteamGameServer_InitSafe\r\n");

	if (SteamInternal_GameServer_Init(unIP, usPort, usGamePort, usQueryPort, eServerMode, pchVersionString) == true && SteamClientSafe != nullptr)
	{
		g_pSteamClientGameServer = SteamClientSafe;
		return true;
	}

	_cprintf_s("[Steam_API_Base] SteamGameServer_InitSafe failed!\r\n");

	return false;
}

S_API bool S_CALLTYPE SteamGameServer_BSecure()
{
	_cprintf_s("[Steam_API_Base] SteamGameServer_BSecure\r\n");

	if (eGameServerMode == eServerModeNoAuthentication)
		return false;

	if (pSteamGameServer == nullptr)
		return false;

	return pSteamGameServer->BSecure();
}

S_API HSteamPipe S_CALLTYPE SteamGameServer_GetHSteamPipe()
{
	_cprintf_s("[Steam_API_Base] SteamGameServer_GetHSteamPipe\r\n");
	return hPipeServer;
}

S_API HSteamUser S_CALLTYPE SteamGameServer_GetHSteamUser()
{
	_cprintf_s("[Steam_API_Base] SteamGameServer_GetHSteamUser\r\n");
	return hUserServer;
}

S_API uint32 S_CALLTYPE SteamGameServer_GetIPCCallCount()
{
	_cprintf_s("[Steam_API_Base] SteamGameServer_GetIPCCallCount\r\n");

	if (SteamUtilsServer == nullptr)
		return false;

	return SteamUtilsServer->GetIPCCallCount();
}

S_API uint64 S_CALLTYPE SteamGameServer_GetSteamID()
{
	_cprintf_s("[Steam_API_Base] SteamGameServer_GetSteamID\r\n");

	if (eGameServerMode == eServerModeNoAuthentication)
		return *(uint64*)&k_steamIDLanModeGS;

	if (pSteamGameServer == nullptr)
		return *(uint64*)&k_steamIDNotInitYetGS;

	return pSteamGameServer->GetSteamID().ConvertToUint64();
}

S_API void S_CALLTYPE SteamGameServer_RunCallbacks()
{
	_cprintf_s("[Steam_API_Base] SteamGameServer_RunCallbacks\r\n");

	if (hPipeServer != 0)
		return Steam_RunCallbacks(hPipeServer, true);

	return;
}

S_API ISteamClient * g_pSteamClientGameServer = nullptr;
S_API ISteamClient * g_pSteamClientGameServer_Latest = nullptr;