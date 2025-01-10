#include "Overlay.h"

bool Overlay::InitOverlay(const char* targetName, int InitMode)
{
    if (InitMode == WINDOW_TITLE || InitMode == WINDOW_CLASS)
    {
        m_hTargetHwnd = InitMode == WINDOW_TITLE ? FindWindowA(NULL, targetName) : FindWindowA(targetName, NULL);

        if (!m_hTargetHwnd) {
            MessageBoxA(nullptr, "target window not found", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }
    }
    else if (InitMode == PROCESS)
    {
        m_hTargetHwnd = GetTargetWindow(targetName);

        if (!m_hTargetHwnd) {
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
    GetClassNameA(m_hTargetHwnd, m_TargetClass, sizeof(m_TargetClass));

    return CreateOverlay();
}

// [+]オーバーレイのウィンドウをゲームのウィンドウの上やサイズに合わせるための処理です。
void Overlay::OverlayManager()
{
    // Window Check
    HWND CheckHwnd = FindWindowA(m_TargetClass, NULL);
    if (!CheckHwnd) {
        g.g_Run = false;
        return;
    }

    // StreamProof
    DWORD Flag = NULL;
    GetWindowDisplayAffinity(m_Hwnd, &Flag);
    if (g.g_StreamProof && Flag == WDA_NONE)
        SetWindowDisplayAffinity(m_Hwnd, WDA_EXCLUDEFROMCAPTURE);
    else if (!g.g_StreamProof && Flag == WDA_EXCLUDEFROMCAPTURE)
        SetWindowDisplayAffinity(m_Hwnd, WDA_NONE);

    // Window Style Changer
    HWND ForegroundWindow = GetForegroundWindow();
    LONG TmpLong = GetWindowLong(m_Hwnd, GWL_EXSTYLE);

    if (g.g_ShowMenu && MenuStyle != TmpLong)
        SetWindowLong(m_Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
    else if (!g.g_ShowMenu && ESPStyle != TmpLong)
        SetWindowLong(m_Hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);

    // ShowMenu Toggle
    if (GetKeyState(g.g_MenuKey) && !g.g_ShowMenu) {
        if (ForegroundWindow != m_Hwnd)
            SetForegroundWindow(m_Hwnd);

        g.g_ShowMenu = true;
    }
    else if (!GetKeyState(g.g_MenuKey) && g.g_ShowMenu) {
        if (ForegroundWindow != CheckHwnd)
            SetForegroundWindow(CheckHwnd);

        g.g_ShowMenu = false;
    }

    // Window Resizer
    RECT TmpRect{};
    POINT TmpPoint{};
    GetClientRect(CheckHwnd, &TmpRect);
    ClientToScreen(CheckHwnd, &TmpPoint);

    if (TmpRect.left != g.g_GameRect.left || TmpRect.bottom != g.g_GameRect.bottom || TmpRect.top != g.g_GameRect.top || TmpRect.right != g.g_GameRect.right || TmpPoint.x != g.g_GamePoint.x || TmpPoint.y != g.g_GamePoint.y)
    {
        g.g_GameRect = TmpRect;
        g.g_GamePoint = TmpPoint;
        SetWindowPos(m_Hwnd, nullptr, TmpPoint.x, TmpPoint.y, g.g_GameRect.right, g.g_GameRect.bottom, SWP_NOREDRAW);
    }
}

HWND Overlay::GetTargetWindow(const std::string processName)
{
    DWORD PID = NULL;
    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(PROCESSENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    while (Process32Next(snapShot, &entry))
    {
        if (!processName.compare(entry.szExeFile))
        {
            PID = entry.th32ProcessID;
            break;
        }
    }

    CloseHandle(snapShot);

    HWND hwnd = GetTopWindow(NULL);
    do {
        if (GetWindowLong(hwnd, 8) != 0 || !IsWindowVisible(hwnd))
            continue;
        DWORD ProcessID;
        GetWindowThreadProcessId(hwnd, &ProcessID);
        if (PID == ProcessID)
            return hwnd;
    } while ((hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) != NULL);

    return NULL;
}
