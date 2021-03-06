#pragma once


class BloodPoint : public IQSGameObject, public ICreateObject
{
public:
	BloodPoint(const CVector3& pos, const CVector3& Normal, float scale);

	void Update()override;

private:
	static constexpr int MAX_NUM = 256;//表示できる最大数
	int m_lifeTime = 8;//消滅までの時間
	CBillboard m_model;
};

