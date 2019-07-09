#pragma once
class ShadowMapBaker
{
public:
	ShadowMapBaker() = default;
	
	/// <summary>
	/// �`�悷�郂�f���ǉ�
	/// </summary>
	/// <param name="caster">�`�悷�郂�f��</param>
	/// <param name="isCasterSetUnable">�ݒ肵�����f���̃V���h�E�L���X�^�[������I�t�ɂ��邩</param>
	void AddDrawModel(GameObj::CSkinModelRender& caster, bool isCasterSetUnable = true);
	
	/// <summary>
	/// �V���h�E�}�b�v�̃x�C�N
	/// </summary>
	/// <param name="x_size">���𑜓x</param>
	/// <param name="y_size">�c�𑜓x</param>
	/// <param name="lightDir">���C�g����</param>
	/// <param name="area">�V���h�E�}�b�v�͈�</param>
	/// <param name="forcusPoint">�V���h�E�}�b�v�̒��S���W</param>
	void Bake(int x_size, int y_size, const CVector3& lightDir, const CVector3& area, const CVector3& forcusPoint);
	
	/// <summary>
	/// �x�C�N�ς݂�?
	/// </summary>
	bool GetIsBaked()const { return m_isBaked; }

private:
	bool m_isBaked = false;
	ShadowMapHandler m_shadowMap;
};

