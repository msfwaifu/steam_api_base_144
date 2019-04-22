LSTATUS SetRegistryDWORD(const char *Key, const char *Name, DWORD Value)
{
	HKEY root = nullptr;
	DWORD dwDisp = 0;
	DWORD dwLength = sizeof(DWORD);

	LSTATUS SetRegKey = ERROR_MORE_DATA;

	SetRegKey = RegOpenKeyExA(HKEY_CURRENT_USER, Key, 0, KEY_WRITE, &root);

	if (SetRegKey != ERROR_SUCCESS)
	{
		SetRegKey = RegCreateKeyExA(HKEY_CURRENT_USER, Key, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &root, &dwDisp);

		if (SetRegKey != ERROR_SUCCESS)
			return SetRegKey;
	}

	SetRegKey = RegSetValueExA(root, Name, 0, REG_DWORD, (PBYTE)&Value, dwLength);

	if (SetRegKey != ERROR_SUCCESS)
	{
		RegCloseKey(root);
		return SetRegKey;
	}

	SetRegKey = RegCloseKey(root);

	return SetRegKey;
}

LSTATUS SetRegistryDWORDMachine(const char *Key, const char *Name, DWORD Value)
{
	HKEY root = nullptr;
	DWORD dwDisp = 0;
	DWORD dwLength = sizeof(DWORD);

	LSTATUS SetRegKey = ERROR_MORE_DATA;

	SetRegKey = RegOpenKeyExA(HKEY_LOCAL_MACHINE, Key, 0, KEY_WRITE, &root);

	if (SetRegKey != ERROR_SUCCESS)
	{
		SetRegKey = RegCreateKeyExA(HKEY_LOCAL_MACHINE, Key, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &root, &dwDisp);

		if (SetRegKey != ERROR_SUCCESS)
			return SetRegKey;
	}

	SetRegKey = RegSetValueExA(root, Name, 0, REG_DWORD, (PBYTE)&Value, dwLength);

	if (SetRegKey != ERROR_SUCCESS)
	{
		RegCloseKey(root);
		return SetRegKey;
	}

	SetRegKey = RegCloseKey(root);

	return SetRegKey;
}

LSTATUS SetRegistryFloat(const char *Key, const char *Name, float fValue)
{
	HKEY Root = nullptr;
	DWORD dwDisp = 0;

	LSTATUS SetRegKey = ERROR_MORE_DATA;

	SetRegKey = RegOpenKeyExA(HKEY_CURRENT_USER, Key, 0, KEY_WRITE, &Root);

	if (SetRegKey != ERROR_SUCCESS)
	{
		SetRegKey = RegCreateKeyExA(HKEY_CURRENT_USER, Key, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &Root, &dwDisp);

		if (SetRegKey != ERROR_SUCCESS)
			return SetRegKey;
	}

	char Buffer[MAX_PATH] = { 0 };
	_snprintf_s(Buffer, MAX_PATH, _TRUNCATE, "%f", fValue);

	DWORD dwLength = strlen(Buffer) + 1;

	SetRegKey = RegSetValueExA(Root, Name, 0, REG_SZ, (PBYTE)Buffer, dwLength);

	if (SetRegKey != ERROR_SUCCESS)
	{
		RegCloseKey(Root);
		return SetRegKey;
	}

	SetRegKey = RegCloseKey(Root);

	return SetRegKey;
}

LSTATUS SetRegistryString(const char *Key, const char *Name, const char *Value)
{
	HKEY Root = nullptr;
	DWORD dwDisp = 0;
	DWORD dwLength = strlen(Value) + 1;

	LSTATUS SetRegKey = ERROR_MORE_DATA;

	SetRegKey = RegOpenKeyExA(HKEY_CURRENT_USER, Key, 0, KEY_WRITE, &Root);

	if (SetRegKey != ERROR_SUCCESS)
	{
		SetRegKey = RegCreateKeyExA(HKEY_CURRENT_USER, Key, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &Root, &dwDisp);

		if (SetRegKey != ERROR_SUCCESS)
			return SetRegKey;
	}

	SetRegKey = RegSetValueExA(Root, Name, 0, REG_SZ, (PBYTE)Value, dwLength);

	if (SetRegKey != ERROR_SUCCESS)
	{
		RegCloseKey(Root);
		return SetRegKey;
	}

	SetRegKey = RegCloseKey(Root);

	return SetRegKey;
}

LSTATUS GetRegistryDWORD(const char* cszPath, const char* cszKey, DWORD& dwValue)
{
	HKEY hKeyNode = nullptr;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);

	LSTATUS GetRegKey = ERROR_MORE_DATA;

	GetRegKey = RegOpenKeyExA(HKEY_CURRENT_USER, cszPath, 0, KEY_READ, &hKeyNode);

	if (GetRegKey != ERROR_SUCCESS)
		return GetRegKey;

	GetRegKey = RegQueryValueExA(hKeyNode, cszKey, nullptr, &dwType, (LPBYTE)&dwValue, &dwSize);

	if (GetRegKey != ERROR_SUCCESS)
	{
		RegCloseKey(hKeyNode);
		return GetRegKey;
	}

	GetRegKey = RegCloseKey(hKeyNode);

	return GetRegKey;
}

LSTATUS GetRegistryString(const char* cszPath, const char* cszKey, char *szString, unsigned int uMaxBuf)
{
	DWORD dwType = REG_SZ;
	DWORD dwSize = uMaxBuf;

	LSTATUS GetRegKey = RegGetValueA(HKEY_CURRENT_USER, cszPath, cszKey, RRF_RT_REG_SZ, &dwType, szString, &dwSize);

	return GetRegKey;
}

LSTATUS GetRegistryFloat(const char* cszPath, const char* cszKey, unsigned int uMaxBuf, float& fValue)
{
	if (uMaxBuf != MAX_PATH)
		return ERROR_MORE_DATA;

	DWORD dwType = REG_SZ;
	DWORD dwSize = uMaxBuf;

	char Buffer[MAX_PATH] = { 0 };

	LSTATUS GetRegKey = RegGetValueA(HKEY_CURRENT_USER, cszPath, cszKey, RRF_RT_REG_SZ, &dwType, Buffer, &dwSize);

	if (GetRegKey == ERROR_SUCCESS)
		fValue = (float)atof(Buffer);

	return GetRegKey;
}