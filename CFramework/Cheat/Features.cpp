#include "FrameCore.h"

const int ReadCount = 15000;

struct CEntityListBase
{
    uintptr_t address[ReadCount]{};
};

Vector3 GetPredict(CEntity& target , float dist)
{
    Vector3 vOut{};
    float bulletTime = dist / 650.f;
    vOut.x = target.m_vecAbsVelocity.x * bulletTime;
    vOut.y = target.m_vecAbsVelocity.y * bulletTime;
    vOut.z = (150.f * 0.5f * (bulletTime * bulletTime));

    return vOut;
}

void CFramework::MiscAll()
{
    // NoRecoil���͂��߂Ƃ����C�ӂ̋@�\��ǉ����Ă�
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
    uintptr_t ViewRenderer = m.Read<uintptr_t>(m.m_gBaseAddress + offset::ViewRender);
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
    float distance = ((pTarget->m_localOrigin - pLocal->m_localOrigin).Length() * 0.01905f);
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
        Vector3 ViewAngle = pLocal->m_ViewAngle;
        Vector3 Delta = Angle - ViewAngle;

        if (g.g_Aim_NoSway)
        {
            Vector3 Breath = pLocal->m_SwayAngle - ViewAngle;

            if (Breath.x != 0.f || Breath.y != 0.f)
                Delta = (Angle - ViewAngle) - Breath;
        }

        NormalizeAngles(Delta);
        Vector3 SmoothedAngle = ViewAngle + (Delta / g.g_Aim_Smooth);
        NormalizeAngles(SmoothedAngle);

        if (!Vec3_Empty(SmoothedAngle))
            m.Write<Vector3>(pLocal->entity + offset::m_ViewAngle, SmoothedAngle);
    }
    else
    {
        int cX = (ScreenMiddle.x - ScreenPos.x) / g.g_Aim_Smooth;
        int cY = (ScreenMiddle.y - ScreenPos.y) / g.g_Aim_Smooth;

        mouse_event(MOUSEEVENTF_MOVE, -cX, -cY, 0, 0);
    }
}

void CFramework::UpdateList()
{
    while (g.g_Run)
    {
        std::vector<CEntity> ent_list;
        std::vector<std::string> spec_list;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        // Local
        local.entity = m.Read<uintptr_t>(m.m_gBaseAddress + offset::dwLocalPlayer);

        if (local.entity == NULL)
            continue;

        // EntityList
        auto list_addr = m.Read<uintptr_t>(m.m_gBaseAddress + offset::dwEntityList);

        if (list_addr == NULL)
            continue;

        // GetEntitys(20000)
        auto list = m.Read< CEntityListBase>(m.m_gBaseAddress + offset::dwEntityList);

        for (int i = 0; i < ReadCount; i++)
        {
            // Pointer Check
            if (list.address[i] != NULL && list.address[i] != local.entity)
            {
                // Player/Item/Bot Check
                char SignifierName[32]{};
                const uintptr_t sig_name_addr = m.Read<uintptr_t>(list.address[i] + offset::m_iSignifierName);
                
                if (sig_name_addr != NULL)
                {
                    m.ReadString(sig_name_addr, SignifierName, sizeof(SignifierName));

                    if (strcmp(SignifierName, "player") == 0 || g.g_ESP_NPC && strcmp(SignifierName, "npc_dummie") == 0)
                    {
                        // Player/Dummy
                        CEntity p = CEntity();
                        p.entity = list.address[i];
                        p.m_iSignifierName = SignifierName;
                        
                        // SpectatorCheck
                        if (strcmp(SignifierName, "player") == 0 && m.Read<int>(list.address[i] + offset::m_iObserverMode) == 5)
                            spec_list.push_back(p.GetName());
                        else 
                            ent_list.push_back(p);
                    }

                    // Work in progress...
                    /*
                    else if (strcmp(iName, "viewmodel") == 0)
                        temp_list.push_back(list.address[i]);
                    else if (strcmp(iName, "prop_survival") == 0)
                        temp_list.push_back(list.address[i]);
                    */
                    /* - Note
                       player
                       npc_dummie
                       weaponx
                       viewmodel
                       prop_survival
                       prop_dynamic
                   */
                }                
            }
        }

        EntityList = ent_list;
        SpectatorPlayerName = spec_list;
        ent_list.clear();
        spec_list.clear();
    }
}

// :(
ImColor CFramework::SetESPColor(bool& is_visible, bool is_team)
{
    if (is_team)
        return ESP_Team;
    
    return is_visible ? ESP_Visible : ESP_Default;
}