#include <inc.h>
bool driver::c_loader::drop(LPCSTR file, LPCVOID data, DWORD size)
{
	std::string pt = stra("C:\\Windows\\System32\\drivers\\"); pt.append(file);
	DeleteFileA(pt.c_str());
	auto hFile = CreateFileA(pt.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;
	DWORD dwWrite;
	WriteFile(hFile, data, size, &dwWrite, NULL);
	CloseHandle(hFile);
	return dwWrite == size;
}
NTSTATUS driver::c_loader::delservicereg(const wchar_t* drv_name)
{
	NTSTATUS status = 0;
	std::wstring RegistryPath = std::wstring(strw(L"System\\CurrentControlSet\\Services\\")) + drv_name;
	status = RegDeleteKeyW(HKEY_LOCAL_MACHINE, RegistryPath.c_str());
	if (!status || status == ERROR_FILE_NOT_FOUND) return 0;
	status = RegDeleteKeyW(HKEY_LOCAL_MACHINE, RegistryPath.c_str());
	if (!status || status == ERROR_FILE_NOT_FOUND) return 0;
	status = RegDeleteKeyW(HKEY_LOCAL_MACHINE, RegistryPath.c_str());
	if (!status || status == ERROR_FILE_NOT_FOUND) return 0;
	return status;
}
NTSTATUS driver::c_loader::addservice(const wchar_t* drv_name)
{
	NTSTATUS status = STATUS_SUCCESS;
	std::wstring registryPath = std::wstring(strw(L"System\\CurrentControlSet\\Services\\")) + drv_name;
	this->delservicereg(drv_name);
	HKEY key;
	status = RegCreateKeyExW(HKEY_LOCAL_MACHINE, registryPath.c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &key, 0);
	if (status != ERROR_SUCCESS) return status;	
	std::wstring driverPath = std::wstring(strw(L"\\SystemRoot\\System32\\drivers\\")) + drv_name + strw(L".sys");
	DWORD value = 1;
	status |= RegSetValueExW(key, strw(L"ImagePath"), 0, REG_EXPAND_SZ, (PBYTE)driverPath.c_str(), driverPath.size() * sizeof(wchar_t));
	status |= RegSetValueExW(key, strw(L"Type"), 0, REG_DWORD, (PBYTE)&value, sizeof(DWORD));
	status |= RegSetValueExW(key, strw(L"ErrorControl"), 0, REG_DWORD, (PBYTE)&value, sizeof(DWORD));
	value = 3;
	status |= RegSetValueExW(key, strw(L"Start"), 0, REG_DWORD, (PBYTE)&value, sizeof(DWORD));

	if (status != ERROR_SUCCESS) 
	{
		RegCloseKey(key);
		this->delservicereg(drv_name);
		return status;
	}

	RegCloseKey(key);
	return STATUS_SUCCESS;
}
NTSTATUS driver::c_loader::openservice(const wchar_t* drv_name)
{
	std::wstring registryPath = std::wstring(strw(L"System\\CurrentControlSet\\Services\\")) + drv_name;
	HKEY key;
	NTSTATUS result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, registryPath.c_str(), 0, KEY_ALL_ACCESS, &key);
	RegCloseKey(key);
	return result;
}
bool driver::c_loader::setup(std::string drv_name)
{
	if (!this->drop(drv_name.c_str(), VBoxDrv, sizeof(VBoxDrv))) return 0;
	return 1;
}
bool driver::c_loader::load(std::wstring drv_name)
{
	BOOLEAN alreadyEnabled = FALSE;
	if (RtlAdjustPrivilege(SeLoadDriverPrivilege, 1ull, AdjustCurrentProcess, &alreadyEnabled) != STATUS_SUCCESS && !alreadyEnabled) return FALSE;
	if (this->addservice(drv_name.c_str()) != STATUS_SUCCESS) return 0;	
	std::wstring sourceRegistry = std::wstring(strw(L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\")) + drv_name;

	UNICODE_STRING sourceRegistryUnicode = { 0 };
	sourceRegistryUnicode.Buffer = (wchar_t*)sourceRegistry.c_str();
	sourceRegistryUnicode.Length = (sourceRegistry.size()) * 2;
	sourceRegistryUnicode.MaximumLength = (sourceRegistry.size() + 1) * 2;

	NTSTATUS status = NtLoadDriver(&sourceRegistryUnicode);

	user::p_data->log(stra(std::string("load driver returned ").append(std::to_string(status)).c_str()));

	if (status != STATUS_SUCCESS) 
	{
		user::p_data->log(stra("load driver failed"));
		this->unload(drv_name.c_str());
		this->delservicereg(drv_name.c_str());
		return 0;
	}
	return 1;
}
bool driver::c_loader::unload(const wchar_t* drv_name)
{
	BOOLEAN alreadyEnabled = FALSE;
	if (RtlAdjustPrivilege(SeLoadDriverPrivilege, 1ull, AdjustCurrentProcess, &alreadyEnabled) != STATUS_SUCCESS && !alreadyEnabled) return FALSE;
	if (this->openservice(drv_name) == 2) this->addservice(drv_name);	
	std::wstring sourceRegistry = std::wstring(strw(L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\")) + drv_name;

	UNICODE_STRING sourceRegistryUnicode = { 0 };
	sourceRegistryUnicode.Buffer = (wchar_t*)sourceRegistry.c_str();
	sourceRegistryUnicode.Length = (sourceRegistry.size()) * 2;
	sourceRegistryUnicode.MaximumLength = (sourceRegistry.size() + 1) * 2;

	NTSTATUS status;
	status = NtUnloadDriver(&sourceRegistryUnicode);

	user::p_data->log(stra(std::string("unload driver returned ").append(std::to_string(status)).c_str()));

	this->delservicereg(drv_name);

	return status;
}
driver::c_loader* driver::p_loader;