#include "CEntity.h"

bool CEntity::GetEntity(uintptr_t& address)
{
	entity = address;
	return entity == NULL ? false : true;
}

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
	m_localAngle	  = m.Read<Vector3>(entity + offset::m_localAngle);
	m_iMaxHealth	  = m.Read<int>(entity + offset::m_iMaxHealth);
	m_lifeState		  = m.Read<int>(entity + offset::m_lifeState);
	m_lastvisibletime = m.Read<float>(entity + offset::m_lastvisibletime);
	camera_origin	  = m.Read<Vector3>(entity + offset::camera_origin);
	m_SwayAngle		  = m.Read<Vector3>(entity + offset::m_SwayAngle);
	m_ViewAngle		  = m.Read<Vector3>(entity + offset::m_ViewAngle);

	return true;
}

bool CEntity::IsPlayer()
{
	return GetSignifierName() == "player";
}

bool CEntity::IsSpectator()
{
	return m.Read<int>(entity + offset::m_iObserverMode) == 5;
}

float CEntity::GetTimeBase()
{
	return m.Read<float>(entity + 0x1D18);
}

void CEntity::SetGlow(GlowColor color, GlowMode mode)
{
	m.Write<int>(entity + 0x310, 1);
	m.Write<int>(entity + 0x320, 2);
	m.Write<GlowMode>(entity + 0x27C, mode);
	m.Write<GlowColor>(entity + 0x1D0, color);
}

std::string CEntity::GetName()
{
	char pName[32]{};
	uintptr_t NameAddress = m.Read<uintptr_t>(entity + 0x3BF8);
	m.ReadString(NameAddress, pName, sizeof(pName));

	return pName;
}

std::string CEntity::GetSignifierName()
{
	char iName[16]{};
	m.ReadString(entity + 0x521, iName, sizeof(iName));

	return iName;
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