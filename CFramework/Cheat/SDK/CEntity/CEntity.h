#pragma once
#include "../GameSDK/GameSDK.h"

class CEntity
{
public:
    uintptr_t entity;

	// Player
	Vector3 m_vecAbsVelocity;
	Vector3 m_localOrigin;
	int m_shieldHealth;
	int m_shieldHealthMax;
	int m_iHealth;
	int m_iTeamNum;
	Vector3 m_localAngle;
	int m_iMaxHealth;
	int m_lifeState;
	float m_lastvisibletime;
	float m_lastcrosshairtime;
	Vector3 camera_origin;
	Vector3 m_SwayAngle;
	Vector3 m_ViewAngle;
	uint64_t m_platformUserId;

    // Functions
    bool GetEntity(uintptr_t& address);
	bool Update();
	bool IsPlayer();
	bool IsSpectator();

	float GetTimeBase();
	void SetGlow(GlowColor color, GlowMode mode);

    std::string GetName();
	std::string GetSignifierName();
	Vector3 GetEntityBonePosition(int BoneId);
};