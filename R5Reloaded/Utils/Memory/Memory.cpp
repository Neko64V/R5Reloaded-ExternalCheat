#include "Memory.h"
#include "../Config/Config.h"

bool Memory::AttachProcess(const std::string targetName, int mode)
{
    switch (mode)
    {
    case MODE::WINDOW: {
        g.GameHwnd = FindWindowA(nullptr, targetName.c_str());

        if (!g.GameHwnd) {
            MessageBoxA(nullptr, "Please open { GAME NAME #1 }", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }

        GetWindowThreadProcessId(g.GameHwnd, &PID);
        pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

        if (!pHandle) {
            MessageBoxA(nullptr, "Failed to get process handle", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK); // 管理者として実行するかアンチチートをバイパスする
            return false;
        }

        g_BaseAddress = GetModuleBase("r5apex.exe");

        return true;
    }
    case MODE::PROCESS: {
        PROCESSENTRY32 process = GetProcess(targetName);

        if (process.th32ProcessID == 0) {
            MessageBoxA(nullptr, "Please open { GAME NAME #2 }", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }

        pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process.th32ProcessID);

        if (!pHandle) {
            MessageBoxA(nullptr, "Failed to get process handle", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK); // 管理者として実行するかアンチチートをバイパスする
            return false;
        }

        g_BaseAddress = GetModuleBase("r5apex.exe");

        return true;
    }
    default:
        MessageBoxA(nullptr, "[DEV] Invalid memory init option", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
        return false;
    }
}

void Memory::DetachProcess()
{
    CloseHandle(pHandle);
}

uintptr_t Memory::GetModuleBase(const std::string moduleName)
{
    MODULEENTRY32 entry{};
    entry.dwSize = sizeof(MODULEENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

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