#include "CEntity.h"

bool CEntity::Update()
{
	m_vecAbsOrigin	  = m.Read<Vector3>(address + offset::m_localOrigin);
	m_iHealth		  = m.Read<int>(address + offset::m_iHealth);

	if (Vec3_Empty(m_vecAbsOrigin) || m_iHealth <= 0)
		return false;

	m_vecAbsVelocity  = m.Read<Vector3>(address + offset::m_vecAbsVelocity);
	m_shieldHealth	  = m.Read<int>(address + offset::m_shieldHealth);
	m_shieldHealthMax = m.Read<int>(address + offset::m_shieldHealthMax);
	m_iTeamNum		  = m.Read<int>(address + offset::m_iTeamNum);
	m_iMaxHealth	  = m.Read<int>(address + offset::m_iMaxHealth);
	m_lastvisibletime = m.Read<float>(address + offset::m_lastvisibletime);
	camera_origin	  = m.Read<Vector3>(address + offset::camera_origin);

	return true;
}

bool CEntity::IsPlayer()
{
	return m_iSignifierName == "player";
}

bool CEntity::IsSpectator()
{
	return m_iObserverMode == 5;
}

Vector3 CEntity::vecMin()
{
	return m.Read<Vector3>(address + offset::m_Collision + 0x10) + m_vecAbsOrigin;
}

Vector3 CEntity::vecMax()
{
	return m.Read<Vector3>(address + offset::m_Collision + 0x1C) + m_vecAbsOrigin;
}

int CEntity::GetLifeState()
{
	return m.Read<int>(address + offset::m_lifeState);
}

Vector3 CEntity::GetViewAngle()
{
	return m.Read<Vector3>(address + offset::m_ViewAngle);
}

Vector3 CEntity::GetSwayAngle()
{
	return m.Read<Vector3>(address + offset::m_SwayAngle);
}

Vector3 CEntity::GetPunchAngle()
{
	return m.Read<Vector3>(address + offset::m_vecPunchAngle);
}

Vector3 CEntity::GetWeaponPunchAngle()
{
	return m.Read<Vector3>(address + offset::m_vecPunchWeapon_Angle);
}

float CEntity::GetTimeBase()
{
	return m.Read<float>(address + 0x1D18);
}

void CEntity::EnableGlow(GlowColor color, GlowMode mode)
{
	m.Write<int>(address + 0x310, 1);
	m.Write<int>(address + 0x320, 2);
	m.Write<GlowMode>(address + 0x27C, mode);
	m.Write<GlowColor>(address + 0x1D0, color);
}

void CEntity::DisableGlow()
{
	m.Write<int>(address + 0x310, 0);
	m.Write<int>(address + 0x320, 0);
}

std::string CEntity::GetName()
{
	char pName[32]{};
	uintptr_t NameAddress = m.Read<uintptr_t>(address + 0x3BF8);
	m.ReadString(NameAddress, pName, sizeof(pName));

	return pName;
}

Vector3 CEntity::GetEntityBonePosition(int BoneId)
{
	Vector3 vOut = m_vecAbsOrigin;
	uintptr_t pBoneArray = m.Read<uintptr_t>(address + offset::m_pBoneMatrix);
	Bone EntityBone = m.Read<Bone>(pBoneArray + BoneId * 0x30);

	vOut.x += EntityBone.x;
	vOut.y += EntityBone.y;
	vOut.z += EntityBone.z;

	return vOut;
}