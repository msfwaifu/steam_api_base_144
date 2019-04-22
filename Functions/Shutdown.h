S_API void S_CALLTYPE SteamAPI_Shutdown()
{
	_cprintf_s("[Steam_API_Base] SteamAPI_Shutdown\r\n");

	SteamUtilsForCallbacks = nullptr;
	SteamControllerForCallbacks = nullptr;
	SteamInputForCallbacks = nullptr;

	if (SteamClient_Client != nullptr)
	{
		if (hUser != 0)
		{
			SteamClient_Client->ReleaseUser(hPipe, hUser);
			hUser = 0;
		}

		if (hPipe != 0)
		{
			SteamClient_Client->BReleaseSteamPipe(hPipe);
			hPipe = 0;
		}

		oReleaseThreadLocalMemory = nullptr;

		SteamClient_Client->BShutdownIfAllPipesClosed();

		SteamClient_Client = nullptr;

		SteamClientSafe = nullptr;

		oBreakpad_SteamWriteMiniDumpUsingExceptionInfoWithBuildId = nullptr;
		oBreakpad_SteamWriteMiniDumpSetComment = nullptr;

		FreeLibrary(hSteamclient_Client);
		ContextCounter++;

		hSteamclient_Client = nullptr;

		if (InitClientPointers == true)
			GetInterfacePointers.Clear();

		InitClientPointers = false;
	}
	return;
}

S_API void S_CALLTYPE SteamGameServer_Shutdown()
{
	_cprintf_s("[Steam_API_Base] SteamGameServer_Shutdown\r\n");

	if (pSteamGameServer != nullptr)
	{
		if (pSteamGameServer->BLoggedOn() == true)
		{
			pSteamGameServer->LogOff();

			while (1)
			{
				if (pSteamGameServer->BLoggedOn() == false)
					break;
			}
		}
	}

	if (SteamClient_Server != nullptr)
	{
		if (hPipeServer != 0 && hUserServer != 0)
		{
			SteamClient_Server->ReleaseUser(hPipeServer, hUserServer);
			SteamClient_Server->BReleaseSteamPipe(hPipeServer);

			hUserServer = 0;
			hPipeServer = 0;
		}

		pSteamGameServer = nullptr;

		SteamClient_Server->BShutdownIfAllPipesClosed();

		SteamClient_Server = nullptr;
		SteamUtilsServer = nullptr;
		g_pSteamClientGameServer = nullptr;
		g_pSteamClientGameServer_Latest = nullptr;

		FreeLibrary(hSteamclient_Server);
		ContextCounter++;

		hSteamclient_Server = nullptr;

		if (InitGameServerPointers == true)
			GetGameServerInterfacePointers.Clear();

		InitGameServerPointers = false;
	}
	return;
}