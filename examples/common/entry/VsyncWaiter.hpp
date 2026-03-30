#pragma once

#include "Utils.h"

//////////////////////////////////////////////////////////////////////////
class VsyncWaiter
{
	Def_NonCopyable(VsyncWaiter);
	Def_NonMovable(VsyncWaiter);

public:
	VsyncWaiter() noexcept;
	~VsyncWaiter() noexcept;

	bool Open(const wchar_t *strDisplay = NULL) noexcept;

	void Close() noexcept;

	// 等待垂直同步信号
	void WaitVsync() const noexcept;

private:
	// 显示器句柄
	uint32_t AdapterHandle_ = 0;
};
