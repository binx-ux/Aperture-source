#include "Core.hpp"

#include "d3d_Hook.hpp"
#include "Menu.hpp"
#include "Classes.hpp"
#include "Settings.hpp"
#include "Aimbot.hpp"
#include "auth.hpp"
#include "GameDetect.hpp"
#include "FiveM.hpp"

bool IsValid = false;
bool AuthConnected = false;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void InitImGui()
{
	using namespace DirectX;

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX11_Init(pDevice, pContext);
	Menu::Style();
}

LRESULT __stdcall WindowHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Menu::Open || !AuthConnected)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return true;
	}
	return CallWindowProcA(DirectX::WindowEx, hWnd, msg, wParam, lParam);
}

bool BindD3DInfo(IDXGISwapChain* swap)
{
	if (FAILED(swap->GetDevice(__uuidof(ID3D11Device), (void**)&DirectX::pDevice)))
		return false;

	DirectX::pDevice->GetImmediateContext(&DirectX::pContext);

	DXGI_SWAP_CHAIN_DESC desc{};
	swap->GetDesc(&desc);
	DirectX::Window = desc.OutputWindow;

	ID3D11Texture2D* back = nullptr;
	swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back);
	DirectX::pDevice->CreateRenderTargetView(back, nullptr, &DirectX::renderTargetView);
	back->Release();

	DirectX::WindowEx = (WNDPROC)SetWindowLongPtrA(DirectX::Window, GWLP_WNDPROC, (LONG_PTR)WindowHandler);
	InitImGui();
	Menu::FirstTime = false;
	Menu::Open = true; // show loader first
	return true;
}

HRESULT __stdcall PresentHook(IDXGISwapChain* swap, UINT sync, UINT flags)
{
	if (Menu::FirstTime)
	{
		if (!BindD3DInfo(swap))
			return DirectX::OriginalPresent(swap, sync, flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Game::screenSize = ImGui::GetIO().DisplaySize;
	FiveM::WindowSize = Game::screenSize;

	if (AuthConnected && (GetAsyncKeyState(VK_INSERT) & 1))
		Menu::Open = !Menu::Open;

	bool capture = Menu::Open || !AuthConnected;
	ImGui::GetIO().MouseDrawCursor = capture;
	ImGui::GetIO().WantCaptureKeyboard = capture;

	if (AuthConnected)
	{
		Aimbot::Tick();
		Aimbot::DrawFov();
		Aimbot::DrawEsp();
		Aimbot::WeaponTick();

		if (Settings::Misc::Watermark)
		{
			ImDrawList* dl = ImGui::GetBackgroundDrawList();
			dl->AddRectFilled(ImVec2(14, 14), ImVec2(150, 40), IM_COL32(14, 14, 16, 200), 7.f);
			dl->AddRectFilled(ImVec2(22, 22), ImVec2(36, 36), IM_COL32(255, 92, 51, 255), 4.f);
			char buf[64];
			sprintf_s(buf, "Aperture | %s", Detect::Name());
			dl->AddText(ImVec2(44, 22), IM_COL32(244, 245, 246, 255), buf);
		}

		if (Settings::Misc::Crosshair)
		{
			ImVec2 c(Game::screenSize.x * 0.5f, Game::screenSize.y * 0.5f);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(c.x - 5, c.y), ImVec2(c.x + 5, c.y), ImColor(255, 92, 51, 255), 1.f);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(c.x, c.y - 5), ImVec2(c.x, c.y + 5), ImColor(255, 92, 51, 255), 1.f);
		}

		if (Settings::Misc::ShowDebug)
		{
			char dbg[128];
			sprintf_s(dbg, "game=%s fn_pawn=%p fm_ready=%d", Detect::Name(), (void*)Game::pawn, (int)FiveM::Ready);
			ImGui::GetBackgroundDrawList()->AddText(ImVec2(14, 50), IM_COL32(180, 180, 180, 180), dbg);
		}
	}

	if (Menu::Open || !AuthConnected)
	{
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), Game::screenSize, ImColor(7, 7, 8, 120));
		Menu::Drawing();
	}

	ImGui::Render();
	DirectX::pContext->OMSetRenderTargets(1, &DirectX::renderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return DirectX::OriginalPresent(swap, sync, flags);
}

bool Core::Init()
{
	Sleep(4000);

	while (!DirectX::OverlayHooked)
	{
		if (Hook::Init())
			DirectX::OverlayHooked = Hook::Present((void**)&DirectX::OriginalPresent, PresentHook);
		Sleep(200);
	}
	return true;
}
