#pragma once

#include <Windows.h>
#include <xmmintrin.h>
#include "includes.hpp"
#include "Scan.hpp"
#include "Memory.hpp"
#include "Classes.hpp"

// FiveM / GTA side. kept close to the old paste layout.

namespace FiveM
{
	inline uintptr_t World = 0;
	inline uintptr_t ReplayInterface = 0;
	inline uintptr_t W2S = 0;
	inline uintptr_t BonePos = 0;
	inline uintptr_t Camera = 0;

	inline DWORD Armor = 0x14B8;
	inline DWORD EntityType = 0x10A8;
	inline DWORD WeaponManager = 0x10C8;
	inline DWORD PlayerInfo = 0x10B8;
	inline DWORD Recoil = 0x2E8;
	inline DWORD Spread = 0x74;
	inline DWORD ReloadMultiplier = 0x12C;
	inline DWORD Range = 0x25C;
	inline DWORD IsInAVehicule = 0x146B;

	inline ImVec2 WindowSize = ImVec2((float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN));
	inline bool Ready = false;

	inline void ApplyBuildOffsets()
	{
		auto has = [](const char* a, const char* b = nullptr)
		{
			if (GetModuleHandleA(a)) return true;
			return b && GetModuleHandleA(b);
		};

		if (has("FiveM_GameProcess.exe", "FiveM_GTAProcess.exe"))
		{
			EntityType = 0x10A8; Armor = 0x14B8; WeaponManager = 0x10C8; PlayerInfo = 0x10B8;
			Recoil = 0x2E8; Spread = 0x74; ReloadMultiplier = 0x12C; Range = 0x25C; IsInAVehicule = 0x146B;
		}
		if (has("FiveM_b2060_GameProcess.exe", "FiveM_b2060_GTAProcess.exe"))
		{
			EntityType = 0x10B8; Armor = 0x14E0; WeaponManager = 0x10D8; PlayerInfo = 0x10B8;
			Recoil = 0x2F4; Spread = 0x84; ReloadMultiplier = 0x134; Range = 0x28C; IsInAVehicule = 0x146B;
		}
		if (has("FiveM_b2189_GameProcess.exe", "FiveM_b2189_GTAProcess.exe") ||
			has("FiveM_b2372_GameProcess.exe", "FiveM_b2372_GTAProcess.exe"))
		{
			EntityType = 0x10B8; Armor = 0x14E0; WeaponManager = 0x10D8; PlayerInfo = 0x10C8;
			Recoil = 0x2F4; Spread = 0x84; ReloadMultiplier = 0x134; Range = 0x28C; IsInAVehicule = 0x146B;
		}
		if (has("FiveM_b2545_GameProcess.exe", "FiveM_b2545_GTAProcess.exe"))
		{
			EntityType = 0x10B8; Armor = 0x14E0 + 0x50; WeaponManager = 0x10D8; PlayerInfo = 0x10C8;
			Recoil = 0x2F4; Spread = 0x84; ReloadMultiplier = 0x134; Range = 0x28C; IsInAVehicule = 0x146B;
		}
		if (has("FiveM_b2612_GameProcess.exe", "FiveM_b2612_GTAProcess.exe") ||
			has("FiveM_b2699_GameProcess.exe", "FiveM_b2699_GTAProcess.exe") ||
			has("FiveM_b2802_GameProcess.exe", "FiveM_b2802_GTAProcess.exe") ||
			has("FiveM_b2944_GameProcess.exe", "FiveM_b2944_GTAProcess.exe") ||
			has("FiveM_b3095_GameProcess.exe", "FiveM_b3095_GTAProcess.exe"))
		{
			EntityType = 0x10B8; Armor = 0x1530; WeaponManager = 0x10D8; PlayerInfo = 0x10C8;
			Recoil = 0x2F4; Spread = 0x84; ReloadMultiplier = 0x134; Range = 0x28C; IsInAVehicule = 0x146B;
		}
	}

	inline bool Init()
	{
		ApplyBuildOffsets();

		World = Scan::Pattern("48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81", 7);
		ReplayInterface = Scan::Pattern("48 8D 0D ? ? ? ? 89 44 24 30 E8 ? ? ? ? 48 83 C4 28 C3 48 8B 05", 7);
		W2S = Scan::Pattern("48 89 5C 24 ?? 55 56 57 48 83 EC 70 65 4C 8B 0C 25", 0);
		BonePos = Scan::Pattern("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 60 48 8B 01 41 8B E8 48 8B F2 48 8B F9 33 DB", 0);
		Camera = Scan::Pattern("48 8B 05 ? ? ? ? 48 8B 98 ? ? ? ? EB", 7);

		Ready = World != 0 && ReplayInterface != 0 && W2S != 0 && BonePos != 0;
		return Ready;
	}

	inline uintptr_t GetCamera()
	{
		if (!Camera) return 0;
		return Mem::ReadPtr(Camera);
	}

	inline ImVec2 WorldToScreen(Vec3 pos)
	{
		if (!W2S) return ImVec2(-1, -1);
		ImVec2 out{};
		__try
		{
			reinterpret_cast<bool(__fastcall*)(Vec3*, float*, float*)>(W2S)(&pos, &out.x, &out.y);
			out.x *= ImGui::GetIO().DisplaySize.x;
			out.y *= ImGui::GetIO().DisplaySize.y;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return ImVec2(-1, -1);
		}
		return out;
	}

	inline Vec3 GetBone(uintptr_t ped, int32_t mask)
	{
		if (!BonePos || !ped) return {};
		__m128 tmp{};
		__try
		{
			reinterpret_cast<void*(__fastcall*)(uint64_t, __m128*, int32_t)>(BonePos)(ped, &tmp, mask);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return {};
		}
		return Vec3(tmp.m128_f32[0], tmp.m128_f32[1], tmp.m128_f32[2]);
	}

	inline ImVec2 GetBoneW2S(uintptr_t ped, int32_t mask)
	{
		return WorldToScreen(GetBone(ped, mask));
	}

	class Ped
	{
	public:
		uintptr_t ptr = 0;
		Ped() = default;
		explicit Ped(uintptr_t p) : ptr(p) {}

		bool Valid() const { return ptr != 0; }

		Vec3 Coord() const
		{
			Vec3 v{};
			Mem::Read(ptr + 0x90, v);
			return v;
		}

		float Health() const
		{
			float h = 0.f;
			Mem::Read(ptr + 0x280, h);
			return h;
		}

		float Armor() const
		{
			float a = 0.f;
			Mem::Read(ptr + FiveM::Armor, a);
			return a;
		}

		uint32_t PedType() const
		{
			uint32_t t = 0;
			Mem::Read(ptr + EntityType, t);
			return t;
		}

		bool IsPlayer() const
		{
			uint32_t t = PedType();
			t = t << 11 >> 25;
			return t == 2;
		}

		uintptr_t WeaponMgr() const
		{
			return Mem::ReadPtr(ptr + WeaponManager);
		}

		uintptr_t WeaponInfo() const
		{
			uintptr_t wm = WeaponMgr();
			if (!wm) return 0;
			return Mem::ReadPtr(wm + 0x20);
		}

		void SetRecoil(float v)
		{
			uintptr_t wi = WeaponInfo();
			if (!wi) return;
			__try { *(float*)(wi + Recoil) = v; }
			__except (EXCEPTION_EXECUTE_HANDLER) {}
		}

		void SetSpread(float v)
		{
			uintptr_t wi = WeaponInfo();
			if (!wi) return;
			__try { *(float*)(wi + Spread) = v; }
			__except (EXCEPTION_EXECUTE_HANDLER) {}
		}
	};

	inline Ped Local()
	{
		uintptr_t world = Mem::ReadPtr(World);
		if (!world) return {};
		return Ped(Mem::ReadPtr(world + 0x8));
	}

	inline uintptr_t PedListBase()
	{
		uintptr_t ri = Mem::ReadPtr(ReplayInterface);
		if (!ri) return 0;
		uintptr_t pi = Mem::ReadPtr(ri + 0x18);
		if (!pi) return 0;
		return Mem::ReadPtr(pi + 0x100);
	}

	inline int PedMax()
	{
		uintptr_t ri = Mem::ReadPtr(ReplayInterface);
		if (!ri) return 0;
		uintptr_t pi = Mem::ReadPtr(ri + 0x18);
		if (!pi) return 0;
		int n = 0;
		Mem::Read(pi + 0x108, n);
		return n;
	}

	inline Ped PedAt(int i)
	{
		uintptr_t list = PedListBase();
		if (!list) return {};
		return Ped(Mem::ReadPtr(list + (uintptr_t)i * 0x10));
	}
}
