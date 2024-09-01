#include "Cheat.h"
#include <thread>

Vector3 GetPredict(CPlayer& target , float dis)
{
    Vector3 vOut{};
    float bulletTime = dis / 650.f;
    vOut.x = target.m_vecAbsVelocity.x * bulletTime;
    vOut.y = target.m_vecAbsVelocity.y * bulletTime;
    vOut.z = (150.f * 0.5f * (bulletTime * bulletTime));

    return vOut;
}

bool Cheat::AimBot(CPlayer& target)
{
    /*----| SomeChecks |--------------------------------------------------------------------------------*/
   // 1st
    if (g.AimKey0 == NULL || !IsKeyDown(g.AimKey0) && !IsKeyDown(g.AimKey1) || GetForegroundWindow() != g.GameHwnd)
        return false;

    // 2nd
    switch (g.AimKeyType)
    {
    case 0: // and
        if (g.AimKey1 == NULL && IsKeyDown(g.AimKey0))
            break;
        else if (!IsKeyDown(g.AimKey0) || !IsKeyDown(g.AimKey1))
            return false;
        else if (!IsKeyDown(g.AimKey0))
            return false;
        break;
    case 1: // or
        if (g.AimKey1 == NULL && IsKeyDown(g.AimKey0))
            break;
        else if (IsKeyDown(g.AimKey0) || IsKeyDown(g.AimKey1))
            break;

        break;
    }
    /*--------------------------------------------------------------------------------------------------*/

    CPlayer* pLocal = &local;
    CPlayer* pTarget = &target;

    if (!pLocal->Update())
        return false;
    else if (!pTarget->Update())
        return false;

    Vector2 ScreenMiddle = { (float)g.GameRect.right / 2.f, (float)g.GameRect.bottom / 2.f };
    int AimBone = 8;
    float BaseTime = m.Read<float>(pLocal->entity + 0x1d18);
    uintptr_t ViewRenderer = m.Read<uintptr_t>(m.g_BaseAddress + sdk.ViewRender);
    Matrix ViewMatrix = m.Read<Matrix>(m.Read<uintptr_t>(ViewRenderer + sdk.ViewMatrix));

    // Set
    switch (g.Aim_Bone)
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

    Vector2 g_Screen{};
    if (!WorldToScreen(ViewMatrix, g.GameRect, TargetBone, g_Screen))
        return false;

    if (g.Aim_Mode == 0)
    {
        Vector3 Angle = CalcAngle(pLocal->camera_origin, TargetBone);
        Vector3 ViewAngle = pLocal->m_ViewAngle;
        Vector3 Delta = Angle - ViewAngle;

        if (g.Aim_NoSway)
        {
            Vector3 Breath = pLocal->m_SwayAngle - ViewAngle;

            if (Breath.x != 0.f || Breath.y != 0.f)
                Delta = (Angle - ViewAngle) - Breath;
        }

        NormalizeAngles(Delta);
        Vector3 SmoothedAngle = ViewAngle + (Delta / g.Aim_Smooth);
        NormalizeAngles(SmoothedAngle);

        if (!Vec3_Empty(SmoothedAngle))
            m.Write<Vector3>(pLocal->entity + sdk.m_ViewAngle, SmoothedAngle);
    }
    else
    {
        int cX = (ScreenMiddle.x - g_Screen.x) / g.Aim_Smooth;
        int cY = (ScreenMiddle.y - g_Screen.y) / g.Aim_Smooth;

        mouse_event(MOUSEEVENTF_MOVE, -cX, -cY, 0, 0);
    }
}

void Cheat::UpdateList()
{
    while (g.Run)
    {
        // GetLocal
        uintptr_t lp = m.Read<uintptr_t>(m.g_BaseAddress + sdk.dwLocalPlayer);
        if (!local.GetEntity(lp)) {
            Sleep(10);
            continue;
        }

        std::vector<CPlayer> templist;

        for (int i = 0; i < 15000; i++)
        {
            CPlayer entity{};
            uintptr_t address = m.Read<uintptr_t>(m.g_BaseAddress + sdk.dwEntityList + (i * 0x20));

            if (address == NULL || address == lp)
                continue;  
            else if (!entity.GetEntity(address))
                continue;   
            else if (!entity.Update())
                continue;

            templist.push_back(entity);
        }

        NewEntityList = templist;
        templist.clear();

        Sleep(500);
    }
}