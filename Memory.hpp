#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstring>

// read only. dont write to game mem through this

namespace Mem
{
	inline bool CanRead(uintptr_t addr, size_t size)
	{
		if (!addr || !size || size > 0x10000)
			return false;

		__try
		{
			volatile char c = *(volatile const char*)addr;
			(void)c;
			if (size > 1)
			{
				c = *(volatile const char*)(addr + size - 1);
				(void)c;
			}
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	inline bool ReadRaw(uintptr_t addr, void* out, size_t size)
	{
		if (!out || !CanRead(addr, size))
			return false;

		__try
		{
			const volatile unsigned char* src = (const volatile unsigned char*)addr;
			unsigned char* dst = (unsigned char*)out;
			for (size_t i = 0; i < size; i++)
				dst[i] = src[i];
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			memset(out, 0, size);
			return false;
		}
	}

	template <typename T>
	inline bool Read(uintptr_t addr, T& out)
	{
		out = T{};
		return ReadRaw(addr, &out, sizeof(T));
	}

	inline uintptr_t ReadPtr(uintptr_t addr)
	{
		uintptr_t v = 0;
		Read(addr, v);
		return v;
	}
}
