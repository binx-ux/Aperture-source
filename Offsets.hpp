#pragma once

#include <cstdint>

// fn 42.20 - cheatoffsets, update when they patch
namespace Offsets
{
	constexpr uintptr_t GWorld = 0x1B2C5BA0;
	constexpr uintptr_t WorldMul = 0x6501B96661E130DDULL;
	constexpr uintptr_t WorldAdd = 0x79D95BD19230E74DULL;

	constexpr uint32_t PersistentLevel = 0x38;
	constexpr uint32_t GameState = 0x1C0;
	constexpr uint32_t OwningGameInstance = 0x238;
	constexpr uint32_t LocalPlayers = 0x38;
	constexpr uint32_t PlayerController = 0x30; // UPlayer

	constexpr uint32_t AcknowledgedPawn = 0x318;
	constexpr uint32_t PlayerCameraManager = 0x328;
	constexpr uint32_t CameraCachePrivate = 0x1590;
	constexpr uint32_t CameraCache_POV = 0x10;
	constexpr uint32_t POV_Location = 0x0;
	constexpr uint32_t POV_Rotation = 0x18;
	constexpr uint32_t POV_FOV = 0x30;

	constexpr uint32_t Mesh = 0x2F0;
	constexpr uint32_t RootComponent = 0x1B0;
	constexpr uint32_t PlayerArray = 0x288;
	constexpr uint32_t PawnPrivate = 0x2E8;

	constexpr uint32_t CurrentHealth = 0xD0C;
	constexpr uint32_t MaxHealth = 0xD10;

	constexpr uint32_t ComponentToWorld = 0x1E0; // not in sdk dump, might drift
	constexpr uint32_t BoneArray = 0x9D0; // CachedComponentSpaceTransforms

	namespace Bones
	{
		constexpr int Root = 0;
		constexpr int Pelvis = 2;
		constexpr int Chest = 7;
		constexpr int Neck = 66;
		constexpr int Head = 110;
	}
}
