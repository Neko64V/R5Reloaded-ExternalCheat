#include "FrameCore.h"
#pragma comment(lib, "WinMM.lib")

const int ReadCount = 15000;
Vector3 GetPredict(CEntity& target, float dist);

struct Entity
{
    uint64_t address;
    uint64_t junk[3];
};

struct CEntityListBase
{
    Entity entity[ReadCount]{};
};

/*
    [ 概要 ]
    ESPで表示したいEntityのみをここで抽出することでESPのパフォーマンスを劇的に改善できる。
    レンダリングを行うスレッドで数千回以上のReadProcessMemoryからのcontinue;をするだけでかなりのロスなのでそれが回避できる功績は大きい。
*/
void CFramework::UpdateList() // C6262 :(
{
    while (g.g_Run)
    {
        std::vector<CEntity> ent_list;
        std::vector<std::string> spec_list;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        // Local
        pLocal->address = m.Read<uintptr_t>(m.m_gProcessBaseAddr + offset::dwLocalPlayer);

        if (pLocal->address == NULL)
            continue;

        // EntityList
        auto list_addr = m.Read<uintptr_t>(m.m_gProcessBaseAddr + offset::dwEntityList);

        if (list_addr == NULL)
            continue;

        // GetEntitys(20000)
        auto list = m.Read<CEntityListBase>(m.m_gProcessBaseAddr + offset::dwEntityList);

        for (int i = 0; i < ReadCount; i++)
        {
            // Pointer Check
            if (list.entity[i].address != NULL && list.entity[i].address != local.address)
            {
                // Player/Item/Bot Check
                char SignifierName[32]{};
                const uintptr_t sig_name_addr = m.Read<uintptr_t>(list.entity[i].address + offset::m_iSignifierName);

                if (sig_name_addr != NULL)
                {
                    m.ReadString(sig_name_addr, SignifierName, sizeof(SignifierName));

                    if (strcmp(SignifierName, "player") == 0 || g.g_ESP_NPC && strcmp(SignifierName, "npc_dummie") == 0)
                    {
                        // Player/Dummy
                        CEntity p = CEntity();
                        p.address = list.entity[i].address;
                        p.m_iSignifierName = SignifierName;
                        p.UpdateStatic();

                        // SpectatorCheck
                        if (strcmp(SignifierName, "player") == 0 && m.Read<int>(list.entity[i].address + offset::m_iObserverMode) == 5) {
                            spec_list.push_back(p.pName);
                        }
                        else if (!p.IsDead()) {
                            ent_list.push_back(p);
                        }
                    }
                }
            }
        }

        EntityList = ent_list;
        SpectatorPlayerName = spec_list;
        ent_list.clear();
        spec_list.clear();
    }
}

void CFramework::MiscAll()
{
    if (!pLocal->Update())
        return;

    // NoRecoil
    if (g.g_NoRecoil)
    {
        static Vector3 OldPunch{};

        Vector3 PunchAngle = pLocal->GetWeaponPunchAngle();

        if (!Vec3_Empty(PunchAngle)) {
            Vector3 Delta = pLocal->GetViewAngle() + ((OldPunch - PunchAngle) * g.g_NoRecoilVal);
            NormalizeAngles(Delta);

            if (!Vec3_Empty(Delta))
                m.Write<Vector3>(pLocal->address + offset::m_ViewAngle, Delta);

            OldPunch = PunchAngle;
        }
    }

    // bHop
    if (g.g_BunnyHop)
    {
        if (GetAsyncKeyState(VK_SPACE))
        {
            int flag = pLocal->GetFlag();

            if (flag != 64) {
                m.Write<uint32_t>(m.m_gProcessBaseAddr + offset::in_jump + 0x8, 5);
                timeBeginPeriod(1); // タイマー解像度を1msに設定
                Sleep(1);          // 10msスリープ
                timeEndPeriod(1);   // タイマー解像度を元に戻す
                m.Write<uint32_t>(m.m_gProcessBaseAddr + offset::in_jump + 0x8, 4);
            }
        }
    }
}

bool CFramework::AimBot(CEntity& target)
{
    /*----| SomeChecks |--------------------------------------------------------------------------------*/
   // 1st
    if (g.g_AimKey0 == NULL || !IsKeyDown(g.g_AimKey0) && !IsKeyDown(g.g_AimKey1) || g.g_ShowMenu)
        return false;

    // 2nd
    switch (g.g_AimKeyType)
    {
    case 0: // and
        if (g.g_AimKey1 == NULL && IsKeyDown(g.g_AimKey0))
            break;
        else if (!IsKeyDown(g.g_AimKey0) || !IsKeyDown(g.g_AimKey1))
            return false;
        else if (!IsKeyDown(g.g_AimKey0))
            return false;
        break;
    case 1: // or
        if (g.g_AimKey1 == NULL && IsKeyDown(g.g_AimKey0))
            break;
        else if (IsKeyDown(g.g_AimKey0) || IsKeyDown(g.g_AimKey1))
            break;

        break;
    }
    /*--------------------------------------------------------------------------------------------------*/

    CEntity* pLocal = &local;
    CEntity* pTarget = &target;

    if (!pLocal->Update())
        return false;
    else if (!pTarget->Update())
        return false;

    Vector2 ScreenMiddle = { (float)g.g_GameRect.right / 2.f, (float)g.g_GameRect.bottom / 2.f };
    int AimBone = 8;
    uintptr_t ViewRenderer = m.Read<uintptr_t>(m.m_gProcessBaseAddr + offset::ViewRender);
    Matrix ViewMatrix = m.Read<Matrix>(m.Read<uintptr_t>(ViewRenderer + offset::ViewMatrix));

    // Set
    switch (g.g_Aim_Bone)
    {
    case 0:
        AimBone = 8;
        break;
    case 1:
        AimBone = 3;
        break;
    }
    float distance = ((pTarget->m_vecAbsOrigin - pLocal->m_vecAbsOrigin).Length() * 0.01905f);
    Vector3 TargetBone = pTarget->GetEntityBonePosition(AimBone); // Head / Neck

    if (Vec3_Empty(TargetBone))
        return false;

    TargetBone += GetPredict(target, distance);

    Vector2 ScreenPos{};
    if (!WorldToScreen(ViewMatrix, g.g_GameRect, TargetBone, ScreenPos))
        return false;

    if (g.g_Aim_Mode == 0)
    {
        Vector3 Angle = CalcAngle(pLocal->camera_origin, TargetBone);
        Vector3 ViewAngle = pLocal->GetViewAngle();
        Vector3 Delta = Angle - ViewAngle;

        if (g.g_Aim_NoSway)
        {
            Vector3 Breath = pLocal->GetSwayAngle() - ViewAngle;

            if (Breath.x != 0.f || Breath.y != 0.f)
                Delta = (Angle - ViewAngle) - Breath;
        }

        NormalizeAngles(Delta);
        Vector3 SmoothedAngle = ViewAngle + (Delta / g.g_Aim_Smooth);
        NormalizeAngles(SmoothedAngle);

        if (!Vec3_Empty(SmoothedAngle))
            m.Write<Vector3>(pLocal->address + offset::m_ViewAngle, SmoothedAngle);
    }
    else
    {
        int cX = (ScreenMiddle.x - ScreenPos.x) / g.g_Aim_Smooth;
        int cY = (ScreenMiddle.y - ScreenPos.y) / g.g_Aim_Smooth;

        mouse_event(MOUSEEVENTF_MOVE, -cX, -cY, 0, 0);
    }
}

// :(
ImColor CFramework::SetESPColor(bool& is_visible, bool is_team)
{
    return is_team ? ESP_Team : (is_visible ? ESP_Visible : ESP_Default);
}

Vector3 GetPredict(CEntity& target, float dist)
{
    Vector3 vOut{};
    float bulletTime = dist / 650.f;
    vOut.x = target.m_vecAbsVelocity.x * bulletTime;
    vOut.y = target.m_vecAbsVelocity.y * bulletTime;
    vOut.z = (150.f * 0.5f * (bulletTime * bulletTime));

    return vOut;
}