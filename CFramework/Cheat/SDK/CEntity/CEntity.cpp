#include "CEntity.h"

bool CEntity::Update()
{
	m_vecAbsOrigin	  = m.Read<Vector3>(m_address + offset::m_localOrigin);
	m_iHealth		  = m.Read<int>(m_address + offset::m_iHealth);
	
	if (IsDead() || Vec3_Empty(m_vecAbsOrigin) || m_iHealth <= 0)
		return false;
		
	pBoneArray		  = m.Read<uintptr_t>(m_address + offset::m_pBoneMatrix);
	m_vecAbsVelocity  = m.Read<Vector3>(m_address + offset::m_vecAbsVelocity);
	m_shieldHealth	  = m.Read<int>(m_address + offset::m_shieldHealth);
	m_lastvisibletime = m.Read<float>(m_address + offset::m_lastvisibletime);
	camera_origin	  = m.Read<Vector3>(m_address + offset::camera_origin);

	return true;
}

void CEntity::UpdateStatic()
{
	m_iTeamNum = m.Read<int>(m_address + offset::m_iTeamNum);
	m_iMaxHealth = m.Read<int>(m_address + offset::m_iMaxHealth);
	m_shieldHealthMax = m.Read<int>(m_address + offset::m_shieldHealthMax);

	// Name
	char name[32]{};
	uintptr_t NameAddress = m.Read<uintptr_t>(m_address + offset::m_szName);
	m.ReadString(NameAddress, name, sizeof(name));
	pName = name;
}

bool CEntity::IsPlayer()
{
	return m_iSignifierName == "player";
}

bool CEntity::IsSpectator()
{
	return m.Read<int>(m_address + offset::m_iObserverMode) == 5;
}

Vector3 CEntity::vecMin()
{
	return m.Read<Vector3>(m_address + offset::m_Collision + 0x10) + m_vecAbsOrigin;
}

Vector3 CEntity::vecMax()
{
	return m.Read<Vector3>(m_address + offset::m_Collision + 0x1C) + m_vecAbsOrigin;
}

int CEntity::GetFlag()
{
	return m.Read<int>(m_address + offset::m_fFlags);
}

bool CEntity::IsDead()
{
	return m.Read<int>(m_address + offset::m_lifeState) > 0;
}

Vector3 CEntity::GetViewAngle()
{
	return m.Read<Vector3>(m_address + offset::m_vecViewAngle);
}

Vector3 CEntity::GetSwayAngle()
{
	return m.Read<Vector3>(m_address + offset::m_vecSwayAngle);
}

Vector3 CEntity::GetPunchAngle()
{
	return m.Read<Vector3>(m_address + offset::m_vecPunchAngle);
}

Vector3 CEntity::GetWeaponPunchAngle()
{
	return m.Read<Vector3>(m_address + offset::m_vecPunchWeapon_Angle);
}

float CEntity::GetTimeBase()
{
	return m.Read<float>(m_address + 0x1D18);
}

void CEntity::EnableGlow(GlowColor color, GlowMode mode)
{
	m.Write<int>(m_address + 0x310, 1);
	m.Write<int>(m_address + 0x320, 2);
	m.Write<GlowMode>(m_address + 0x27C, mode);
	m.Write<GlowColor>(m_address + 0x1D0, color);
}

void CEntity::DisableGlow()
{
	m.Write<int>(m_address + 0x310, 0);
	m.Write<int>(m_address + 0x320, 0);
}

Vector3 CEntity::GetEntityBonePosition(int BoneId)
{
	Vector3 vOut = m_vecAbsOrigin;
	Bone EntityBone = m.Read<Bone>(pBoneArray + BoneId * 0x30);

	vOut.x += EntityBone.x;
	vOut.y += EntityBone.y;
	vOut.z += EntityBone.z;

	return vOut;
}