#include "VsyncWaiter.hpp"
#include <windows.h>
#include <d3dkmthk.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ntdll.lib")

//////////////////////////////////////////////////////////////////////////
extern "C"
{
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetTimerResolution(
	_In_ ULONG DesiredTime,
	_In_ BOOLEAN SetResolution,
	_Out_ PULONG ActualTime);
}

//////////////////////////////////////////////////////////////////////////
VsyncWaiter::VsyncWaiter() noexcept
{
	ULONG lastTimerRes = 0;
	NTSTATUS status = NtSetTimerResolution(5000, TRUE, &lastTimerRes);
}

VsyncWaiter::~VsyncWaiter() noexcept
{
	Close();
}

bool VsyncWaiter::Open(const wchar_t *strDisplay) noexcept
{
	D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME openArgs{};

	if (strDisplay != NULL)
		StrCopyNoOverlap(openArgs.DeviceName, sizeof(openArgs.DeviceName), strDisplay);
	else
	{
		uint32_t maxRate = 0;

		DISPLAY_DEVICEW dd{};
		dd.cb = sizeof(dd);

		DWORD deviceIndex = 0;
		while (EnumDisplayDevicesW(
			NULL,
			deviceIndex,
			&dd,
			EDD_GET_DEVICE_INTERFACE_NAME) != FALSE)
		{
			DEVMODEW dm{};
			dm.dmSize = sizeof(DEVMODEW);

			if (EnumDisplaySettingsExW(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm, 0) != FALSE)
			{
				if (maxRate < dm.dmDisplayFrequency)
				{
					maxRate = dm.dmDisplayFrequency;
					StrCopyNoOverlap(openArgs.DeviceName, sizeof(openArgs.DeviceName), dd.DeviceName, sizeof(dd.DeviceName));
				}
			}

			deviceIndex++;
		}
	}

	const NTSTATUS status = D3DKMTOpenAdapterFromGdiDisplayName(&openArgs);
	if (status != S_OK)
		return false;

	AdapterHandle_ = openArgs.hAdapter;

	return true;
}

void VsyncWaiter::Close() noexcept
{
	if (AdapterHandle_ == 0)
		return;

	const D3DKMT_CLOSEADAPTER closeArgs{ AdapterHandle_ };
	const NTSTATUS status = D3DKMTCloseAdapter(&closeArgs);

	AdapterHandle_ = 0;
}

void VsyncWaiter::WaitVsync() const noexcept
{
	D3DKMT_WAITFORVERTICALBLANKEVENT ev{};
	ev.hAdapter = AdapterHandle_;
	const NTSTATUS status = D3DKMTWaitForVerticalBlankEvent(&ev);
	Assert(status == S_OK);
}
