#pragma once

/// <summary>
/// �Q�[���t�B�[���h��ɑ��݂�����̂�\���C���^�[�t�F�[�X�N���X
/// </summary>
class IFu 
{
public:

private:
	CVector3 m_pos,m_scale;
	CQuaternion m_rot;

	GameObj::CSkinModelRender m_model;

	DHCollision m_col;
};

