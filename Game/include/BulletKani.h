#pragma once
class BulletKani :
	public IGameObject
{
public:
	BulletKani(const CVector3& pos, const CVector3& move) : m_pos(pos), m_posOld(pos), m_vector(move){}

	bool Start()override;
	void Update()override;
	void PostLoopUpdate()override;

private:
	//GameObj::CInstancingModelRender m_model;

	float m_lifeTime = 30.0f;
	CVector3 m_pos, m_posOld, m_vector;
};

