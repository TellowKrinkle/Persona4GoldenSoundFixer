#include "stdafx.h"
#include "CustomHooks.h"

typedef HANDLE(WINAPI*CreateFileW_t)(
	LPCWSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile);

CreateFileW_t OriginalCreateFileW;

HANDLE WINAPI CreateFileW_Wrapper(
	LPCWSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
)
{
	// P4G opens sound files with FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, then sends them to XACT3.
	// XACT3 reads them asynchronously.
	// For some reason, on some drives, the reads just stop completing after a while...
	// Removing the no buffering flag seems to fix it, but I can't seem to reproduce the issue with small test programs so who knows if this is actually a full solution.
	dwFlagsAndAttributes &= ~FILE_FLAG_NO_BUFFERING;

	// Call the original CreateFileW function
	return OriginalCreateFileW(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

void SetupHooks()
{
	OriginalCreateFileW = HookFunction("KERNEL32.dll", "CreateFileW", &CreateFileW_Wrapper);
}

