extern bool bCallbackManagerInitialized;
extern CCallbackMgr* GCallbackMgr();
util::lock::SpinLock Lock;
bool CallbackDone = false;

S_API void S_CALLTYPE SteamAPI_RegisterCallResult(class CCallbackBase *pCallback, SteamAPICall_t hAPICall)
{
	_cprintf_s("[Steam_API_Base] SteamAPI_RegisterCallResult\r\n");

	CCallbackMgr* pCallbackManager = GCallbackMgr();
	return pCallbackManager->RegisterCallResult(pCallback, hAPICall);
}

S_API void S_CALLTYPE SteamAPI_RegisterCallback(class CCallbackBase *pCallback, int32 iCallback)
{
	_cprintf_s("[Steam_API_Base] SteamAPI_RegisterCallback\r\n");

	CCallbackMgr* pCallbackManager = GCallbackMgr();
	return pCallbackManager->RegisterCallback(pCallback, iCallback);
}

S_API void S_CALLTYPE SteamAPI_UnregisterCallResult(class CCallbackBase *pCallback, SteamAPICall_t hAPICall)
{
	_cprintf_s("[Steam_API_Base] SteamAPI_UnregisterCallResult\r\n");

	if (bCallbackManagerInitialized == true)
	{
		CCallbackMgr* pCallbackManager = GCallbackMgr();
		return pCallbackManager->UnregisterCallResult(pCallback, hAPICall);
	}
	return;
}

S_API void S_CALLTYPE SteamAPI_UnregisterCallback(class CCallbackBase *pCallback)
{
	_cprintf_s("[Steam_API_Base] SteamAPI_UnregisterCallback\r\n");

	if (bCallbackManagerInitialized == true)
	{
		CCallbackMgr* pCallbackManager = GCallbackMgr();
		return pCallbackManager->UnregisterCallback(pCallback);
	}
	return;
}

S_API void S_CALLTYPE SteamAPI_RunCallbacks()
{
	if (hPipe == 0)
		goto PipeFail;

	if (Lock.IsLocked() == true)
	{
		Lock.Unlock();

		while (1)
		{
			if (Lock.IsLocked() == false)
				break;
		}
	}

	Lock.Lock();

	if (Lock.IsLocked() == true)
	{
		_cprintf_s("[Steam_API_Base] SteamAPI_RunCallbacks\r\n");

		Steam_RunCallbacks(hPipe, false);
		CallbackDone = true;
	}

	Lock.Unlock();

	if (CallbackDone == false)
	{
	PipeFail:
		if (oReleaseThreadLocalMemory != nullptr)
			oReleaseThreadLocalMemory(0);
	}

	CallbackDone = false;

	return;
}

S_API void S_CALLTYPE SteamAPI_SetTryCatchCallbacks(bool bTryCatchCallbacks)
{
	_cprintf_s("[Steam_API_Base] SteamAPI_SetTryCatchCallbacks\r\n");
	TryCatch = bTryCatchCallbacks;

	return;
}

S_API HSteamUser S_CALLTYPE Steam_GetHSteamUserCurrent()
{
	_cprintf_s("[Steam_API_Base] Steam_GetHSteamUserCurrent\r\n");

	CCallbackMgr* pCallbackManager = GCallbackMgr();
	return pCallbackManager->HSteamUserCurrent;
}

S_API void S_CALLTYPE Steam_RegisterInterfaceFuncs(void *hModule)
{
	_cprintf_s("[Steam_API_Base] Steam_RegisterInterfaceFuncs\r\n");

	CCallbackMgr* pCallbackManager = GCallbackMgr();
	pCallbackManager->oSteam_BGetCallback = _Steam_BGetCallback(GetProcAddress((HMODULE)hModule, "Steam_BGetCallback"));
	pCallbackManager->oSteam_FreeLastCallback = _Steam_FreeLastCallback(GetProcAddress((HMODULE)hModule, "Steam_FreeLastCallback"));
	pCallbackManager->oSteam_GetAPICallResult = _Steam_GetAPICallResult(GetProcAddress((HMODULE)hModule, "Steam_GetAPICallResult"));

	return;
}

S_API void S_CALLTYPE Steam_RunCallbacks(HSteamPipe hSteamPipe, bool bGameServerCallbacks)
{
	_cprintf_s("[Steam_API_Base] Steam_RunCallbacks\r\n");

	if (hSteamPipe == 0)
		return;

	if (bGameServerCallbacks == false && SteamClient_Client != nullptr && hSteamPipe == hPipe)
	{
		if (SteamUtilsForCallbacks == nullptr)
		{
			SteamUtilsForCallbacks = (ISteamUtils*)SteamClient_Client->GetISteamUtils(hPipe, STEAMUTILS_INTERFACE_VERSION);

			if (SteamUtilsForCallbacks != nullptr)
			{
				SteamUtilsForCallbacks->GetAppID();
				SteamUtilsForCallbacks->RunFrame();
			}
		}
		else
		{
			SteamUtilsForCallbacks->GetAppID();
			SteamUtilsForCallbacks->RunFrame();
		}

		if (SteamControllerForCallbacks == nullptr)
		{
			SteamControllerForCallbacks = (ISteamController*)SteamClient_Client->GetISteamController(hUser, hPipe, STEAMCONTROLLER_INTERFACE_VERSION);

			if (SteamControllerForCallbacks != nullptr)
				SteamControllerForCallbacks->RunFrame();
		}
		else
		{
			SteamControllerForCallbacks->RunFrame();
		}

		if (SteamInputForCallbacks == nullptr)
		{
			SteamInputForCallbacks = (ISteamInput*)SteamClient_Client->GetISteamInput(hUser, hPipe, STEAMINPUT_INTERFACE_VERSION);

			if (SteamInputForCallbacks != nullptr)
				SteamInputForCallbacks->RunFrame();
		}
		else
		{
			SteamInputForCallbacks->RunFrame();
		}
	}

	CCallbackMgr* pCallbackManager = GCallbackMgr();

	if (TryCatch == true)
		return pCallbackManager->RunCallbacksTryCatch(hSteamPipe, bGameServerCallbacks);

	return pCallbackManager->RunCallbacks(hSteamPipe, bGameServerCallbacks);
}