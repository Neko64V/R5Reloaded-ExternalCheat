#include "Cheat/FrameCore.h"
#include "Framework/Overlay/Overlay.h"

Overlay*	ov = new Overlay;
CFramework* cx = new CFramework;

// DEBUG時にはコンソールウィンドウを表示する
#if _DEBUG
int main()
#else 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	// Fix DPI Scale
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	// Apexのウィンドウをベースにして初期化を行う
	if (!m.AttachProcess("Apex Legends", InitMode::WINDOW_TITLE)) // 詳細は Memory/Memory.h を参照
		return 1;

	// Overlay
	if (!ov->InitOverlay("Apex Legends", InitMode::WINDOW_TITLE)) // MemoryInitModeと同様
		return 2;

	// スレッドを作成
	std::thread([&]() { cx->UpdateList(); }).detach(); // ESP/AIM用にプレイヤーのデータをキャッシュする

	ov->OverlayLoop();
	ov->DestroyOverlay();
	m.DetachProcess();
	g.g_Run = false;
	delete cx, ov;

	return 0;
}

void Overlay::OverlayLoop()
{
	while (g.g_Run)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// オーバーレイウィンドウの位置やサイズ等のチェック
		OverlayManager();
		cx->MiscAll();

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		cx->RenderInfo();

		if (g.g_ESP)
			cx->RenderESP();

		if (g.g_ShowMenu)
			cx->RenderMenu();

		ImGui::Render();
		static const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0);
	}
}