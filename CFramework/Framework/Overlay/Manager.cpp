#include "Overlay.h"

bool Overlay::InitOverlay(const char* targetName, int InitMode)
{
    if (InitMode == WINDOW_TITLE || InitMode == WINDOW_CLASS)
    {
        g.g_GameHwnd = InitMode == WINDOW_TITLE ? FindWindowA(NULL, targetName) : FindWindowA(targetName, NULL);

        if (!g.g_GameHwnd) {
            MessageBoxA(nullptr, "target window not found", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }
    }
    else if (InitMode == PROCESS)
    {
        g.g_GameHwnd = GetTargetWindow(targetName);

        if (!g.g_GameHwnd) {
           MessageBoxA(nullptr, "target process not found", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }
    }
    else
    {
        MessageBoxA(nullptr, "Invalid init option", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
        return false;
    }

    // ToDo - このままでいいか考える
    GetClassNameA(g.g_GameHwnd, m_TargetClass, sizeof(m_TargetClass));

    return CreateOverlay();
}

void Overlay::OverlayManager()
{
    // Window Check
    HWND hWnd = FindWindowA(m_TargetClass, NULL);
    if (!hWnd) {
        g.g_Run = false;
        return;
    }

    // オーバーレイをゲームの上に配置
    HWND ProcessHwnd = GetWindow(hWnd, GW_HWNDPREV);
    SetWindowPos(m_Hwnd, ProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // ShowMenu Toggle
    if (GetKeyState(g.g_MenuKey) && !g.g_ShowMenu) {
        g.g_ShowMenu = true;
    }
    else if (!GetKeyState(g.g_MenuKey) && g.g_ShowMenu) {
        g.g_ShowMenu = false;
    }

    // サイズを取得
    RECT TmpRect{};
    POINT TmpPoint{};
    GetClientRect(hWnd, &TmpRect);
    ClientToScreen(hWnd, &TmpPoint);

    // ImGuiにマウス入力を渡す
    POINT MousePos{};
    GetCursorPos(&MousePos);
    ImGui::GetIO().MousePos = ImVec2(MousePos.x - TmpPoint.x, MousePos.y - TmpPoint.y);
    ImGui::GetIO().MouseDown[0] = IsKeyDown(VK_LBUTTON);

    // Window Resizer
    if (TmpRect.left != g.g_GameRect.left || TmpRect.bottom != g.g_GameRect.bottom || TmpRect.top != g.g_GameRect.top || TmpRect.right != g.g_GameRect.right || TmpPoint.x != g.g_GamePoint.x || TmpPoint.y != g.g_GamePoint.y)
    {
        g.g_GameRect = TmpRect;
        g.g_GamePoint = TmpPoint;
        SetWindowPos(m_Hwnd, nullptr, TmpPoint.x, TmpPoint.y, g.g_GameRect.right, g.g_GameRect.bottom, SWP_NOREDRAW);
    }
}

HWND Overlay::GetTargetWindow(const std::string processName)
{
    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(PROCESSENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    while (Process32Next(snapShot, &entry))
    {
        if (!processName.compare(entry.szExeFile))
            break;
    }

    CloseHandle(snapShot);

    HWND hwnd = GetTopWindow(NULL);
    do {
        if (GetWindowLong(hwnd, 8) != 0 || !IsWindowVisible(hwnd))
            continue;
        DWORD ProcessID;
        GetWindowThreadProcessId(hwnd, &ProcessID);
        if (entry.th32ProcessID == ProcessID)
            return hwnd;
    } while ((hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) != NULL);

    return NULL;
}
