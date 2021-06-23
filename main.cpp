#pragma once
#include "main.h"

#include <string>
#include <regex>
#include <cstdio>
#include <time.h>

bool open = true;
bool ShowActiveMods = false;

inline void AMUMSS_GUI_MAIN() {
	//ImGui::ShowDemoWindow();
	ImGui::SetNextWindowSize(ImVec2(450, 450));
	if (!open || !ImGui::Begin("AMUMSS", &open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Info"))
		{
			if (ImGui::MenuItem("AMUMSS - auto modbuilder-updater with MOD script definition system\n\nMade by Mjjstral aka MetaIdea and Wbertro aka TheBossBoy ")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	if (ImGui::Button("BUILD MODS", { 100, 25 })) {
		BuildMods();
	}
	ImGui::SameLine();
	if (ImGui::Button("VERIFY", { 100, 25 })) {

	}
	ImGui::Separator();
	ImGui::Checkbox("Copy mod to gamefolder", &CopyToGamefolder);
	ImGui::SameLine(230);
	ImGui::Checkbox("Update MBinCompiler", &UpdateMBinCompiler);
	ImGui::Checkbox("Combine Mods", &CombinedModPak);
	ImGui::SameLine(230);
	ImGui::Checkbox("Check mod conflicts", &CheckForModConflicts);
	ImGui::Checkbox("(Re)Create MapFileTrees", &ReCreateMapFileTree);
	ImGui::Separator();
	if (ImGui::BeginTabBar("TabBar1")) {
		if (ImGui::BeginTabItem("Script Collection")) {		
			ImGui::Text("Name"); ImGui::SameLine(); ImGui::Checkbox("Active", &ShowActiveMods); ImGui::SameLine(280); ImGui::Text("Author"); ImGui::SameLine(390); ImGui::Text("Update");
			ImGui::Separator();
			if (ImGui::BeginChild("#SCRIPT_LIST")) {
				for (size_t i = 0; i < ScriptList.size(); i++) {
					if (!ShowActiveMods || (ShowActiveMods && ScriptList[i].ACTIVE)) {
						if (ImGui::Selectable(ScriptList[i].MOD_NAME.c_str(), ScriptList[i].ACTIVE, 0, { 385 , 25 })) {
							ScriptList[i].ACTIVE = !ScriptList[i].ACTIVE;
						}
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip(ScriptList[i].MOD_TOOLTIP.c_str());
						}
						ImGui::SameLine(272);
						ImGui::Text(ScriptList[i].MOD_AUTHOR.c_str());
						//ImGui::SameLine(330);
						//ImGui::Text(ScriptList[i].MOD_NMS_VERSION.c_str());
						if (ScriptList[i].UPDATEABLE) {
							ImGui::SameLine(390);
							if (ImGui::Button("U", { 25, 25 })) {
								UpdateScriptFromPastebin((LPCTSTR)ScriptList[i].MOD_UPDATE_LINK.c_str(), (LPCTSTR)ScriptList[i].SCRIPT_PATH.c_str());
							}
							if (ImGui::IsItemHovered()) {
								ImGui::SetTooltip(ScriptList[i].MOD_UPDATE_LINK.c_str());
							}
						}
					}
				}
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

char WINNAME[16] = " ";
int testint = 1;
float colortest[4] = { 1.f,0.f,1.f,1.f };
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
		break;
	case WM_NCHITTEST:
	{
		ImVec2 Shit = ImGui::GetMousePos();
		if (Shit.y < 10 && Shit.x < 20)
		{
			LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);
			if (hit == HTCLIENT) hit = HTCAPTION;
			return hit;
		}
		else break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void InitImGui(LPDIRECT3DDEVICE9 device)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuiStyle& style = ImGui::GetStyle();
}

int main(int, char**)
{
	InitScriptList();

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = WINNAME;
	wc.lpszMenuName = WINNAME;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	RegisterClassEx(&wc);
	g_hwnd = CreateWindowEx(/*WS_EX_TOPMOST*/ /*| WS_EX_TRANSPARENT |*/ WS_EX_LAYERED, WINNAME, WINNAME, WS_POPUP, 0, 0, Width, Height, 0, 0, 0, 0);
	//g_hwnd = CreateWindowEx(WS_EX_TOPMOST /*| WS_EX_TRANSPARENT*/ | WS_EX_LAYERED, WINNAME, WINNAME, WS_POPUP, ImGui::PopStyleColor();, Width, Height, 0, 0, 0, 0);

	SetLayeredWindowAttributes(g_hwnd, 0, 255, LWA_ALPHA);
	SetLayeredWindowAttributes(g_hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	DwmExtendFrameIntoClientArea(g_hwnd, &MARGIN);

	if (CreateDeviceD3D(g_hwnd) < 0)
	{
		CleanupDeviceD3D();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	ShowWindow(g_hwnd, SW_SHOWDEFAULT);
	UpdateWindow(g_hwnd);
	SetWindowPos(g_hwnd, NULL /*HWND_TOPMOST*/, 0, 0, 0, 0, SWP_NOSIZE/*SWP_NOMOVE | SWP_NOSIZE*/);
	ImGui_ImplWin32_Init(g_hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	ImGui_ImplWin32_Init(g_hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		if (!open) ExitProcess(0);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		ResetDevice();
		g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			AMUMSS_GUI_MAIN();

			ImGui::End();
			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) ResetDevice();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(g_hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}


