#pragma once

//���p�[�e�B�N��
class FlyingGrass : private SuicideObj::CParticle<GameObj::CInstancingModelRender>, public ICreateObject
{
public:
	FlyingGrass(const CVector3& pos, const CVector3& move, int time = 8);

private:
	static constexpr int MAX_NUM = 1024;//�\���ł��鉌�̍ő吔

	int m_maxLifeTime = 8;//���ł܂ł̎���
};