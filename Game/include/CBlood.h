#pragma once
#include "IFu.h"
#include "BeamModel.h"

class CBlood :
	public IFu, public IGameObject
{
public:
	CBlood(const CVector3& pos, const CVector3& move, float radius = -1.0f, bool createCollision = true)
		: m_posOld(pos), m_vector(move), m_radiusSetting(radius), m_createCollision(createCollision)
	{
		SetPos(pos);
	}

	bool Start()override;
	void Update()override;

private:
	//モデル
	BeamModel m_model;
	//寿命
	float m_lifeTime = 0.5f;
	//座標とか
	CVector3 m_posOld, m_vector;
	//長さ
	float m_lengthScale = 1.0f;
	//半径
	float m_radiusSetting = -1.0f;
	//落下速度
	float m_gravityScale = 1.0f;
	//コリジョンを作成するか?
	bool m_createCollision = true;
};

