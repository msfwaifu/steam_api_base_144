//====== Copyright Valve Corporation, All rights reserved. ====================
//
// This header includes *all* of the interfaces and callback structures
// in the Steamworks SDK, and some high level functions to control the SDK
// (init, shutdown, etc) that you probably only need in one or two files.
//
// To save your compile times, we recommend that you not include this file
// in header files.  Instead, include the specific headers for the interfaces
// and callback structures you need.  The one file you might consider including
// in your precompiled header (e.g. stdafx.h) is steam_api_common.h
//
//=============================================================================

#ifndef STEAM_API_H
#define STEAM_API_H
#ifdef _WIN32
#pragma once
#endif

// Basic stuff
#include "steam_api_common.h"

// All of the interfaces
#include "isteamclient.h"
#include "isteamuser.h"
#include "isteamfriends.h"
#include "isteamutils.h"
#include "isteammatchmaking.h"
#include "isteamuserstats.h"
#include "isteamapps.h"
#include "isteamnetworking.h"
#include "isteamremotestorage.h"
#include "isteamscreenshots.h"
#include "isteammusic.h"
#include "isteammusicremote.h"
#include "isteamhttp.h"
#include "isteamcontroller.h"
#include "isteamugc.h"
#include "isteamapplist.h"
#include "isteamhtmlsurface.h"
#include "isteaminventory.h"
#include "isteamvideo.h"
#include "isteamparentalsettings.h"
#include "isteaminput.h"


//----------------------------------------------------------------------------------------------------------------------------------------------------------//
//	Steam API setup & shutdown
//
//	These functions manage loading, initializing and shutdown of the steamclient.dll
//
//----------------------------------------------------------------------------------------------------------------------------------------------------------//


// SteamAPI_Init must be called before using any other API functions. If it fails, an
// error message will be output to the debugger (or stderr) with further information.
S_API bool S_CALLTYPE SteamAPI_Init();

// SteamAPI_Shutdown should be called during process shutdown if possible.
S_API void S_CALLTYPE SteamAPI_Shutdown();

// SteamAPI_RestartAppIfNecessary ensures that your executable was launched through Steam.
//
// Returns true if the current process should terminate. Steam is now re-launching your application.
//
// Returns false if no action needs to be taken. This means that your executable was started through
// the Steam client, or a steam_appid.txt file is present in your game's directory (for development).
// Your current process should continue if false is returned.
//
// NOTE: If you use the Steam DRM wrapper on your primary executable file, this check is unnecessary
// since the DRM wrapper will ensure that your application was launched properly through Steam.
S_API bool S_CALLTYPE SteamAPI_RestartAppIfNecessary( uint32 unOwnAppID );

// Many Steam API functions allocate a small amount of thread-local memory for parameter storage.
// SteamAPI_ReleaseCurrentThreadMemory() will free API memory associated with the calling thread.
// This function is also called automatically by SteamAPI_RunCallbacks(), so a single-threaded
// program never needs to explicitly call this function.
S_API void S_CALLTYPE SteamAPI_ReleaseCurrentThreadMemory();


// crash dump recording functions
S_API void S_CALLTYPE SteamAPI_WriteMiniDump( uint32 uStructuredExceptionCode, void* pvExceptionInfo, uint32 uBuildID );
S_API void S_CALLTYPE SteamAPI_SetMiniDumpComment( const char *pchMsg );

//----------------------------------------------------------------------------------------------------------------------------------------------------------//
//	steamclient.dll private wrapper functions
//
//	The following functions are part of abstracting API access to the steamclient.dll, but should only be used in very specific cases
//----------------------------------------------------------------------------------------------------------------------------------------------------------//

// SteamAPI_IsSteamRunning() returns true if Steam is currently running
S_API bool S_CALLTYPE SteamAPI_IsSteamRunning();

// Pumps out all the steam messages, calling registered callbacks.
// NOT THREADSAFE - do not call from multiple threads simultaneously.
S_API void Steam_RunCallbacks( HSteamPipe hSteamPipe, bool bGameServerCallbacks );

// register the callback funcs to use to interact with the steam dll
S_API void Steam_RegisterInterfaceFuncs( void *hModule );

// returns the HSteamUser of the last user to dispatch a callback
S_API HSteamUser Steam_GetHSteamUserCurrent();

// returns the filename path of the current running Steam process, used if you need to load an explicit steam dll by name.
// DEPRECATED - implementation is Windows only, and the path returned is a UTF-8 string which must be converted to UTF-16 for use with Win32 APIs
S_API const char *SteamAPI_GetSteamInstallPath();

// sets whether or not Steam_RunCallbacks() should do a try {} catch (...) {} around calls to issuing callbacks
S_API void SteamAPI_SetTryCatchCallbacks( bool bTryCatchCallbacks );

// backwards compat export, passes through to SteamAPI_ variants
S_API HSteamPipe GetHSteamPipe();
S_API HSteamUser GetHSteamUser();


#if defined( VERSION_SAFE_STEAM_API_INTERFACES )
// exists only for backwards compat with code written against older SDKs
S_API bool S_CALLTYPE SteamAPI_InitSafe();
#endif

#if defined(USE_BREAKPAD_HANDLER) || defined(STEAM_API_EXPORTS)
// this should be called before the game initialized the steam APIs
// pchDate should be of the format "Mmm dd yyyy" (such as from the __ DATE __ macro )
// pchTime should be of the format "hh:mm:ss" (such as from the __ TIME __ macro )
// bFullMemoryDumps (Win32 only) -- writes out a uuid-full.dmp in the client/dumps folder
// pvContext-- can be NULL, will be the void * context passed into m_pfnPreMinidumpCallback
// PFNPreMinidumpCallback m_pfnPreMinidumpCallback   -- optional callback which occurs just before a .dmp file is written during a crash.  Applications can hook this to allow adding additional information into the .dmp comment stream.
S_API void S_CALLTYPE SteamAPI_UseBreakpadCrashHandler( char const *pchVersion, char const *pchDate, char const *pchTime, bool bFullMemoryDumps, void *pvContext, PFNPreMinidumpCallback m_pfnPreMinidumpCallback );
S_API void S_CALLTYPE SteamAPI_SetBreakpadAppID( uint32 unAppID );
#endif


//----------------------------------------------------------------------------------------------------------------------------------------------------------//
//
//	CSteamAPIContext
//
//----------------------------------------------------------------------------------------------------------------------------------------------------------//

//#ifndef STEAM_API_EXPORTS

// Deprecated!  Use the global accessors directly
inline bool CSteamAPIContext::Init()
{
	HSteamUser hSteamUser = SteamAPI_GetHSteamUser();
	HSteamPipe hSteamPipe = SteamAPI_GetHSteamPipe();
	if (!hSteamPipe)
		return false;

	m_pSteamClient = (ISteamClient*)SteamInternal_CreateInterface(STEAMCLIENT_INTERFACE_VERSION);
	if (!m_pSteamClient)
		return false;

	m_pSteamUser = m_pSteamClient->GetISteamUser(hSteamUser, hSteamPipe, STEAMUSER_INTERFACE_VERSION);
	if (!m_pSteamUser)
		return false;

	m_pSteamFriends = m_pSteamClient->GetISteamFriends(hSteamUser, hSteamPipe, STEAMFRIENDS_INTERFACE_VERSION);
	if (!m_pSteamFriends)
		return false;

	m_pSteamUtils = m_pSteamClient->GetISteamUtils(hSteamPipe, STEAMUTILS_INTERFACE_VERSION);
	if (!m_pSteamUtils)
		return false;

	m_pSteamMatchmaking = m_pSteamClient->GetISteamMatchmaking(hSteamUser, hSteamPipe, STEAMMATCHMAKING_INTERFACE_VERSION);
	if (!m_pSteamMatchmaking)
		return false;

	m_pSteamGameSearch = m_pSteamClient->GetISteamGameSearch(hSteamUser, hSteamPipe, STEAMGAMESEARCH_INTERFACE_VERSION);
	if (!m_pSteamGameSearch)
		return false;

	m_pSteamMatchmakingServers = m_pSteamClient->GetISteamMatchmakingServers(hSteamUser, hSteamPipe, STEAMMATCHMAKINGSERVERS_INTERFACE_VERSION);
	if (!m_pSteamMatchmakingServers)
		return false;

	m_pSteamUserStats = m_pSteamClient->GetISteamUserStats(hSteamUser, hSteamPipe, STEAMUSERSTATS_INTERFACE_VERSION);
	if (!m_pSteamUserStats)
		return false;

	m_pSteamApps = m_pSteamClient->GetISteamApps(hSteamUser, hSteamPipe, STEAMAPPS_INTERFACE_VERSION);
	if (!m_pSteamApps)
		return false;

	m_pSteamNetworking = m_pSteamClient->GetISteamNetworking(hSteamUser, hSteamPipe, STEAMNETWORKING_INTERFACE_VERSION);
	if (!m_pSteamNetworking)
		return false;

	m_pSteamRemoteStorage = m_pSteamClient->GetISteamRemoteStorage(hSteamUser, hSteamPipe, STEAMREMOTESTORAGE_INTERFACE_VERSION);
	if (!m_pSteamRemoteStorage)
		return false;

	m_pSteamScreenshots = m_pSteamClient->GetISteamScreenshots(hSteamUser, hSteamPipe, STEAMSCREENSHOTS_INTERFACE_VERSION);
	if (!m_pSteamScreenshots)
		return false;

	m_pSteamHTTP = m_pSteamClient->GetISteamHTTP(hSteamUser, hSteamPipe, STEAMHTTP_INTERFACE_VERSION);
	if (!m_pSteamHTTP)
		return false;

	m_pController = m_pSteamClient->GetISteamController(hSteamUser, hSteamPipe, STEAMCONTROLLER_INTERFACE_VERSION);
	if (!m_pController)
		return false;

	m_pSteamUGC = m_pSteamClient->GetISteamUGC(hSteamUser, hSteamPipe, STEAMUGC_INTERFACE_VERSION);
	if (!m_pSteamUGC)
		return false;

	m_pSteamAppList = m_pSteamClient->GetISteamAppList(hSteamUser, hSteamPipe, STEAMAPPLIST_INTERFACE_VERSION);
	if (!m_pSteamAppList)
		return false;

	m_pSteamMusic = m_pSteamClient->GetISteamMusic(hSteamUser, hSteamPipe, STEAMMUSIC_INTERFACE_VERSION);
	if (!m_pSteamMusic)
		return false;

	m_pSteamMusicRemote = m_pSteamClient->GetISteamMusicRemote(hSteamUser, hSteamPipe, STEAMMUSICREMOTE_INTERFACE_VERSION);
	if (!m_pSteamMusicRemote)
		return false;

	m_pSteamHTMLSurface = m_pSteamClient->GetISteamHTMLSurface(hSteamUser, hSteamPipe, STEAMHTMLSURFACE_INTERFACE_VERSION);
	if (!m_pSteamHTMLSurface)
		return false;

	m_pSteamInventory = m_pSteamClient->GetISteamInventory(hSteamUser, hSteamPipe, STEAMINVENTORY_INTERFACE_VERSION);
	if (!m_pSteamInventory)
		return false;

	m_pSteamVideo = m_pSteamClient->GetISteamVideo(hSteamUser, hSteamPipe, STEAMVIDEO_INTERFACE_VERSION);
	if (!m_pSteamVideo)
		return false;

	m_pSteamInput = m_pSteamClient->GetISteamInput(hSteamUser, hSteamPipe, STEAMINPUT_INTERFACE_VERSION);
	if (!m_pSteamInput)
		return false;

	m_pSteamParties = m_pSteamClient->GetISteamParties(hSteamUser, hSteamPipe, STEAMPARTIES_INTERFACE_VERSION);
	if (!m_pSteamParties)
		return false;

	return true;
}

//#endif

#endif // STEAM_API_H
