#pragma once
#include "..\ImGui\imgui.h"
#include "..\ImGui\imgui_impl_win32.h"
#include "..\ImGui\imgui_impl_dx11.h"
#include "..\Utils\Utils.h";
#include <dwmapi.h>
#include <thread>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

class Overlay
{
private:
	// オーバーレイに登録する情報
	WNDCLASSEXA wc{};
	HWND m_Hwnd;
	char m_OTitle[32] = "The Overlay";
	char m_OClass[32] = "WND_CLS";

	// ターゲットウィンドウに関する情報
	HWND m_TargetHwnd;
	char m_TargetClass[128]{};
	RECT m_GameRect{};
	POINT m_GamePoint{};

	bool CreateOverlay();
	HWND GetTargetWindow(const std::string processName);	// 実行ファイル名からウィンドウハンドルを取得する
public:
	bool InitOverlay(const char* targetName, int mode);
	void OverlayLoop();
	void DestroyOverlay();
	void OverlayManager();
};

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;
