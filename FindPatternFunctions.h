#include <Psapi.h>
bool FailedToGetModuleInfo = false;

MODULEINFO GetModuleInfo(const char *szModule)
{
	MODULEINFO modinfo = { nullptr };
	HMODULE hModule = GetModuleHandleA(szModule);

	if (hModule == nullptr)
	{
		FailedToGetModuleInfo = true;
		return modinfo;
	}

	BOOL ModuleInfo = GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));

	if (ModuleInfo == FALSE)
	{
		FailedToGetModuleInfo = true;
		return modinfo;
	}

	return modinfo;
}

DWORD FindPattern(const char *Module, const char *Pattern, const char *Mask)
{
	if (Module == nullptr)
		return 0;

	if (Pattern == nullptr)
		return 0;

	if (Mask == nullptr)
		return 0;

	MODULEINFO mInfo = GetModuleInfo(Module);

	if (FailedToGetModuleInfo == true)
	{
		FailedToGetModuleInfo = false;
		return 0;
	}

	DWORD BaseDll = (DWORD)mInfo.lpBaseOfDll;

	if (BaseDll == 0)
		return 0;

	DWORD SizeDll = (DWORD)mInfo.SizeOfImage;

	if (SizeDll == 0)
		return 0;

	DWORD PatternLength = (DWORD)strlen(Mask);

	for (DWORD i = 0; i < SizeDll - PatternLength; i++)
	{
		bool Found = true;
		for (DWORD j = 0; j < PatternLength; j++)
		{
			Found &= Mask[j] == '?' || Pattern[j] == *(char*)(BaseDll + i + j);
		}

		if (Found)
		{
			return BaseDll + i;
		}
	}

	return 0;
}

BOOL WriteToMemory(DWORD MemoryAddress, const void* PatchBytes, int byteNum)
{
	if (MemoryAddress == 0 || byteNum == 0)
		return FALSE;

	DWORD dwProtect = 0;

	BOOL VP = VirtualProtect((LPVOID)(MemoryAddress), byteNum, PAGE_READWRITE, &dwProtect);

	if (VP == TRUE)
	{
		memcpy((LPVOID)MemoryAddress, (const void*)PatchBytes, byteNum);
		VirtualProtect((LPVOID)(MemoryAddress), byteNum, dwProtect, &dwProtect);

		return TRUE;
	}
	return FALSE;
}