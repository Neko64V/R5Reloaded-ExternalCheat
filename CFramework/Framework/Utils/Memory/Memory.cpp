#include "Memory.h"

bool Memory::AttachProcess(const char* targetName, int InitMode)
{
    // 任意の方法でPIDを取得する
    if (InitMode == WINDOW_TITLE || InitMode == WINDOW_CLASS)
    {

        // ウィンドウベースでPIDを取得
        HWND hWindow = InitMode == WINDOW_TITLE ? FindWindowA(nullptr, targetName) : FindWindowA(targetName, nullptr);

        if (!hWindow) {
            MessageBoxA(nullptr, "please open r5reloaded", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }

        GetWindowThreadProcessId(hWindow, &m_dwPID);
    }
    else if (InitMode == PROCESS)
    {
        // 実行ファイル名ベースでPIDを取得
        PROCESSENTRY32 process = GetProcess(targetName);

        if (process.th32ProcessID == 0) {
            MessageBoxA(nullptr, "please open r5reloaded", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }

        m_dwPID = process.th32ProcessID;
    }
    else 
    {
        // 無効なオプション
        MessageBoxA(nullptr, "Invalid memory init option", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
        return false;
    }

    // プロセスハンドルを取得
    m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPID);

    if (!m_hProcess) {
        MessageBoxA(nullptr, "Failed to get process handle", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK); // 管理者として実行するかアンチチートをバイパスする
        return false;
    }

    return true;
}

void Memory::GetBaseAddress(const char* targetName)
{
    // ベースアドレスを取得
    m_gProcessBaseAddr = GetModuleBase(targetName);

    if (m_gProcessBaseAddr == 0)
        MessageBoxA(nullptr, "BaseAddress == 0", "WARNING", MB_OK | MB_TOPMOST);
}

void Memory::DetachProcess()
{
    CloseHandle(m_hProcess);
}

uintptr_t Memory::GetModuleBase(const std::string moduleName)
{
    MODULEENTRY32 entry{};
    entry.dwSize = sizeof(MODULEENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwPID);

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
