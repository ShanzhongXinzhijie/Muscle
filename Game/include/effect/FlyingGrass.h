#pragma once

//草パーティクル
class FlyingGrass : private SuicideObj::CParticle<GameObj::CInstancingModelRender>, public ICreateObject
{
public:
	FlyingGrass(const CVector3& pos, const CVector3& move, int time = 8);

private:
	static constexpr int MAX_NUM = 1024;//表示できる煙の最大数

	int m_maxLifeTime = 8;//消滅までの時間
};