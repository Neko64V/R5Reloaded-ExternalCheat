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
	int m_iMaxHealth;
	std::string m_iSignifierName;
	int m_lifeState;
	float m_lastvisibletime;
	int m_iObserverMode;
	Vector3 camera_origin;

    // Functions
	bool Update();
	bool IsPlayer();
	bool IsSpectator();

	int GetLifeState();
	Vector3 GetViewAngle();
	Vector3 GetSwayAngle();
	Vector3 GetPunchAngle();
	Vector3 GetWeaponPunchAngle();
	float GetTimeBase();
	void EnableGlow(GlowColor color, GlowMode mode);
	void DisableGlow();

    std::string GetName();
	Vector3 GetEntityBonePosition(int BoneId);
};