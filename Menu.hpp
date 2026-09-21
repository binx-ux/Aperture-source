#pragma once

#include "includes.hpp"
#include "Settings.hpp"
#include "auth.hpp"
#include "GameDetect.hpp"
#include <cstdio>

namespace Menu
{
	inline bool Open = false;
	inline bool FirstTime = true;
	inline int Tab = 0;

	inline void Style()
	{
		ImGuiStyle& s = ImGui::GetStyle();
		s.WindowRounding = 14.f;
		s.ChildRounding = 10.f;
		s.FrameRounding = 8.f;
		s.GrabRounding = 8.f;
		s.WindowBorderSize = 1.f;
		s.WindowPadding = ImVec2(0, 0);
		s.FramePadding = ImVec2(10, 7);
		s.ItemSpacing = ImVec2(8, 8);

		auto* c = s.Colors;
		c[ImGuiCol_WindowBg] = ImVec4(0.055f, 0.055f, 0.063f, 0.97f);
		c[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
		c[ImGuiCol_Border] = ImVec4(1, 1, 1, 0.10f);
		c[ImGuiCol_Text] = ImVec4(0.96f, 0.96f, 0.965f, 1);
		c[ImGuiCol_TextDisabled] = ImVec4(0.48f, 0.50f, 0.54f, 1);
		c[ImGuiCol_FrameBg] = ImVec4(1, 1, 1, 0.04f);
		c[ImGuiCol_FrameBgHovered] = ImVec4(1.f, 0.36f, 0.20f, 0.18f);
		c[ImGuiCol_FrameBgActive] = ImVec4(1.f, 0.36f, 0.20f, 0.28f);
		c[ImGuiCol_CheckMark] = ImVec4(1.f, 0.36f, 0.20f, 1);
		c[ImGuiCol_SliderGrab] = ImVec4(1.f, 0.36f, 0.20f, 1);
		c[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.11f, 1);
		c[ImGuiCol_ButtonHovered] = ImVec4(1.f, 0.36f, 0.20f, 0.22f);
		c[ImGuiCol_ButtonActive] = ImVec4(1.f, 0.36f, 0.20f, 1);
		c[ImGuiCol_Header] = ImVec4(1.f, 0.36f, 0.20f, 0.18f);
		c[ImGuiCol_PlotHistogram] = ImVec4(1.f, 0.36f, 0.20f, 1);
	}

	inline bool Nav(const char* id, const char* title, const char* sub, bool on)
	{
		ImGui::PushID(id);
		ImVec2 p = ImGui::GetCursorScreenPos();
		float w = ImGui::GetContentRegionAvail().x;
		bool click = ImGui::InvisibleButton("n", ImVec2(w, 48));
		ImDrawList* dl = ImGui::GetWindowDrawList();
		if (on || ImGui::IsItemHovered())
			dl->AddRectFilled(p, ImVec2(p.x + w, p.y + 48), on ? IM_COL32(255, 92, 51, 36) : IM_COL32(255, 255, 255, 10), 10.f);
		if (on)
			dl->AddRectFilled(ImVec2(p.x, p.y + 10), ImVec2(p.x + 3, p.y + 38), IM_COL32(255, 92, 51, 255), 2.f);
		dl->AddRectFilled(ImVec2(p.x + 12, p.y + 12), ImVec2(p.x + 34, p.y + 34), on ? IM_COL32(255, 92, 51, 255) : IM_COL32(26, 26, 29, 255), 7.f);
		dl->AddText(ImVec2(p.x + 44, p.y + 10), IM_COL32(244, 245, 246, 255), title);
		dl->AddText(ImVec2(p.x + 44, p.y + 27), IM_COL32(122, 127, 137, 255), sub);
		ImGui::PopID();
		ImGui::Dummy(ImVec2(0, 2));
		return click;
	}

	inline void DrawLoader()
	{
		Auth::Tick();

		ImGui::SetNextWindowSize(ImVec2(420, 280), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.f);

		ImGui::Begin("##loader", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

		ImDrawList* dl = ImGui::GetWindowDrawList();
		ImVec2 wp = ImGui::GetWindowPos();
		float ww = ImGui::GetWindowWidth();

		dl->AddRectFilled(wp, ImVec2(wp.x + ww, wp.y + 64), IM_COL32(255, 255, 255, 6));
		dl->AddLine(ImVec2(wp.x, wp.y + 64), ImVec2(wp.x + ww, wp.y + 64), IM_COL32(255, 255, 255, 18));

		ImGui::SetCursorPos(ImVec2(18, 16));
		ImVec2 logo = ImGui::GetCursorScreenPos();
		dl->AddRectFilled(logo, ImVec2(logo.x + 32, logo.y + 32), IM_COL32(255, 92, 51, 255), 9.f);
		ImGui::SetCursorPos(ImVec2(60, 16));
		ImGui::BeginGroup();
		ImGui::Text("Aperture Loader");
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.48f, 0.5f, 0.54f, 1));
		ImGui::Text("%s build", Detect::Name());
		ImGui::PopStyleColor();
		ImGui::EndGroup();

		ImGui::SetCursorPos(ImVec2(28, 88));
		ImGui::BeginGroup();
		ImGui::PushItemWidth(364);
		ImGui::Text("license key");
		ImGui::InputText("##key", Auth::Key, IM_ARRAYSIZE(Auth::Key), Auth::Busy ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_Password);
		ImGui::Dummy(ImVec2(0, 8));

		if (Auth::Busy)
		{
			ImGui::TextDisabled("%s", Auth::Status);
			ImGui::ProgressBar(Auth::Progress, ImVec2(364, 18), "");
		}
		else
		{
			if (Auth::Failed)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.35f, 0.3f, 1));
				ImGui::Text("%s", Auth::Status);
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::TextDisabled("enter key then continue");
			}

			ImGui::Dummy(ImVec2(0, 6));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.36f, 0.20f, 1));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.42f, 0.26f, 1));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.90f, 0.30f, 0.16f, 1));
			if (ImGui::Button("Launch", ImVec2(364, 36)))
				Auth::BeginLogin();
			ImGui::PopStyleColor(3);
		}
		ImGui::PopItemWidth();
		ImGui::EndGroup();

		ImGui::SetCursorPos(ImVec2(28, 245));
		ImGui::TextDisabled("local auth, no remote panel");

		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	inline void Drawing()
	{
		if (!AuthConnected)
		{
			DrawLoader();
			return;
		}

		ImGui::SetNextWindowSize(ImVec2(900, 540), ImGuiCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.f);

		if (!ImGui::Begin("Aperture", &Open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::End();
			ImGui::PopStyleVar(2);
			return;
		}

		ImDrawList* dl = ImGui::GetWindowDrawList();
		ImVec2 wp = ImGui::GetWindowPos();
		float ww = ImGui::GetWindowWidth();

		dl->AddRectFilled(wp, ImVec2(wp.x + ww, wp.y + 54), IM_COL32(255, 255, 255, 5));
		dl->AddLine(ImVec2(wp.x, wp.y + 54), ImVec2(wp.x + ww, wp.y + 54), IM_COL32(255, 255, 255, 16));

		ImGui::SetCursorPos(ImVec2(14, 11));
		ImVec2 logo = ImGui::GetCursorScreenPos();
		dl->AddRectFilled(logo, ImVec2(logo.x + 30, logo.y + 30), IM_COL32(255, 92, 51, 255), 8.f);
		ImGui::SetCursorPos(ImVec2(52, 12));
		ImGui::BeginGroup();
		ImGui::Text("Aperture");
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.48f, 0.5f, 0.54f, 1));
		ImGui::Text("%s", Detect::Name());
		ImGui::PopStyleColor();
		ImGui::EndGroup();

		ImGui::SameLine(ww - 120);
		ImGui::SetCursorPosY(20);
		ImGui::TextColored(ImVec4(0.21f, 0.82f, 0.73f, 1), "online");

		ImGui::SetCursorPos(ImVec2(0, 54));
		ImGui::BeginChild("body", ImVec2(0, 0), false);

		ImGui::BeginChild("nav", ImVec2(200, 0), false);
		ImGui::SetCursorPos(ImVec2(10, 12));
		ImGui::BeginGroup();
		if (Nav("aim", "Aim", "tracking", Tab == 0)) Tab = 0;
		if (Nav("vis", "Visual", "esp", Tab == 1)) Tab = 1;
		if (Nav("wep", "Weapon", "combat", Tab == 2)) Tab = 2;
		if (Nav("msc", "Misc", "extra", Tab == 3)) Tab = 3;
		ImGui::EndGroup();
		ImGui::EndChild();

		ImGui::SameLine();
		dl->AddLine(ImVec2(wp.x + 200, wp.y + 54), ImVec2(wp.x + 200, wp.y + ImGui::GetWindowHeight()), IM_COL32(255, 255, 255, 16));

		ImGui::BeginChild("main", ImVec2(0, 0), false);
		ImGui::SetCursorPos(ImVec2(18, 16));
		ImGui::BeginGroup();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 28.f);

		if (Tab == 0)
		{
			ImGui::Text("Aim assist");
			ImGui::TextDisabled("hold aim key to track");
			ImGui::Spacing();
			ImGui::Checkbox("enabled", &Settings::Aimbot::Enabled);
			ImGui::Checkbox("draw fov", &Settings::Aimbot::DrawFov);
			ImGui::Checkbox("humanize", &Settings::Aimbot::Humanize);
			ImGui::SliderInt("fov", &Settings::Aimbot::Fov, 15, 300);
			ImGui::SliderInt("smooth", &Settings::Aimbot::Smooth, 1, 25);
			ImGui::SliderFloat("max distance", &Settings::Aimbot::MaxDistance, 50.f, 800.f, "%.0f");

			const char* zones[] = { "head", "chest", "pelvis" };
			ImGui::Combo("hit zone", &Settings::Aimbot::HitZone, zones, 3);

			const char* prio[] = { "closest to crosshair", "lowest health" };
			ImGui::Combo("priority", &Settings::Aimbot::Priority, prio, 2);

			ImGui::TextDisabled("aim key: right mouse");

			if (Detect::Current == GameId::FiveM)
			{
				const char* filt[] = { "all", "players", "npcs" };
				ImGui::Combo("target filter", &Settings::Aimbot::TargetFilter, filt, 3);
			}
		}
		else if (Tab == 1)
		{
			ImGui::Text("Visuals");
			ImGui::Spacing();
			ImGui::Checkbox("box", &Settings::Visuals::Box);
			ImGui::Checkbox("corner box", &Settings::Visuals::CornerBox);
			ImGui::Checkbox("skeleton", &Settings::Visuals::Skeleton);
			ImGui::Checkbox("snapline", &Settings::Visuals::Snapline);
			ImGui::Checkbox("health bar", &Settings::Visuals::HealthBar);
			if (Detect::Current == GameId::FiveM)
			{
				ImGui::Checkbox("armor bar", &Settings::Visuals::ArmorBar);
				ImGui::Checkbox("show npcs", &Settings::Visuals::ShowNPCs);
			}
			ImGui::Checkbox("distance", &Settings::Visuals::Distance);
			ImGui::SliderFloat("esp distance", &Settings::Visuals::MaxDistance, 50.f, 1000.f, "%.0f");
			ImGui::ColorEdit4("enemy color", Settings::Visuals::EnemyColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		}
		else if (Tab == 2)
		{
			ImGui::Text("Weapon");
			ImGui::Spacing();
			if (Detect::Current == GameId::FiveM)
			{
				ImGui::Checkbox("no recoil", &Settings::Weapon::NoRecoil);
				ImGui::Checkbox("no spread", &Settings::Weapon::NoSpread);
			}
			else
			{
				ImGui::TextDisabled("weapon mods are FiveM only for now");
			}
		}
		else
		{
			ImGui::Text("Misc");
			ImGui::Spacing();
			ImGui::Checkbox("crosshair", &Settings::Misc::Crosshair);
			ImGui::Checkbox("watermark", &Settings::Misc::Watermark);
			ImGui::Checkbox("debug info", &Settings::Misc::ShowDebug);
			ImGui::TextDisabled("menu: insert");
			ImGui::Separator();
			ImGui::Text("session");
			ImGui::TextDisabled("game: %s", Detect::Name());
			ImGui::TextDisabled("auth: local");
		}

		ImGui::PopItemWidth();
		ImGui::EndGroup();
		ImGui::EndChild();
		ImGui::EndChild();
		ImGui::End();
		ImGui::PopStyleVar(2);
	}
}
