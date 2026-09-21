#pragma once

#include "Classes.hpp"
#include "FiveM.hpp"
#include "Settings.hpp"
#include "GameDetect.hpp"

namespace Aimbot
{
	inline Player fnTarget{};
	inline bool hasFnTarget = false;
	inline uintptr_t fmTarget = 0;

	inline bool LocalDistCheck(const Vec3& pos)
	{
		if (Detect::Current == GameId::Fortnite)
		{
			if (Game::camPos.Dist(pos) > Settings::Aimbot::MaxDistance)
				return false;
		}
		return true;
	}

	inline int BoneMask()
	{
		switch (Settings::Aimbot::HitZone)
		{
		case 1: return 0x60F2;
		case 2: return 0xF9BB;
		default: return 0x796e;
		}
	}

	inline void FindFN()
	{
		hasFnTarget = false;
		fnTarget = {};
		if (!Settings::Aimbot::Enabled) return;

		ImVec2 mid(Game::screenSize.x * 0.5f, Game::screenSize.y * 0.5f);
		float best = (float)Settings::Aimbot::Fov;

		Game::ForEach([&](const Player& p)
		{
			if (!LocalDistCheck(p.root)) return;

			Vec3 spot = p.head;
			if (Settings::Aimbot::HitZone == 1) spot = Game::Bone(p.mesh, Offsets::Bones::Chest);
			else if (Settings::Aimbot::HitZone == 2) spot = Game::Bone(p.mesh, Offsets::Bones::Pelvis);
			ImVec2 s = Game::W2S(spot);
			if (!OnScreen(s)) return;

			float score = Game::Dist2D(mid, s);
			if (Settings::Aimbot::Priority == 1) score = p.hp;

			if (score < best)
			{
				best = score;
				fnTarget = p;
				hasFnTarget = true;
			}
		});
	}

	inline void FindFM()
	{
		fmTarget = 0;
		if (!Settings::Aimbot::Enabled || !FiveM::Ready) return;

		auto local = FiveM::Local();
		if (!local.Valid()) return;

		ImVec2 mid(FiveM::WindowSize.x * 0.5f, FiveM::WindowSize.y * 0.5f);
		float best = (float)Settings::Aimbot::Fov;
		int maxP = FiveM::PedMax();
		if (maxP <= 0 || maxP > 256) return;

		for (int i = 0; i < maxP; i++)
		{
			auto ped = FiveM::PedAt(i);
			if (!ped.Valid() || ped.ptr == local.ptr) continue;
			if (ped.Health() <= 0.f) continue;

			bool isPl = ped.IsPlayer();
			if (Settings::Aimbot::TargetFilter == 1 && !isPl) continue;
			if (Settings::Aimbot::TargetFilter == 2 && isPl) continue;

			Vec3 c = ped.Coord();
			if (local.Coord().Dist(c) > Settings::Aimbot::MaxDistance) continue;

			ImVec2 head = FiveM::GetBoneW2S(ped.ptr, BoneMask());
			if (!OnScreen(head)) continue;

			float d = Game::Dist2D(mid, head);
			if (d < best)
			{
				best = d;
				fmTarget = ped.ptr;
			}
		}
	}

	inline void MoveTo(ImVec2 screen)
	{
		if (screen.x < 0.f || screen.y < 0.f) return;

		float cx = (Detect::Current == GameId::FiveM ? FiveM::WindowSize.x : Game::screenSize.x) * 0.5f;
		float cy = (Detect::Current == GameId::FiveM ? FiveM::WindowSize.y : Game::screenSize.y) * 0.5f;

		float dx = screen.x - cx;
		float dy = screen.y - cy;
		int sm = Settings::Aimbot::Smooth < 1 ? 1 : Settings::Aimbot::Smooth;
		if (Settings::Aimbot::Humanize) sm += 2;
		dx /= (float)sm;
		dy /= (float)sm;
		mouse_event(MOUSEEVENTF_MOVE, (DWORD)dx, (DWORD)dy, 0, 0);
	}

	inline void Tick()
	{
		if (!Settings::Aimbot::Enabled) return;
		if (!(GetAsyncKeyState(Settings::Aimbot::Hotkey) & 0x8000)) return;

		if (Detect::Current == GameId::Fortnite)
		{
			FindFN();
			if (!hasFnTarget) return;
			Vec3 spot = fnTarget.head;
			if (Settings::Aimbot::HitZone == 1) spot = Game::Bone(fnTarget.mesh, Offsets::Bones::Chest);
			else if (Settings::Aimbot::HitZone == 2) spot = Game::Bone(fnTarget.mesh, Offsets::Bones::Pelvis);
			MoveTo(Game::W2S(spot));
		}
		else if (Detect::Current == GameId::FiveM)
		{
			FindFM();
			if (!fmTarget) return;
			MoveTo(FiveM::GetBoneW2S(fmTarget, BoneMask()));
		}
	}

	inline void DrawFov()
	{
		if (!Settings::Aimbot::DrawFov) return;
		float cx = (Detect::Current == GameId::FiveM ? FiveM::WindowSize.x : Game::screenSize.x) * 0.5f;
		float cy = (Detect::Current == GameId::FiveM ? FiveM::WindowSize.y : Game::screenSize.y) * 0.5f;
		ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(cx, cy), (float)Settings::Aimbot::Fov, ImColor(255, 92, 51, 110), 64, 1.f);
	}

	inline void DrawEspFN()
	{
		Game::ForEach([&](const Player& p)
		{
			if (Game::camPos.Dist(p.root) > Settings::Visuals::MaxDistance) return;

			ImVec2 head = Game::W2S(p.head);
			ImVec2 root = Game::W2S(p.root);
			if (!OnScreen(head) && !OnScreen(root)) return;

			float h = fabsf(root.y - head.y);
			float w = h * 0.45f;
			ImVec2 tl(head.x - w * 0.5f, head.y);
			ImVec2 br(head.x + w * 0.5f, root.y);
			ImColor col(Settings::Visuals::EnemyColor[0], Settings::Visuals::EnemyColor[1], Settings::Visuals::EnemyColor[2], Settings::Visuals::EnemyColor[3]);

			if (Settings::Visuals::Box)
				ImGui::GetBackgroundDrawList()->AddRect(tl, br, col, 0.f, 0, 1.2f);

			if (Settings::Visuals::CornerBox)
			{
				float cw = w * 0.25f, ch = h * 0.2f;
				auto L = [&](float x1, float y1, float x2, float y2) {
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), col, 1.4f);
				};
				L(tl.x, tl.y, tl.x + cw, tl.y); L(tl.x, tl.y, tl.x, tl.y + ch);
				L(br.x, tl.y, br.x - cw, tl.y); L(br.x, tl.y, br.x, tl.y + ch);
				L(tl.x, br.y, tl.x + cw, br.y); L(tl.x, br.y, tl.x, br.y - ch);
				L(br.x, br.y, br.x - cw, br.y); L(br.x, br.y, br.x, br.y - ch);
			}

			if (Settings::Visuals::Snapline)
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Game::screenSize.x * 0.5f, Game::screenSize.y), root, ImColor(255, 255, 255, 110), 1.f);

			if (Settings::Visuals::HealthBar)
			{
				float r = p.maxHp > 0.f ? p.hp / p.maxHp : 1.f;
				if (r < 0.f) r = 0.f; if (r > 1.f) r = 1.f;
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(tl.x - 4.f, br.y), ImVec2(tl.x - 4.f, br.y - h * r), ImColor(0, 255, 0, 200), 2.f);
			}

			if (Settings::Visuals::Distance)
			{
				char buf[32];
				sprintf_s(buf, "%.0fm", Game::camPos.Dist(p.root) / 100.f);
				ImGui::GetBackgroundDrawList()->AddText(ImVec2(head.x - 10.f, br.y + 2.f), ImColor(220, 220, 220, 200), buf);
			}

			if (Settings::Visuals::Skeleton && p.mesh)
			{
				ImVec2 neck = Game::W2S(Game::Bone(p.mesh, Offsets::Bones::Neck));
				ImVec2 chest = Game::W2S(Game::Bone(p.mesh, Offsets::Bones::Chest));
				ImVec2 pelvis = Game::W2S(Game::Bone(p.mesh, Offsets::Bones::Pelvis));
				auto line = [&](ImVec2 a, ImVec2 b) {
					if (OnScreen(a) && OnScreen(b))
						ImGui::GetBackgroundDrawList()->AddLine(a, b, ImColor(255, 255, 255, 170), 1.f);
				};
				line(head, neck); line(neck, chest); line(chest, pelvis);
			}
		});
	}

	inline void DrawEspFM()
	{
		if (!FiveM::Ready) return;
		auto local = FiveM::Local();
		if (!local.Valid()) return;

		int maxP = FiveM::PedMax();
		if (maxP <= 0 || maxP > 256) return;

		for (int i = 0; i < maxP; i++)
		{
			auto ped = FiveM::PedAt(i);
			if (!ped.Valid() || ped.ptr == local.ptr) continue;
			if (ped.Health() <= 0.f) continue;

			bool isPl = ped.IsPlayer();
			if (!isPl && !Settings::Visuals::ShowNPCs) continue;

			Vec3 c = ped.Coord();
			float dist = local.Coord().Dist(c);
			if (dist > Settings::Visuals::MaxDistance) continue;

			ImVec2 head = FiveM::GetBoneW2S(ped.ptr, 0x796e);
			ImVec2 foot = FiveM::WorldToScreen(c);
			if (!OnScreen(head) && !OnScreen(foot)) continue;

			float h = fabsf(foot.y - head.y);
			float w = h * 0.45f;
			ImVec2 tl(head.x - w * 0.5f, head.y);
			ImVec2 br(head.x + w * 0.5f, foot.y);
			ImColor col = isPl
				? ImColor(Settings::Visuals::EnemyColor[0], Settings::Visuals::EnemyColor[1], Settings::Visuals::EnemyColor[2], Settings::Visuals::EnemyColor[3])
				: ImColor(0.7f, 0.7f, 0.7f, 0.7f);

			if (Settings::Visuals::Box)
				ImGui::GetBackgroundDrawList()->AddRect(tl, br, col, 0.f, 0, 1.2f);

			if (Settings::Visuals::Snapline)
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(FiveM::WindowSize.x * 0.5f, FiveM::WindowSize.y), foot, ImColor(255, 255, 255, 100), 1.f);

			if (Settings::Visuals::HealthBar)
			{
				float r = ped.Health() / 200.f;
				if (r > 1.f) r = 1.f; if (r < 0.f) r = 0.f;
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(tl.x - 4.f, br.y), ImVec2(tl.x - 4.f, br.y - h * r), ImColor(0, 255, 0, 200), 2.f);
			}

			if (Settings::Visuals::ArmorBar)
			{
				float a = ped.Armor() / 100.f;
				if (a > 1.f) a = 1.f; if (a < 0.f) a = 0.f;
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(tl.x - 8.f, br.y), ImVec2(tl.x - 8.f, br.y - h * a), ImColor(50, 120, 255, 200), 2.f);
			}

			if (Settings::Visuals::Distance)
			{
				char buf[32];
				sprintf_s(buf, "%.0fm", dist);
				ImGui::GetBackgroundDrawList()->AddText(ImVec2(head.x - 10.f, br.y + 2.f), ImColor(220, 220, 220, 200), buf);
			}

			if (Settings::Visuals::Skeleton)
			{
				ImVec2 neck = FiveM::GetBoneW2S(ped.ptr, 0x9995);
				ImVec2 pelvis = FiveM::GetBoneW2S(ped.ptr, 0x2e28);
				auto line = [&](ImVec2 a, ImVec2 b) {
					if (OnScreen(a) && OnScreen(b))
						ImGui::GetBackgroundDrawList()->AddLine(a, b, ImColor(255, 255, 255, 160), 1.f);
				};
				line(head, neck); line(neck, pelvis);
			}
		}
	}

	inline void DrawEsp()
	{
		if (Detect::Current == GameId::Fortnite) DrawEspFN();
		else if (Detect::Current == GameId::FiveM) DrawEspFM();
	}

	inline void WeaponTick()
	{
		if (Detect::Current != GameId::FiveM || !FiveM::Ready) return;
		auto local = FiveM::Local();
		if (!local.Valid()) return;

		if (Settings::Weapon::NoRecoil)
			local.SetRecoil(0.f);
		if (Settings::Weapon::NoSpread)
			local.SetSpread(0.f);
	}
}
