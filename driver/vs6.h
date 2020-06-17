#pragma once
/*
VIsual Studio 6.0 Setup

VC++ Directory - Include directory
C:\Program Files\Microsoft Platform SDK\Include;
C:\Program Files\Microsoft Platform SDK\Include\crt;
C:\Program Files\Microsoft Platform SDK\Include\crt\sys;
C:\Program Files\Microsoft Platform SDK\Include\mfc;
C:\Program Files\Microsoft Platform SDK\Include\atl

VC++ Directory - Library directory
x64
C:\Program Files\Microsoft Platform SDK\Lib\X64;
C:\Program Files\Microsoft Platform SDK\Lib\X64\atlmfc
x86
C:\Program Files\Microsoft Platform SDK\Lib\X86;
C:\Program Files\Microsoft Platform SDK\Lib\X86\mfc
*/

//C++, Runtime Library, /MD (Multi Thread DLL) or /MDd
//C++, Code Generation, /GS-
//C++, Code Generation, Basic Runtime Check, default
//C++, Optimization, remove /GL
//Linker, Advanced, /SAFESEH:NO (x86)

#include <Windows.h>

#ifdef __cplusplus 
#if WINVER == 0x0501
__declspec(dllimport) void terminate();
extern "C" __declspec(dllimport) void __CxxFrameHandler();
#ifdef _WIN64
void __std_terminate() { terminate(); }
extern "C" void __CxxFrameHandler3() { __CxxFrameHandler(); }
#else
__declspec(naked) void __std_terminate() { __asm jmp dword ptr[terminate] }
extern "C" __declspec(naked) void __CxxFrameHandler3() { __asm jmp dword ptr[__CxxFrameHandler] }
#endif
#endif
#endif