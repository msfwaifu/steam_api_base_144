#include <Windows.h>
#include <map>
#include <conio.h>
#include "Steam\steam_api.h"
#include "Steam\steamclientpublic.h"
#include "CallbackManager.h"

bool bCallbackManagerInitialized = false;

CCallbackMgr::CCallbackMgr()
{
	this->oSteam_BGetCallback = nullptr;
	this->oSteam_FreeLastCallback = nullptr;
	this->oSteam_GetAPICallResult = nullptr;
	this->HSteamUserCurrent = 0;
	bCallbackManagerInitialized = true;
}

CCallbackMgr::~CCallbackMgr()
{
	this->oSteam_BGetCallback = nullptr;
	this->oSteam_FreeLastCallback = nullptr;
	this->oSteam_GetAPICallResult = nullptr;
	this->HSteamUserCurrent = 0;
	bCallbackManagerInitialized = false;
}

void CCallbackMgr::RunCallResult(HSteamPipe hSteamPipe, SteamAPICall_t APICall, CCallbackBase* pCallback)
{
	if (pCallback != nullptr)
	{
		char *CallResultBuffer = new char[pCallback->GetCallbackSizeBytes()]();

		bool pbFailed = false;
		bool bAPICall = this->oSteam_GetAPICallResult(hSteamPipe, APICall, CallResultBuffer, pCallback->GetCallbackSizeBytes(), pCallback->GetICallback(), &pbFailed);

		if (bAPICall == true && pbFailed == false)
		{
			_cprintf_s("[Steam_API_Base] Running Callresult --> %lld --> %d\r\n", APICall, pCallback->GetICallback());

			size_t OriginalCallbackMapSize = this->MapCallbacks.size();

			pCallback->Run(CallResultBuffer, pbFailed, APICall);

			if (OriginalCallbackMapSize != this->MapCallbacks.size())
				pCallback->Run(CallResultBuffer);
		}
		delete[]CallResultBuffer;
	}
	return;
}

void CCallbackMgr::RunCallbacks(HSteamPipe hSteamPipe, bool bGameServerCallbacks)
{
	if (this->oSteam_BGetCallback != nullptr && this->oSteam_FreeLastCallback != nullptr && this->oSteam_GetAPICallResult != nullptr)
	{
		CallbackMsg_t CallbackMsg = { 0 };

		if (this->oSteam_BGetCallback(hSteamPipe, &CallbackMsg))
		{
			_cprintf_s("[Steam_API_Base] Callback --> %d\r\n", CallbackMsg.m_iCallback);

			this->HSteamUserCurrent = CallbackMsg.m_hSteamUser;

			if (CallbackMsg.m_iCallback == SteamAPICallCompleted_t::k_iCallback)
			{
				SteamAPICallCompleted_t* SteamAPICallCompleted = (SteamAPICallCompleted_t*)CallbackMsg.m_pubParam;

				if (this->MapAPICalls.size() != 0)
				{
					std::multimap<SteamAPICall_t, CCallbackBase*>::iterator mapCallResultIterator;

					for (mapCallResultIterator = this->MapAPICalls.begin(); mapCallResultIterator != this->MapAPICalls.end(); mapCallResultIterator++)
					{
						if (SteamAPICallCompleted->m_hAsyncCall == mapCallResultIterator->first && 
							SteamAPICallCompleted->m_iCallback == mapCallResultIterator->second->GetICallback() && 
							SteamAPICallCompleted->m_cubParam == mapCallResultIterator->second->GetCallbackSizeBytes())
						{
							this->RunCallResult(hSteamPipe, mapCallResultIterator->first, mapCallResultIterator->second);
						}
					}
				}
			}

			if (this->MapCallbacks.size() != 0)
			{
				std::multimap<int, class CCallbackBase*>::iterator mapCallbacksIterator;

				for (mapCallbacksIterator = this->MapCallbacks.begin(); mapCallbacksIterator != this->MapCallbacks.end(); mapCallbacksIterator++)
				{
					CCallbackBase* pCallback = mapCallbacksIterator->second;
					if (mapCallbacksIterator->first == CallbackMsg.m_iCallback && ((pCallback->m_nCallbackFlags & pCallback->k_ECallbackFlagsGameServer) >> 1) == (int)bGameServerCallbacks)
					{
						_cprintf_s("[Steam_API_Base] Running Callback --> %d\r\n", CallbackMsg.m_iCallback);

						pCallback->Run(CallbackMsg.m_pubParam);
						this->oSteam_FreeLastCallback(hSteamPipe);
						return;
					}
				}
			}
			_cprintf_s("[Steam_API_Base] Freeing Callback --> %d\r\n", CallbackMsg.m_iCallback);
			this->oSteam_FreeLastCallback(hSteamPipe);
		}
	}
}

void CCallbackMgr::RunCallbacksTryCatch(HSteamPipe hSteamPipe, bool bGameServerCallbacks)
{
	try
	{
		if (this->oSteam_BGetCallback != nullptr && this->oSteam_FreeLastCallback != nullptr && this->oSteam_GetAPICallResult != nullptr)
		{
			CallbackMsg_t CallbackMsg = { 0 };

			if (this->oSteam_BGetCallback(hSteamPipe, &CallbackMsg))
			{
				_cprintf_s("[Steam_API_Base] Callback (TryCatch) --> %d\r\n", CallbackMsg.m_iCallback);

				this->HSteamUserCurrent = CallbackMsg.m_hSteamUser;

				if (CallbackMsg.m_iCallback == SteamAPICallCompleted_t::k_iCallback)
				{
					SteamAPICallCompleted_t* SteamAPICallCompleted = (SteamAPICallCompleted_t*)CallbackMsg.m_pubParam;

					if (this->MapAPICalls.size() != 0)
					{
						std::multimap<SteamAPICall_t, CCallbackBase*>::iterator mapCallResultIterator;

						for (mapCallResultIterator = this->MapAPICalls.begin(); mapCallResultIterator != this->MapAPICalls.end(); mapCallResultIterator++)
						{
							if (SteamAPICallCompleted->m_hAsyncCall == mapCallResultIterator->first &&
								SteamAPICallCompleted->m_iCallback == mapCallResultIterator->second->GetICallback() &&
								SteamAPICallCompleted->m_cubParam == mapCallResultIterator->second->GetCallbackSizeBytes())
							{
								this->RunCallResult(hSteamPipe, mapCallResultIterator->first, mapCallResultIterator->second);
							}
						}
					}
				}

				if (this->MapCallbacks.size() != 0)
				{
					std::multimap<int, class CCallbackBase*>::iterator mapCallbacksIterator;

					for (mapCallbacksIterator = this->MapCallbacks.begin(); mapCallbacksIterator != this->MapCallbacks.end(); mapCallbacksIterator++)
					{
						CCallbackBase* pCallback = mapCallbacksIterator->second;
						if (mapCallbacksIterator->first == CallbackMsg.m_iCallback && ((pCallback->m_nCallbackFlags & pCallback->k_ECallbackFlagsGameServer) >> 1) == (int)bGameServerCallbacks)
						{
							_cprintf_s("[Steam_API_Base] Running Callback (TryCatch) --> %d\r\n", CallbackMsg.m_iCallback);

							pCallback->Run(CallbackMsg.m_pubParam);
							this->oSteam_FreeLastCallback(hSteamPipe);
							return;
						}
					}
				}
				_cprintf_s("[Steam_API_Base] Freeing Callback (TryCatch) --> %d\r\n", CallbackMsg.m_iCallback);
				this->oSteam_FreeLastCallback(hSteamPipe);
			}
		}
	}
	catch (...)
	{
		_cprintf_s("[Steam_API_Base] Exception in callback code!\r\n");
	}
}

void CCallbackMgr::RegisterCallback(CCallbackBase *pCallback, int iCallback)
{
	_cprintf_s("[Steam_API_Base] CCallbackMgr::RegisterCallback --> %d\r\n", iCallback);

	if (pCallback != nullptr)
	{
		pCallback->m_nCallbackFlags |= pCallback->k_ECallbackFlagsRegistered;
		pCallback->m_iCallback = iCallback;

		this->MapCallbacks.insert(std::make_pair(iCallback, pCallback));
	}
}

void CCallbackMgr::UnregisterCallback(CCallbackBase* pCallback)
{
	if (pCallback != nullptr)
	{
		if ((pCallback->m_nCallbackFlags & pCallback->k_ECallbackFlagsRegistered) && this->MapCallbacks.size() != 0)
		{
			_cprintf_s("[Steam_API_Base] CCallbackMgr::UnregisterCallback --> %d\r\n", pCallback->GetICallback());

			pCallback->m_nCallbackFlags = 0;
			this->MapCallbacks.erase(pCallback->GetICallback());
		}
	}
}

void CCallbackMgr::RegisterCallResult(CCallbackBase *pCallback, SteamAPICall_t APICall)
{
	if (pCallback != nullptr)
	{
		_cprintf_s("[Steam_API_Base] CCallbackMgr::RegisterCallResult --> %d --> %lld\r\n", pCallback->GetICallback(), APICall);

		pCallback->m_nCallbackFlags |= pCallback->k_ECallbackFlagsRegistered;
		this->MapAPICalls.insert(std::make_pair(APICall, pCallback));
	}
}

void CCallbackMgr::UnregisterCallResult(CCallbackBase *pCallback, SteamAPICall_t APICall)
{
	if (pCallback != nullptr)
	{
		if ((pCallback->m_nCallbackFlags & pCallback->k_ECallbackFlagsRegistered) && this->MapAPICalls.size() != 0)
		{
			_cprintf_s("[Steam_API_Base] CCallbackMgr::UnregisterCallResult --> %d --> %lld\r\n", pCallback->GetICallback(), APICall);

			pCallback->m_nCallbackFlags = 0;
			this->MapAPICalls.erase(APICall);
		}
	}
}

CCallbackMgr* GCallbackMgr()
{
	static CCallbackMgr CallbackMgr;
	return &CallbackMgr;
}