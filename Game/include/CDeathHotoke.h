#pragma once
#include"IGamePad.h"

class IBodyPart;

class CDeathHotoke :
	public IGameObject
{
public:
	CDeathHotoke(IGamePad* ptrPad):m_ptrPad(ptrPad){}

	bool Start()override;
	void Update()override;
	void PostRender()override;

	const CVector3& GetPos()const { return m_pos; }
	void SetPos(const CVector3& pos) { m_pos = pos; }

	const CQuaternion& GetRot()const { return m_rot; }
	const CVector3& GetScale()const { return m_scale; }

	IGamePad* GetPad() { return m_ptrPad; }

private:
	GameObj::CSkinModelRender m_model[4];
	AnimationClip m_animationClip[2];

	SuicideObj::CCollisionObj m_col;

	//TRS
	CVector3 m_pos, m_scale;
	CQuaternion m_rot;

	//パーツ
	IBodyPart* m_parts[4] = {};

	//ゲームパッド
	IGamePad* m_ptrPad = nullptr;

public:
	enum enBodyParts {
		enWing,
	};
};

