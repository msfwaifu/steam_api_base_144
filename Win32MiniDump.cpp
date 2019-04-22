#include <Windows.h>
#include <conio.h>
#include "Win32MiniDump.h"

//Made by RevCrew, Steam006

CWin32MiniDump::CWin32MiniDump(const char* cszName)
{
	this->m_sName.assign(cszName);
	this->m_hDbgHelp = nullptr;
	this->m_fnMiniDumpWriteDump = nullptr;
	this->m_sComment.clear();

	this->m_hDbgHelp = LoadLibraryExA("DbgHelp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

	if (this->m_hDbgHelp != nullptr)
	{
		FARPROC fpMiniDumpWriteDump = GetProcAddress(this->m_hDbgHelp, "MiniDumpWriteDump");

		if (fpMiniDumpWriteDump != nullptr)
		{
			this->m_fnMiniDumpWriteDump = fnMiniDumpWriteDump(fpMiniDumpWriteDump);

			CreateDirectoryA("steam_api_Minidumps", nullptr);
		}
		else
		{
			FreeLibrary(this->m_hDbgHelp);
			this->m_hDbgHelp = nullptr;
		}
	}
}

CWin32MiniDump::~CWin32MiniDump()
{
	this->m_sName.clear();

	if (this->m_hDbgHelp != nullptr)
	{
		FreeLibrary(this->m_hDbgHelp);
		this->m_hDbgHelp = nullptr;
	}

	this->m_fnMiniDumpWriteDump = nullptr;
	this->m_sComment.clear();
}

void CWin32MiniDump::SetComment(const char* cszComment)
{
	this->m_sComment.assign(cszComment);
}

void CWin32MiniDump::ClearComment()
{
	this->m_sComment.clear();
}

void CWin32MiniDump::WriteUsingExceptionInfo(DWORD dwExceptionCode, _EXCEPTION_POINTERS* pStructuredExceptionPointers)
{
	if (this->m_hDbgHelp == nullptr || this->m_hDbgHelp != GetModuleHandleA("DbgHelp.dll"))
		return;

	if (this->m_fnMiniDumpWriteDump == nullptr || (FARPROC)this->m_fnMiniDumpWriteDump != GetProcAddress(this->m_hDbgHelp, "MiniDumpWriteDump"))
		return;

	if (this->m_sName.length() == 0)
		return;

	HANDLE hCurrentProcess = GetCurrentProcess();

	if (hCurrentProcess != (HANDLE)-1)
		return;

	if (GetCurrentProcessId() != PID)
		return;

	SYSTEMTIME time;
	GetLocalTime(&time);

	char szFileName[MAX_PATH] = { 0 };
	int StringFactory = _snprintf_s(szFileName, MAX_PATH, _TRUNCATE, "steam_api_Minidumps\\%s_%04u-%02u-%02u-%02u.%02u.%02u.%03u.mdmp", this->m_sName.c_str(), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	if (StringFactory == -1)
		return;

	HANDLE hMiniDumpFile = CreateFileA(szFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hMiniDumpFile == INVALID_HANDLE_VALUE)
		return;

	__try
	{
		MINIDUMP_EXCEPTION_INFORMATION mdmpExceptionInfo;
		mdmpExceptionInfo.ThreadId = GetCurrentThreadId();
		mdmpExceptionInfo.ExceptionPointers = pStructuredExceptionPointers;
		mdmpExceptionInfo.ClientPointers = FALSE;

		if (this->m_sComment.length() != 0)
		{
			MINIDUMP_USER_STREAM UserStream[2] = { 0 };

			UserStream[0].Type = CommentStreamA;
			UserStream[0].BufferSize = this->m_sComment.length() + 1;
			UserStream[0].Buffer = (LPVOID)this->m_sComment.c_str();

			MINIDUMP_EXCEPTION_STREAM MinidumpExcptionStream;
			MinidumpExcptionStream.ExceptionRecord.ExceptionCode = dwExceptionCode;

			UserStream[1].Type = ExceptionStream;
			UserStream[1].BufferSize = sizeof(MINIDUMP_EXCEPTION_STREAM);
			UserStream[1].Buffer = &MinidumpExcptionStream;

			MINIDUMP_USER_STREAM_INFORMATION MinidumpUserStreamInfo;
			MinidumpUserStreamInfo.UserStreamCount = 2;
			MinidumpUserStreamInfo.UserStreamArray = UserStream;

			this->m_fnMiniDumpWriteDump(hCurrentProcess, PID, hMiniDumpFile, (_MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithHandleData | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules | MiniDumpWithThreadInfo), &mdmpExceptionInfo, &MinidumpUserStreamInfo, nullptr);
		}
		else
		{
			MINIDUMP_USER_STREAM UserStream[1] = { 0 };

			MINIDUMP_EXCEPTION_STREAM MinidumpExcptionStream;
			MinidumpExcptionStream.ExceptionRecord.ExceptionCode = dwExceptionCode;

			UserStream[0].Type = ExceptionStream;
			UserStream[0].BufferSize = sizeof(MINIDUMP_EXCEPTION_STREAM);
			UserStream[0].Buffer = &MinidumpExcptionStream;

			MINIDUMP_USER_STREAM_INFORMATION MinidumpUserStreamInfo;
			MinidumpUserStreamInfo.UserStreamCount = 1;
			MinidumpUserStreamInfo.UserStreamArray = UserStream;

			this->m_fnMiniDumpWriteDump(hCurrentProcess, PID, hMiniDumpFile, (_MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithHandleData | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules | MiniDumpWithThreadInfo), &mdmpExceptionInfo, &MinidumpUserStreamInfo, nullptr);
		}

		CloseHandle(hMiniDumpFile);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		_cprintf_s("[Steam_API_Base] Exception in minidump code!\r\n");
	}
	return;
}