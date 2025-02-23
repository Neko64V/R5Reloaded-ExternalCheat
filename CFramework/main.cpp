#include "Cheat/FrameCore.h"
#include "Framework/Overlay/Overlay.h"

Overlay*	ov = new Overlay;
CFramework* cx = new CFramework;

void CFramework::Init()
{
	// ImGui io setting
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	// Load Font
	io.Fonts->AddFontFromMemoryCompressedTTF(RobotoRegular_compressed_data, RobotoRegular_compressed_size, 13.f, nullptr);

	// Load Icon
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.GlyphOffset.y = 2.f;
	icon = io.Fonts->AddFontFromMemoryCompressedTTF(FA_compressed_data, FA_compressed_size, 16.f, &icons_config, icons_ranges);
	io.Fonts->Build();
}

void Overlay::OverlayUserFunction()
{
	cx->MiscAll();

	cx->RenderInfo();

	if (g.g_ESP)
		cx->RenderESP();

	if (g.g_ShowMenu)
		cx->RenderMenu();
}

// DEBUG時にはコンソールウィンドウを表示する
#if _DEBUG
int main()
#else 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	// プロセスに接続する
	if (!m.AttachProcess("Apex Legends", InitializeMode::WINDOW_TITLE)) // 詳細は Framework/Utils/Memory/Memory.h を参照
		return 1;

	// ベースアドレスを取得する
	m.GetBaseAddress("r5apex.exe");

	// Overlay
	if (!ov->InitOverlay("Apex Legends", InitializeMode::WINDOW_TITLE)) // MemoryInitModeと同様
		return 2;

	cx->Init();

	// スレッドを作成
	std::thread([&]() { cx->UpdateList(); }).detach(); // ESP/AIM用にプレイヤーのデータをキャッシュする

	timeBeginPeriod(1);
	ov->OverlayLoop();
	ov->DestroyOverlay();
	m.DetachProcess();
	timeEndPeriod(1);
	g.g_Run = false;
	delete cx, ov;

	return 0;
}