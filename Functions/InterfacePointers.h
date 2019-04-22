S_API ISteamAppList* S_CALLTYPE SteamAppList()
{
	_cprintf_s("[Steam_API_Base] SteamAppList\r\n");
	return GetInterfacePointers.SteamAppList();
}

S_API ISteamApps* S_CALLTYPE SteamApps()
{
	_cprintf_s("[Steam_API_Base] SteamApps\r\n");
	return GetInterfacePointers.SteamApps();
}

S_API ISteamClient* S_CALLTYPE SteamClient()
{
	_cprintf_s("[Steam_API_Base] SteamClient\r\n");
	return SteamClientSafe;
}

S_API ISteamController* S_CALLTYPE SteamController()
{
	_cprintf_s("[Steam_API_Base] SteamController\r\n");
	return GetInterfacePointers.SteamController();
}

S_API ISteamFriends* S_CALLTYPE SteamFriends()
{
	_cprintf_s("[Steam_API_Base] SteamFriends\r\n");
	return GetInterfacePointers.SteamFriends();
}

S_API ISteamGameSearch* S_CALLTYPE SteamGameSearch()
{
	_cprintf_s("[Steam_API_Base] SteamGameSearch\r\n");
	return GetInterfacePointers.SteamGameSearch();
}

S_API ISteamGameServer* S_CALLTYPE SteamGameServer()
{
	_cprintf_s("[Steam_API_Base] SteamGameServer\r\n");
	return GetGameServerInterfacePointers.SteamGameServer();
}

S_API ISteamApps* S_CALLTYPE SteamGameServerApps()
{
	_cprintf_s("[Steam_API_Base] SteamGameServerApps\r\n");
	return GetGameServerInterfacePointers.SteamApps();
}

S_API ISteamHTTP* S_CALLTYPE SteamGameServerHTTP()
{
	_cprintf_s("[Steam_API_Base] SteamGameServerHTTP\r\n");
	return GetGameServerInterfacePointers.SteamHTTP();
}

S_API ISteamInventory* S_CALLTYPE SteamGameServerInventory()
{
	_cprintf_s("[Steam_API_Base] SteamGameServerInventory\r\n");
	return GetGameServerInterfacePointers.SteamInventory();
}

S_API ISteamNetworking* S_CALLTYPE SteamGameServerNetworking()
{
	_cprintf_s("[Steam_API_Base] SteamGameServerNetworking\r\n");
	return GetGameServerInterfacePointers.SteamGameServerNetworking();
}

S_API ISteamGameServerStats* S_CALLTYPE SteamGameServerStats()
{
	_cprintf_s("[Steam_API_Base] SteamGameServerStats\r\n");
	return GetGameServerInterfacePointers.SteamGameServerStats();
}

S_API ISteamUGC* S_CALLTYPE SteamGameServerUGC()
{
	_cprintf_s("[Steam_API_Base] SteamGameServerUGC\r\n");
	return GetGameServerInterfacePointers.SteamUGC();
}

S_API ISteamUtils* S_CALLTYPE SteamGameServerUtils()
{
	_cprintf_s("[Steam_API_Base] SteamGameServerUtils\r\n");
	return GetGameServerInterfacePointers.SteamGameServerUtils();
}

S_API ISteamHTMLSurface* S_CALLTYPE SteamHTMLSurface()
{
	_cprintf_s("[Steam_API_Base] SteamHTMLSurface\r\n");
	return GetInterfacePointers.SteamHTMLSurface();
}

S_API ISteamHTTP* S_CALLTYPE SteamHTTP()
{
	_cprintf_s("[Steam_API_Base] SteamHTTP\r\n");
	return GetInterfacePointers.SteamHTTP();
}

S_API ISteamInventory* S_CALLTYPE SteamInventory()
{
	_cprintf_s("[Steam_API_Base] SteamInventory\r\n");
	return GetInterfacePointers.SteamInventory();
}

S_API ISteamMatchmaking* S_CALLTYPE SteamMatchmaking()
{
	_cprintf_s("[Steam_API_Base] SteamMatchmaking\r\n");
	return GetInterfacePointers.SteamMatchmaking();
}

S_API ISteamMatchmakingServers* S_CALLTYPE SteamMatchmakingServers()
{
	_cprintf_s("[Steam_API_Base] SteamMatchmakingServers\r\n");
	return GetInterfacePointers.SteamMatchmakingServers();
}

S_API ISteamMusic* S_CALLTYPE SteamMusic()
{
	_cprintf_s("[Steam_API_Base] SteamMusic\r\n");
	return GetInterfacePointers.SteamMusic();
}

S_API ISteamMusicRemote* S_CALLTYPE SteamMusicRemote()
{
	_cprintf_s("[Steam_API_Base] SteamMusicRemote\r\n");
	return GetInterfacePointers.SteamMusicRemote();
}

S_API ISteamNetworking* S_CALLTYPE SteamNetworking()
{
	_cprintf_s("[Steam_API_Base] SteamNetworking\r\n");
	return GetInterfacePointers.SteamNetworking();
}

S_API ISteamRemoteStorage* S_CALLTYPE SteamRemoteStorage()
{
	_cprintf_s("[Steam_API_Base] SteamRemoteStorage\r\n");
	return GetInterfacePointers.SteamRemoteStorage();
}

S_API ISteamScreenshots* S_CALLTYPE SteamScreenshots()
{
	_cprintf_s("[Steam_API_Base] SteamScreenshots\r\n");
	return GetInterfacePointers.SteamScreenshots();
}

S_API ISteamUGC* S_CALLTYPE SteamUGC()
{
	_cprintf_s("[Steam_API_Base] SteamUGC\r\n");
	return GetInterfacePointers.SteamUGC();
}

S_API ISteamUser* S_CALLTYPE SteamUser()
{
	_cprintf_s("[Steam_API_Base] SteamUser\r\n");
	return GetInterfacePointers.SteamUser();
}

S_API ISteamUserStats* S_CALLTYPE SteamUserStats()
{
	_cprintf_s("[Steam_API_Base] SteamUserStats\r\n");
	return GetInterfacePointers.SteamUserStats();
}

S_API ISteamUtils* S_CALLTYPE SteamUtils()
{
	_cprintf_s("[Steam_API_Base] SteamUtils\r\n");
	return GetInterfacePointers.SteamUtils();
}

S_API void* S_CALLTYPE SteamUnifiedMessages()
{
	MessageBoxA(nullptr, "SteamUnifiedMessages is removed in new SDK versions!", "Steam API Base", (MB_OK | MB_ICONERROR | MB_DEFBUTTON1 | MB_SETFOREGROUND));
	return nullptr;
}

S_API ISteamVideo* S_CALLTYPE SteamVideo()
{
	_cprintf_s("[Steam_API_Base] SteamVideo\r\n");
	return GetInterfacePointers.SteamVideo();
}

S_API ISteamParentalSettings* S_CALLTYPE SteamParentalSettings()
{
	_cprintf_s("[Steam_API_Base] SteamParentalSettings\r\n");
	return GetInterfacePointers.SteamParentalSettings();
}

S_API ISteamInput* S_CALLTYPE SteamInput()
{
	_cprintf_s("[Steam_API_Base] SteamInput\r\n");
	return GetInterfacePointers.SteamInput();
}

S_API ISteamParties* S_CALLTYPE SteamParties()
{
	_cprintf_s("[Steam_API_Base] SteamParties\r\n");
	return GetInterfacePointers.SteamParties();
}