#pragma once


class BloodPoint : public IQSGameObject, public ICreateObject
{
public:
	BloodPoint(const CVector3& pos, const CVector3& Normal, float scale);

	void Update()override;

private:
	static constexpr int MAX_NUM = 256;//•\¦‚Å‚«‚éÅ‘å”
	int m_lifeTime = 8;//Á–Å‚Ü‚Å‚ÌŠÔ
	CBillboard m_model;
};

