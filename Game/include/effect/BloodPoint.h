#pragma once


class BloodPoint : public IQSGameObject, public ICreateObject
{
public:
	BloodPoint(const CVector3& pos, const CVector3& Normal, float scale);

	void Update()override;

private:
	static constexpr int MAX_NUM = 256;//�\���ł���ő吔
	int m_lifeTime = 8;//���ł܂ł̎���
	CBillboard m_model;
};

