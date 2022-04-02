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
	// XACT3's reads are always aligned to at least 512 bytes, but are not always aligned to 4096 bytes, and will fail on drives with 4K blocks.
	// Remove the no buffering flag to fix
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

