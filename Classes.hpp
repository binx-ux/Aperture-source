#pragma once

#include "includes.hpp"
#include "Offsets.hpp"
#include "Memory.hpp"

struct Vec3
{
	float x = 0.f, y = 0.f, z = 0.f;

	Vec3() = default;
	Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

	Vec3 operator+(const Vec3& o) const { return Vec3(x + o.x, y + o.y, z + o.z); }
	Vec3 operator-(const Vec3& o) const { return Vec3(x - o.x, y - o.y, z - o.z); }
	float Dist(const Vec3& o) const
	{
		float dx = x - o.x, dy = y - o.y, dz = z - o.z;
		return sqrtf(dx * dx + dy * dy + dz * dz);
	}
};

struct FQuat { float x, y, z, w; };
struct FTransform
{
	FQuat rot;
	Vec3 translation;
	char pad[4];
	Vec3 scale;
};

struct FVectorD { double x, y, z; };
struct FRotatorD { double Pitch, Yaw, Roll; };

inline bool OnScreen(ImVec2 p)
{
	auto& io = ImGui::GetIO();
	if (p.x < 1.f || p.y < 1.f) return false;
	if (p.x > io.DisplaySize.x || p.y > io.DisplaySize.y) return false;
	return true;
}

struct Player
{
	uintptr_t pawn = 0;
	uintptr_t mesh = 0;
	Vec3 root;
	Vec3 head;
	float hp = 100.f;
	float maxHp = 100.f;
};

namespace Game
{
	inline uintptr_t base = 0;
	inline uintptr_t uworld = 0;
	inline uintptr_t gi = 0;
	inline uintptr_t gs = 0;
	inline uintptr_t localPlayer = 0;
	inline uintptr_t pc = 0;
	inline uintptr_t pawn = 0;
	inline Vec3 camPos;
	inline FRotatorD camRot;
	inline float camFov = 80.f;
	inline ImVec2 screenSize = ImVec2((float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN));

	inline float Dist2D(ImVec2 a, ImVec2 b)
	{
		float dx = a.x - b.x;
		float dy = a.y - b.y;
		return sqrtf(dx * dx + dy * dy);
	}

	inline bool IsFN()
	{
		return GetModuleHandleA("FortniteClient-Win64-Shipping.exe") != nullptr;
	}

	inline uintptr_t DecryptWorld(uintptr_t enc)
	{
		return (uintptr_t)(Offsets::WorldMul * enc + Offsets::WorldAdd);
	}

	inline ImVec2 W2S(const Vec3& world)
	{
		float pitch = (float)(camRot.Pitch * 0.01745329251);
		float yaw = (float)(camRot.Yaw * 0.01745329251);
		float roll = (float)(camRot.Roll * 0.01745329251);

		float sp = sinf(pitch), cp = cosf(pitch);
		float sy = sinf(yaw), cy = cosf(yaw);
		float sr = sinf(roll), cr = cosf(roll);

		Vec3 axisX(cp * cy, cp * sy, sp);
		Vec3 axisY(sr * sp * cy - cr * sy, sr * sp * sy + cr * cy, -sr * cp);
		Vec3 axisZ(-(cr * sp * cy + sr * sy), cy * sr - cr * sp * sy, cr * cp);

		Vec3 delta = world - camPos;
		Vec3 t(
			delta.x * axisY.x + delta.y * axisY.y + delta.z * axisY.z,
			delta.x * axisZ.x + delta.y * axisZ.y + delta.z * axisZ.z,
			delta.x * axisX.x + delta.y * axisX.y + delta.z * axisX.z);

		if (t.z < 1.f)
			return ImVec2(-1.f, -1.f);

		float fovRad = camFov * 0.01745329251f;
		float halfW = screenSize.x * 0.5f;
		float halfH = screenSize.y * 0.5f;
		float s = halfW / tanf(fovRad * 0.5f);

		return ImVec2(halfW + t.x * s / t.z, halfH - t.y * s / t.z);
	}

	inline Vec3 Bone(uintptr_t mesh, int idx)
	{
		if (!mesh) return {};

		uintptr_t arr = Mem::ReadPtr(mesh + Offsets::BoneArray);
		if (!arr) return {};

		FTransform bone{};
		FTransform c2w{};
		if (!Mem::Read(arr + (uintptr_t)idx * sizeof(FTransform), bone))
			return {};
		if (!Mem::Read(mesh + Offsets::ComponentToWorld, c2w))
			return {};

		// lazy, good enough for now
		return bone.translation + c2w.translation;
	}

	inline bool UpdateCam()
	{
		if (!pc) return false;
		uintptr_t cam = Mem::ReadPtr(pc + Offsets::PlayerCameraManager);
		if (!cam) return false;

		uintptr_t pov = cam + Offsets::CameraCachePrivate + Offsets::CameraCache_POV;
		FVectorD loc{};
		FRotatorD rot{};
		float fov = 80.f;

		if (!Mem::Read(pov + Offsets::POV_Location, loc))
			return false;
		Mem::Read(pov + Offsets::POV_Rotation, rot);
		Mem::Read(pov + Offsets::POV_FOV, fov);

		camPos = Vec3((float)loc.x, (float)loc.y, (float)loc.z);
		camRot = rot;
		if (fov > 5.f && fov < 160.f)
			camFov = fov;
		return true;
	}

	inline bool Refresh()
	{
		if (!base)
			base = (uintptr_t)GetModuleHandleA("FortniteClient-Win64-Shipping.exe");
		if (!base) return false;

		uintptr_t enc = Mem::ReadPtr(base + Offsets::GWorld);
		uworld = DecryptWorld(enc);
		if (!uworld) return false;

		gi = Mem::ReadPtr(uworld + Offsets::OwningGameInstance);
		gs = Mem::ReadPtr(uworld + Offsets::GameState);

		uintptr_t lpArr = Mem::ReadPtr(gi + Offsets::LocalPlayers);
		localPlayer = lpArr ? Mem::ReadPtr(lpArr) : 0;
		pc = localPlayer ? Mem::ReadPtr(localPlayer + Offsets::PlayerController) : 0;
		pawn = pc ? Mem::ReadPtr(pc + Offsets::AcknowledgedPawn) : 0;

		UpdateCam();
		return pawn != 0;
	}

	inline bool Init()
	{
		if (!IsFN()) return false;
		base = (uintptr_t)GetModuleHandleA("FortniteClient-Win64-Shipping.exe");
		screenSize = ImVec2((float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN));
		Refresh();
		return base != 0;
	}

	template <typename F>
	inline void ForEach(F&& fn)
	{
		if (!Refresh() || !gs)
			return;

		uintptr_t data = Mem::ReadPtr(gs + Offsets::PlayerArray);
		int count = 0;
		Mem::Read(gs + Offsets::PlayerArray + 8, count);
		if (!data || count <= 0 || count > 150)
			return;

		for (int i = 0; i < count; i++)
		{
			uintptr_t ps = Mem::ReadPtr(data + (uintptr_t)i * 8);
			if (!ps) continue;

			uintptr_t p = Mem::ReadPtr(ps + Offsets::PawnPrivate);
			if (!p || p == pawn) continue;

			uintptr_t mesh = Mem::ReadPtr(p + Offsets::Mesh);
			if (!mesh) continue;

			Player pl{};
			pl.pawn = p;
			pl.mesh = mesh;
			pl.root = Bone(mesh, Offsets::Bones::Root);
			pl.head = Bone(mesh, Offsets::Bones::Head);

			float hp = 100.f, mx = 100.f;
			Mem::Read(p + Offsets::CurrentHealth, hp);
			Mem::Read(p + Offsets::MaxHealth, mx);
			pl.hp = hp;
			pl.maxHp = mx > 1.f ? mx : 100.f;

			fn(pl);
		}
	}
}
