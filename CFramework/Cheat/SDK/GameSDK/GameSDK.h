#pragma once
#include "../../../Framework/Utils/Utils.h"

namespace offset
{
	// Main
	const uintptr_t dwEntityList = 0x1F33F78;
	const uintptr_t dwLocalPlayer = 0x22E93F8;
	const uintptr_t ViewRender = 0xD4138F0;
	const uintptr_t ViewMatrix = 0x1A93D0;
	const uintptr_t TimeScale = 0x1843340;

	// CBaseEntity
	const uintptr_t m_vecAbsVelocity	= 0x140;
	const uintptr_t m_localOrigin		= 0x14C;
	const uintptr_t m_shieldHealth		= 0x170;
	const uintptr_t m_shieldHealthMax	= 0x174;
	const uintptr_t m_iHealth			= 0x3E0;
	const uintptr_t m_iTeamNum			= 0x3F0;
	const uintptr_t m_localAngle		= 0x428;
	const uintptr_t m_iMaxHealth		= 0x510;
	const uintptr_t m_lifeState			= 0x730;
	const uintptr_t camera_origin		= 0x1b68;
	const uintptr_t m_lastvisibletime	= 0x1754;
	const uintptr_t m_vecPunchAngle		= 0x20bc;
	const uintptr_t m_SwayAngle			= 0x2178;
	const uintptr_t m_ViewAngle			= 0x2188;
	const uintptr_t m_platformUserId	= 0x2220;
	const uintptr_t m_iObserverMode		= 0x2fcc;
	const uintptr_t m_hObserverTarget	= 0x2fd0;

	const uintptr_t m_nSkin				= 0xE48;
	const uintptr_t m_pBoneMatrix		= 0xEE0;
};

struct Bone
{
	char pad_01[0xCC];
	float x;
	char pad_02[0xC];
	float y;
	char pad_03[0xC];
	float z;
};

struct GlowMode
{
	int8_t GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel;
};

struct GlowColor
{
	float R, G, B;
};

extern struct Bone;
extern struct GlowMode;
extern struct GlowColor;
extern bool Vec3_Empty(const Vector3& value);
extern void NormalizeAngles(Vector3& angle);
extern Vector3 CalcAngle(const Vector3& src, const Vector3& dst);
extern bool WorldToScreen(Matrix ViewMatrix, RECT Size, Vector3 vIn, Vector2& vOut);