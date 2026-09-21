#pragma once

namespace Settings
{
	namespace Aimbot
	{
		inline bool Enabled = false;
		inline bool DrawFov = true;
		inline bool Humanize = true;
		inline bool VisibleOnly = false;
		inline int Fov = 80;
		inline int Smooth = 5;
		inline int HitZone = 0;
		inline int Priority = 0;
		inline int Hotkey = VK_RBUTTON;
		inline float MaxDistance = 250.f;
		inline int TargetFilter = 0; // 0 all 1 players 2 npcs (fivem)
	}

	namespace Visuals
	{
		inline bool Box = false;
		inline bool CornerBox = false;
		inline bool Skeleton = false;
		inline bool Snapline = false;
		inline bool HealthBar = false;
		inline bool ArmorBar = false;
		inline bool Name = false;
		inline bool Distance = false;
		inline bool ShowNPCs = true;
		inline float MaxDistance = 400.f;
		inline float EnemyColor[4] = { 1.f, 0.36f, 0.20f, 0.86f };
	}

	namespace Weapon
	{
		inline bool NoRecoil = false;
		inline bool NoSpread = false;
	}

	namespace Misc
	{
		inline bool Crosshair = false;
		inline bool Watermark = true;
		inline bool StreamProof = false;
		inline bool ShowDebug = false;
	}
}
