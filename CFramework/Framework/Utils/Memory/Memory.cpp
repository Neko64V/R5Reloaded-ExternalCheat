#include "Memory.h"

bool Memory::AttachProcess(const char* targetName, int mode)
{
    switch (mode)
    {
    case InitMode::WINDOW_TITLE: {
        HWND TargetHwnd = FindWindowA(NULL, targetName);

        if (!TargetHwnd) {
            MessageBoxA(nullptr, "Please open { GAME NAME #1 }", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }

        GetWindowThreadProcessId(TargetHwnd, &m_PID);
        break;
    }
    case InitMode::WINDOW_CLASS: {
        HWND TargetHwnd = FindWindowA(targetName, NULL);

        if (!TargetHwnd) {
            MessageBoxA(nullptr, "Please open { GAME NAME #2 }", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }

        GetWindowThreadProcessId(TargetHwnd, &m_PID);
        break;
    }
    case InitMode::PROCESS: {
        PROCESSENTRY32 process = GetProcess(targetName);

        if (process.th32ProcessID == 0) {
            MessageBoxA(nullptr, "Please open { GAME NAME #3 }", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }

        m_PID = process.th32ProcessID;
        break;
    }
    default:
        MessageBoxA(nullptr, "Invalid memory init option", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
        return false;
    }

    // プロセスハンドルを取得
    m_pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_PID);

    if (!m_pHandle) {
        MessageBoxA(nullptr, "Failed to get process handle", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK); // 管理者として実行するかアンチチートをバイパスする
        return false;
    }

    // ベースアドレスを取得
    m_gBaseAddress = GetModuleBase("r5apex.exe");

    if (m_gBaseAddress == 0)
        MessageBoxA(nullptr, "BaseAddress is 0", "WARNING", MB_OK | MB_TOPMOST);

    return true;
}

void Memory::DetachProcess()
{
    CloseHandle(m_pHandle);
}

uintptr_t Memory::GetModuleBase(const std::string moduleName)
{
    MODULEENTRY32 entry{};
    entry.dwSize = sizeof(MODULEENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_PID);

    while (Module32Next(snapShot, &entry))
    {
        if (!moduleName.compare(entry.szModule))
        {
            CloseHandle(snapShot);
            return reinterpret_cast<uintptr_t>(entry.modBaseAddr);
        }
    }

    if (snapShot)
        CloseHandle(snapShot);

    return reinterpret_cast<uintptr_t>(entry.modBaseAddr);
}

PROCESSENTRY32 Memory::GetProcess(const std::string processName)
{
    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(PROCESSENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    while (Process32Next(snapShot, &entry))
    {
        if (!processName.compare(entry.szExeFile))
        {
            CloseHandle(snapShot);
            return entry;
        }
    }

    CloseHandle(snapShot);

    return PROCESSENTRY32();
}

Memory m;
