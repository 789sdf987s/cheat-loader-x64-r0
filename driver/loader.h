#pragma once
#include <inc.h>

#define NtCurrentProcess() ( HANDLE(-1) )
#define SeLoadDriverPrivilege 10ull
#define SystemModuleInformation 0xBull
#define AdjustCurrentProcess 0ull
#define STATUS_SUCCESS 0

using fnFreeCall = uint64_t(__fastcall*)(...);
template<typename ...Params>
static NTSTATUS __NtRoutine(const char* Name, Params&& ... params) {
	auto fn = (fnFreeCall)GetProcAddress(GetModuleHandleA("ntdll.dll"), Name);
	return fn(std::forward<Params>(params) ...);
}

#define NtQuerySystemInformation(...) __NtRoutine("NtQuerySystemInformation", __VA_ARGS__)
#define RtlAdjustPrivilege(...) __NtRoutine("RtlAdjustPrivilege", __VA_ARGS__)
#define NtUnloadDriver(...) __NtRoutine("NtUnloadDriver", __VA_ARGS__)
#define NtLoadDriver(...) __NtRoutine("NtLoadDriver", __VA_ARGS__)

namespace driver
{
	class c_loader
	{
	private:
		bool drop(LPCSTR file, LPCVOID data, DWORD size);
		NTSTATUS delservicereg(const wchar_t* drv_name);
		NTSTATUS addservice(const wchar_t* drv_name);
		NTSTATUS openservice(const wchar_t* drv_name);
	public:
		bool setup(std::string drv_name); /*sets up the driver - not loading it*/
		bool load(std::wstring drv_name);
		bool unload(const wchar_t* drv_name);
	};
	extern c_loader* p_loader;
}