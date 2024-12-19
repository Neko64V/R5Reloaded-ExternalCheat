#include "CEntity.h"

bool CEntity::Update()
{
	m_localOrigin	  = m.Read<Vector3>(entity + offset::m_localOrigin);
	m_iHealth		  = m.Read<int>(entity + offset::m_iHealth);

	if (Vec3_Empty(m_localOrigin) || m_iHealth <= 0)
		return false;

	m_vecAbsVelocity  = m.Read<Vector3>(entity + offset::m_vecAbsVelocity);
	m_shieldHealth	  = m.Read<int>(entity + offset::m_shieldHealth);
	m_shieldHealthMax = m.Read<int>(entity + offset::m_shieldHealthMax);
	m_iTeamNum		  = m.Read<int>(entity + offset::m_iTeamNum);
	m_iMaxHealth	  = m.Read<int>(entity + offset::m_iMaxHealth);
	m_lastvisibletime = m.Read<float>(entity + offset::m_lastvisibletime);
	camera_origin	  = m.Read<Vector3>(entity + offset::camera_origin);

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

int CEntity::GetLifeState()
{
	return m.Read<int>(entity + offset::m_lifeState);
}

Vector3 CEntity::GetViewAngle()
{
	return m.Read<Vector3>(entity + offset::m_ViewAngle);
}

Vector3 CEntity::GetSwayAngle()
{
	return m.Read<Vector3>(entity + offset::m_SwayAngle);
}

Vector3 CEntity::GetPunchAngle()
{
	return m.Read<Vector3>(entity + offset::m_vecPunchAngle);
}

Vector3 CEntity::GetWeaponPunchAngle()
{
	return m.Read<Vector3>(entity + offset::m_vecPunchWeapon_Angle);
}

float CEntity::GetTimeBase()
{
	return m.Read<float>(entity + 0x1D18);
}

void CEntity::EnableGlow(GlowColor color, GlowMode mode)
{
	m.Write<int>(entity + 0x310, 1);
	m.Write<int>(entity + 0x320, 2);
	m.Write<GlowMode>(entity + 0x27C, mode);
	m.Write<GlowColor>(entity + 0x1D0, color);
}

void CEntity::DisableGlow()
{
	m.Write<int>(entity + 0x310, 0);
	m.Write<int>(entity + 0x320, 0);
}

std::string CEntity::GetName()
{
	char pName[32]{};
	uintptr_t NameAddress = m.Read<uintptr_t>(entity + 0x3BF8);
	m.ReadString(NameAddress, pName, sizeof(pName));

	return pName;
}

Vector3 CEntity::GetEntityBonePosition(int BoneId)
{
	Vector3 vOut = m_localOrigin;
	uintptr_t pBoneArray = m.Read<uintptr_t>(entity + offset::m_pBoneMatrix);
	Bone EntityBone = m.Read<Bone>(pBoneArray + BoneId * 0x30);

	vOut.x += EntityBone.x;
	vOut.y += EntityBone.y;
	vOut.z += EntityBone.z;

	return vOut;
}