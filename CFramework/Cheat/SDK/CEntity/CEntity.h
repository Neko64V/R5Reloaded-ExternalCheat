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
	std::string m_iSignifierName;
	int m_lifeState;
	float m_lastvisibletime;
	float m_lastcrosshairtime;

	int m_iObserverMode;

	Vector3 camera_origin;
	Vector3 m_SwayAngle;
	Vector3 m_ViewAngle;
	uint64_t m_platformUserId;

    // Functions
	bool Update();
	bool IsPlayer();
	bool IsSpectator();

	float GetTimeBase();
	void EnableGlow(GlowColor color, GlowMode mode);
	void DisableGlow();

    std::string GetName();
	Vector3 GetEntityBonePosition(int BoneId);
};