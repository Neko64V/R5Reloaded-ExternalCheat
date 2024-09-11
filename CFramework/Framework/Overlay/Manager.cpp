#include "Overlay.h"

LONG MenuStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
LONG ESPStyle = WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST;

bool Overlay::InitOverlay(const char* targetName, int mode)
{
    switch (mode)
    {
    case InitMode::WINDOW_TITLE:
        TargetHwnd = FindWindowA(NULL, targetName);

        if (!TargetHwnd) {
            MessageBoxA(nullptr, "Target not found Mode: 0", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }
        break;
    case InitMode::WINDOW_CLASS:
        TargetHwnd = FindWindowA(targetName, NULL);

        if (!TargetHwnd) {
            MessageBoxA(nullptr, "Target not found Mode: 1", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }
        break;
    case InitMode::PROCESS: 
        TargetHwnd = GetTargetWindow(targetName);

        if (!TargetHwnd) {
            MessageBoxA(nullptr, "Target not found Mode: 2", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }
        break;
    default:
        return false;
    }

    GetClassNameA(TargetHwnd, TargetClass, sizeof(TargetClass));

    return CreateOverlay();
}

// [+]オーバーレイのウィンドウをゲームのウィンドウの上やサイズに合わせるための処理です。
void Overlay::OverlayManager()
{
    while (g.Run)
    {
        // Window Check
        HWND CheckHwnd = FindWindowA(TargetClass, NULL);
        if (!CheckHwnd) {
            g.Run = false;
            break;
        }

        // StreamProof
        DWORD Flag = NULL;
        GetWindowDisplayAffinity(Hwnd, &Flag);
        if (g.StreamProof && Flag == WDA_NONE)
            SetWindowDisplayAffinity(Hwnd, WDA_EXCLUDEFROMCAPTURE);
        else if (!g.StreamProof && Flag == WDA_EXCLUDEFROMCAPTURE)
            SetWindowDisplayAffinity(Hwnd, WDA_NONE);

        // Window Style Changer
        HWND ForegroundWindow = GetForegroundWindow();
        LONG TmpLong = GetWindowLong(Hwnd, GWL_EXSTYLE);

        if (g.ShowMenu && MenuStyle != TmpLong)
            SetWindowLong(Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
        else if (!g.ShowMenu && ESPStyle != TmpLong)
            SetWindowLong(Hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);

        // ShowMenu
        static bool menu_key = false;
        if (IsKeyDown(g.MenuKey) && !menu_key)
        {
            g.ShowMenu = !g.ShowMenu;

            if (g.ShowMenu && ForegroundWindow != Hwnd)
                SetForegroundWindow(Hwnd);
            else if (!g.ShowMenu && ForegroundWindow != CheckHwnd)
                SetForegroundWindow(CheckHwnd);

            menu_key = true;
        }
        else if (!IsKeyDown(g.MenuKey) && menu_key)
        {
            menu_key = false;
        }

        // Window Resizer
        RECT TmpRect{};
        POINT TmpPoint{};
        GetClientRect(CheckHwnd, &TmpRect);
        ClientToScreen(CheckHwnd, &TmpPoint);

        // Resizer
        if (TmpRect.left != g.GameRect.left || TmpRect.bottom != g.GameRect.bottom || TmpRect.top != g.GameRect.top || TmpRect.right != g.GameRect.right || TmpPoint.x != g.GamePoint.x || TmpPoint.y != g.GamePoint.y)
        {
            g.GameRect = TmpRect;
            g.GamePoint = TmpPoint;
            SetWindowPos(Hwnd, nullptr, TmpPoint.x, TmpPoint.y, g.GameRect.right, g.GameRect.bottom, SWP_NOREDRAW);
        }

        Sleep(2);
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
