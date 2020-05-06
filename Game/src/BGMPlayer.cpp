#include "stdafx.h"
#include "BGMPlayer.h"

void BGMPlayer::Play(std::wstring_view path) {

	if (m_playingPath == path) {
		return;//���łɍĐ���
	}

	//BGM�쐬
	m_bgm = std::make_unique<SuicideObj::CBGM>(path.data());
	//�����ō폜���Ȃ�
	m_bgm->SetIsAutoDelete(false);
	//���[�v�ōĐ�
	m_bgm->Play(false, true); 

	//�Đ����̃p�X��ۑ�
	m_playingPath = path;

	//bgm->Stop(); //�Đ����~�߂�(���̌�폜�����)
	//bgm->Pause(); //�ꎞ��~�BPlay()�ōĐ��ĊJ(�����͈Ӗ��Ȃ�)
}