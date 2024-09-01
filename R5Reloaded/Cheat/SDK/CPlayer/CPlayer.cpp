#include "CPlayer.h"

bool CPlayer::GetEntity(uintptr_t& address)
{
	entity = address;
	return entity == NULL ? false : true;
}

bool CPlayer::Update()
{
	m_localOrigin	  = m.Read<Vector3>(entity + sdk.m_localOrigin);
	m_iHealth		  = m.Read<int>(entity + sdk.m_iHealth);

	if (Vec3_Empty(m_localOrigin) || m_iHealth <= 0)
		return false;

	m_vecAbsVelocity  = m.Read<Vector3>(entity + sdk.m_vecAbsVelocity);
	m_shieldHealth	  = m.Read<int>(entity + sdk.m_shieldHealth);
	m_shieldHealthMax = m.Read<int>(entity + sdk.m_shieldHealthMax);
	m_iTeamNum		  = m.Read<int>(entity + sdk.m_iTeamNum);
	m_localAngle	  = m.Read<Vector3>(entity + sdk.m_localAngle);
	m_iMaxHealth	  = m.Read<int>(entity + sdk.m_iMaxHealth);
	m_lifeState		  = m.Read<int>(entity + sdk.m_lifeState);
	m_lastvisibletime = m.Read<float>(entity + sdk.m_lastvisibletime);
	camera_origin	  = m.Read<Vector3>(entity + sdk.camera_origin);
	m_SwayAngle		  = m.Read<Vector3>(entity + sdk.m_SwayAngle);
	m_ViewAngle		  = m.Read<Vector3>(entity + sdk.m_ViewAngle);

	return true;
}

bool CPlayer::IsPlayer()
{
	return GetSignifierName() == "player";
}

bool CPlayer::IsSpectator()
{
	return m.Read<int>(entity + sdk.m_iObserverMode) == 5;
}

void CPlayer::SetGlow(GlowColor color, GlowMode mode)
{
	m.Write<int>(entity + 0x310, 1);
	m.Write<int>(entity + 0x320, 2);
	m.Write<GlowMode>(entity + 0x27C, mode);
	m.Write<GlowColor>(entity + 0x1D0, color);
}

uintptr_t CPlayer::GetWeapon()
{
	uint32_t weaponid = m.Read<uint32_t>(entity + 0x1704);

	return weaponid == -1 ? 0 : m.Read<uintptr_t>(m.g_BaseAddress + sdk.dwEntityList + (static_cast<uint16_t>(weaponid) * 0x20));
}

std::string CPlayer::GetName()
{
	char pName[32]{};
	uintptr_t NameAddress = m.Read<uintptr_t>(entity + 0x3BF8);
	m.ReadString(NameAddress, pName, sizeof(pName));

	return pName;
}

std::string CPlayer::GetSignifierName()
{
	char iName[32]{};
	m.ReadString(entity + 0x521, iName, sizeof(iName));

	return iName;
}

Vector3 CPlayer::GetEntityBonePosition(int BoneId)
{
	Vector3 vOut = m_localOrigin;
	uintptr_t pBoneArray = m.Read<uintptr_t>(entity + sdk.m_pBoneMatrix);
	Bone EntityBone = m.Read<Bone>(pBoneArray + BoneId * 0x30);

	vOut.x += EntityBone.x;
	vOut.y += EntityBone.y;
	vOut.z += EntityBone.z;

	return vOut;
}