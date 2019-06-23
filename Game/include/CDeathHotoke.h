#pragma once

class CDeathHotoke :
	public IGameObject
{
public:
	CDeathHotoke();
	~CDeathHotoke();

	bool Start()override;
	void Update()override;

	const CVector3& GetPos()const { return m_pos; }
	void SetPos(const CVector3& pos) { m_pos = pos; }

private:
	GameObj::CSkinModelRender m_model[5];
	AnimationClip m_animationClip[2];

	SuicideObj::CCollisionObj m_col;

	CVector3 m_pos;
};

