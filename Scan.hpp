#pragma once

#include <Windows.h>
#include <cstdint>
#include <vector>
#include <cstring>

namespace Scan
{
	inline uintptr_t Pattern(const char* sig, int ripOffset = 0)
	{
		HMODULE mod = GetModuleHandleA(nullptr);
		if (!mod) return 0;

		auto toBytes = [](const char* pattern)
		{
			std::vector<int> bytes;
			const char* start = pattern;
			const char* end = pattern + strlen(pattern);
			for (const char* cur = start; cur < end; ++cur)
			{
				if (*cur == '?')
				{
					++cur;
					if (*cur == '?') ++cur;
					bytes.push_back(-1);
				}
				else
				{
					bytes.push_back((int)strtoul(cur, const_cast<char**>(&cur), 16));
				}
			}
			return bytes;
		};

		auto* dos = (PIMAGE_DOS_HEADER)mod;
		auto* nt = (PIMAGE_NT_HEADERS)((uint8_t*)mod + dos->e_lfanew);
		DWORD size = nt->OptionalHeader.SizeOfImage;
		auto* scan = (uint8_t*)mod;
		auto bytes = toBytes(sig);
		size_t len = bytes.size();

		for (DWORD i = 0; i + len < size; i++)
		{
			bool ok = true;
			for (size_t j = 0; j < len; j++)
			{
				if (bytes[j] != -1 && scan[i + j] != (uint8_t)bytes[j])
				{
					ok = false;
					break;
				}
			}
			if (!ok) continue;

			uintptr_t addr = (uintptr_t)&scan[i];
			if (ripOffset > 0)
			{
				int32_t rel = *(int32_t*)(addr + 3);
				addr = addr + 7 + rel;
			}
			return addr;
		}
		return 0;
	}
}
