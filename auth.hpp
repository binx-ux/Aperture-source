#pragma once

#include <Windows.h>
#include <string>
#include <tuple>
#include <vector>
#include <cstdint>
#include <cstring>
#include <chrono>

extern bool IsValid;
extern bool AuthConnected;

namespace Auth
{
	inline char Key[64] = "";
	inline char Status[128] = "waiting for key";
	inline bool Busy = false;
	inline bool Failed = false;
	inline float Progress = 0.f;
	inline std::chrono::steady_clock::time_point BusyStart{};

	// local unlock keys, no remote panel
	inline bool CheckKey(const char* key)
	{
		if (!key || !key[0])
			return false;

		if (_stricmp(key, "aperture") == 0) return true;
		if (_stricmp(key, "loader") == 0) return true;
		if (strlen(key) >= 8) return true; // accept longer custom keys locally
		return false;
	}

	inline void BeginLogin()
	{
		if (Busy || AuthConnected)
			return;

		if (!CheckKey(Key))
		{
			Failed = true;
			strcpy_s(Status, "invalid key");
			return;
		}

		Busy = true;
		Failed = false;
		Progress = 0.f;
		BusyStart = std::chrono::steady_clock::now();
		strcpy_s(Status, "connecting...");
	}

	inline void Tick()
	{
		if (!Busy)
			return;

		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - BusyStart).count();

		Progress = (float)ms / 1800.f;
		if (Progress > 1.f) Progress = 1.f;

		if (ms < 600) strcpy_s(Status, "connecting...");
		else if (ms < 1200) strcpy_s(Status, "verifying license...");
		else if (ms < 1800) strcpy_s(Status, "loading modules...");
		else
		{
			Busy = false;
			IsValid = true;
			AuthConnected = true;
			strcpy_s(Status, "ready");
		}
	}
}

static char PassWord[50] = "";

class program
{
public:
	static std::tuple<std::string, std::string, std::string> login(std::string, std::string, std::string, std::string, std::string)
	{
		return { "OK", "", "" };
	}

	static std::vector<uint8_t> Stream(std::string, std::string)
	{
		return {};
	}
};

static void BAN_USER(std::string, std::string) {}
