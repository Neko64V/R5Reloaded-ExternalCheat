#pragma once
#include "../GameSDK/GameSDK.h"

class CEntity
{
public:
    uintptr_t address;

	// Player
	Vector3 m_vecAbsVelocity;
	Vector3 m_vecAbsOrigin;
	int m_shieldHealth;
	int m_shieldHealthMax;
	int m_iHealth;
	int m_iTeamNum;
	int m_iMaxHealth;
	int m_lifeState;
	float m_lastvisibletime;
	int m_iObserverMode;
	Vector3 camera_origin;
	std::string pName;
	std::string m_iSignifierName;
	uintptr_t pBoneArray;

    // Functions
	bool Update();
	void UpdateStatic();
	bool IsPlayer();
	bool IsSpectator();

	bool IsDead();

	Vector3 vecMin();
	Vector3 vecMax();

	int GetFlag();
	float GetTimeBase();
	Vector3 GetViewAngle();
	Vector3 GetSwayAngle();
	Vector3 GetPunchAngle();
	Vector3 GetWeaponPunchAngle();
	Vector3 GetEntityBonePosition(int BoneId);
	void EnableGlow(GlowColor color, GlowMode mode);
	void DisableGlow();
};