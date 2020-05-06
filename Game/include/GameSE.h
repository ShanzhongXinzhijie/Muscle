#pragma once

class CGameMode;
class Game;

/// <summary>
/// �Q�[�����ʉ�
/// </summary>
class GameSE : private SuicideObj::CSE
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="fileName">wav�t�@�C���p�X</param>
	/// <param name="pos">�Đ��ʒu</param>
	/// <param name="distanceMeter">�Đ�����(m)</param>
	/// <param name="screenNum">�Đ��X�N���[���ԍ�</param>
	/// <param name="isLoop">���[�v���邩</param>
	/// <param name="mute">�~���[�g���邩</param>
	GameSE(const wchar_t* fileName, const CVector3& pos, float distanceMeter, int screenNum, bool isLoop = false, bool mute = false);
	
	/// <summary>
	/// �Đ��I���㎩�E���邩�ݒ�
	/// </summary>
	void SetIsAutoDelete(bool isAutoDel) {
		SuicideObj::CSE::SetIsAutoDelete(isAutoDel);
	}

	/// <summary>
	/// �Đ��ʒu�ݒ�
	/// </summary>
	void SetPos(const CVector3& pos) {
		SuicideObj::CSE::SetPos(pos);
	}

	/// <summary>
	/// ���ʐݒ�
	/// </summary>
	void SetVol(float vol) {
		SuicideObj::CSE::SetVolume(vol);
	}

	/// <summary>
	/// �Đ������擾
	/// </summary>
	bool GetIsPlaying()const { 
		return SuicideObj::CSE::GetIsPlaying();
	}

	/// <summary>
	/// �Q�[�����[�h�ւ̎Q�Ƃ�ݒ�
	/// </summary>
	static void SetGameModePtr(const CGameMode* gamemode) {
		m_s_gameMode = gamemode;
	}
	/// <summary>
	/// �Q�[���ւ̎Q�Ƃ�ݒ�
	/// </summary>
	static void SetGamePtr(const Game* game) {
		m_s_game = game;
	}

private:
	bool m_is3D = false;

	static inline const CGameMode* m_s_gameMode = nullptr;
	static inline const Game* m_s_game = nullptr;
};

