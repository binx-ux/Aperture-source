#include "Core.hpp"
#include "Classes.hpp"
#include "auth.hpp"
#include "GameDetect.hpp"
#include "FiveM.hpp"

#include <process.h>

#pragma warning(disable : 6031)

#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef NTSTATUS(NTAPI* RtlCreateUserThread_t)(
	HANDLE, PSECURITY_DESCRIPTOR, BOOLEAN, ULONG, SIZE_T, SIZE_T,
	PTHREAD_START_ROUTINE, PVOID, PHANDLE, PCLIENT_ID);

BOOLEAN APIENTRY DllMain(HINSTANCE dll, DWORD reason, LPVOID)
{
	if (reason != DLL_PROCESS_ATTACH)
		return TRUE;

	DisableThreadLibraryCalls(dll);

	Detect::Current = Detect::Resolve();
	if (Detect::Current == GameId::None)
		return TRUE;

	if (DEBUG == 0)
	{
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		printf("[aperture] attached to %s\n", Detect::Name());
	}

	bool ok = false;
	if (Detect::Current == GameId::Fortnite)
		ok = Game::Init();
	else if (Detect::Current == GameId::FiveM)
		ok = FiveM::Init();

	if (!ok)
	{
		printf("[aperture] init failed\n");
		return TRUE;
	}

	auto fn = (RtlCreateUserThread_t)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlCreateUserThread");
	if (fn)
		fn((HANDLE)(LONG_PTR)-1, 0, 0, 0, 0, 0, (PTHREAD_START_ROUTINE)Core::Init, 0, 0, 0);

	return TRUE;
}
