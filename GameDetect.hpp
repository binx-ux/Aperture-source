#pragma once

#include <Windows.h>

enum class GameId
{
	None,
	Fortnite,
	FiveM
};

namespace Detect
{
	inline GameId Current = GameId::None;

	inline bool HasMod(const char* name)
	{
		return GetModuleHandleA(name) != nullptr;
	}

	inline GameId Resolve()
	{
		if (HasMod("FortniteClient-Win64-Shipping.exe"))
			return GameId::Fortnite;

		const char* fivem[] = {
			"FiveM_GameProcess.exe", "FiveM_GTAProcess.exe",
			"FiveM_b2060_GameProcess.exe", "FiveM_b2060_GTAProcess.exe",
			"FiveM_b2189_GameProcess.exe", "FiveM_b2189_GTAProcess.exe",
			"FiveM_b2372_GameProcess.exe", "FiveM_b2372_GTAProcess.exe",
			"FiveM_b2545_GameProcess.exe", "FiveM_b2545_GTAProcess.exe",
			"FiveM_b2612_GameProcess.exe", "FiveM_b2612_GTAProcess.exe",
			"FiveM_b2699_GameProcess.exe", "FiveM_b2699_GTAProcess.exe",
			"FiveM_b2802_GameProcess.exe", "FiveM_b2802_GTAProcess.exe",
			"FiveM_b2944_GameProcess.exe", "FiveM_b2944_GTAProcess.exe",
			"FiveM_b3095_GameProcess.exe", "FiveM_b3095_GTAProcess.exe",
		};

		for (auto* n : fivem)
		{
			if (HasMod(n))
				return GameId::FiveM;
		}
		return GameId::None;
	}

	inline const char* Name()
	{
		switch (Current)
		{
		case GameId::Fortnite: return "Fortnite";
		case GameId::FiveM: return "FiveM";
		default: return "Unknown";
		}
	}
}
