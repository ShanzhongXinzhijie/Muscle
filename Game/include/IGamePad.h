#pragma once

/// <summary>
/// ������̓N���X
/// </summary>
class IGamePad : public IGameObject
{
public:
	IGamePad(int num) {
		m_usePad = &Pad(num);
	}
	virtual ~IGamePad() {}

	void PreUpdate()override;

	/// <summary>
	/// �X�e�B�b�N���͂��擾
	/// </summary>
	/// <param name="lr">���E</param>
	[[nodiscard]] 
	const CVector2& GetStick(enLR lr) const {
		return m_usePad->GetStick(lr);
	}

	/// <summary>
	/// ���̃t���[���ɒe�����͂��ꂽ��
	/// </summary>
	/// <param name="lr">���E</param>
	[[nodiscard]]
	bool IsSmashInput(enLR lr)const {
		return m_isSmash[lr];
	}

	/// <summary>
	/// �X�e�B�b�N���͂̑O�t���[���Ƃ̍��p�x���擾
	/// </summary>
	/// <param name="lr">���E</param>
	[[nodiscard]]
	float GetStickRollAngle(enLR lr) const {
		return m_stickRollAngle[lr];
	}

	//��]���͌n
	enum enSpinDirection { enUp, enRight, enDown, enLeft, enDirNum, };//���͊e����
	enum enSpinMode { enNone, enClockwise, enBack, };//��]����

	/// <summary>
	/// �X�e�B�b�N��]�����擾
	/// </summary>
	/// <param name="lr">���E</param>
	[[nodiscard]]
	int GetStickCircleInput(enLR lr) const {
		return m_circleInputStatus[lr].m_spinCount / enDirNum;
	}

	/// <summary>
	/// ���̃t���[���ŃX�e�B�b�N��1��]������?
	/// </summary>
	/// <param name="lr">���E</param>
	[[nodiscard]]
	bool GetIsStickCircleInputNow(enLR lr) const {
		return m_circleInputStatus[lr].m_spinCountBefore + enDirNum <= m_circleInputStatus[lr].m_spinCount;
	}

	/// <summary>
	/// �X�e�B�b�N�̉�]����(���v���or�����v���)���擾
	/// </summary>
	/// <param name="lr">���E</param>
	[[nodiscard]]
	enSpinMode GetStickCircleenSpinMode(enLR lr) const {
		return m_circleInputStatus[lr].m_spinMode;
	}

	/// <summary>
	/// �ˌ����͂��擾
	/// </summary>
	/// <param name="lr">���E</param>
	[[nodiscard]] 
	bool GetFire(enLR lr) const {
		bool isFire = m_usePad->GetButton((lr == L) ? enButtonLB : enButtonRB);
#ifndef DW_MASTER
		if (!isFire) {
			isFire = GetKeyInput((lr == L) ? VK_LBUTTON : VK_RBUTTON);
		}
#endif // DW_MASTER
		return isFire;
	}
	/// <summary>
	/// �ˌ��_�u���^�b�v���͂��擾
	/// </summary>
	/// <param name="lr">���E</param>
	[[nodiscard]]
	bool GetDoubleTapFire(enLR lr) const{
		return m_isDoubleTapFire[lr];
	}

	/// <summary>
	/// �����͂��擾
	/// </summary>
	[[nodiscard]]
	bool GetWingInput() const {
		return m_usePad->GetButton(enButtonLSB);
	}

	/// <summary>
	/// �o�b�N�~���[���͂��擾
	/// </summary>
	[[nodiscard]]
	bool GetBackMirrorDown() const {
		return m_usePad->GetDown(enButtonRSB);
	}

	/// <summary>
	/// �r���͂��擾
	/// </summary>
	[[nodiscard]]
	bool GetLegInput() const {
		return m_usePad->GetButton(enButtonLT) || m_usePad->GetButton(enButtonRT);
	}
	[[nodiscard]]
	bool GetLegDown() const {
		return m_usePad->GetDown(enButtonLT) || m_usePad->GetDown(enButtonRT);
	}
	//���E�ǂ��炩
	[[nodiscard]]
	bool GetLegInput(enLR lr) const {
		return m_usePad->GetButton(lr == L ? enButtonLT : enButtonRT);
	}
	//��������
	[[nodiscard]]
	bool GetDoubleLegDown() const {
		return (m_usePad->GetDown(enButtonLT) || m_usePad->GetDown(enButtonRT)) && (m_usePad->GetButton(enButtonLT) && m_usePad->GetButton(enButtonRT));
	}

private:
	XInputPad* m_usePad = nullptr;//�g�p����p�b�h

	//�e������
	bool m_isSmash[enLRNUM] = {};
	int m_smashInputStartTime[enLRNUM] = {};
	static constexpr float SMASH_INPUT_DEADZONE = 0.24f;//�f�b�h�]�[��
	static constexpr int SMASH_INPUT_TIME = 4;//���͗P�\

	//�p�x���͌n
	float m_stickRollAngle[enLRNUM] = {};//�X�e�B�b�N�̊p�x����
	bool m_isInitBeforeStickInput = false;
	CVector2 m_beforeStickInput[enLRNUM];//�O�t���[���̃X�e�B�b�N����
	static constexpr float ANGLE_INPUT_DEADZONE = 0.75f;//�f�b�h�]�[��

	//�_�u���^�b�v�n
	bool m_isDoubleTapFire[enLRNUM] = {};
	int m_doubleTapFirePhase[enLRNUM] = {};
	int m_doubleTapFireTimer[enLRNUM] = {};
	static constexpr int FIRE_DOUBLE_TAP_TIME = 20;//���͗P�\

	//��]���͊֌W
	struct CircleInputStatus {
		float m_inputTimer = 0.0f;//���͗P�\
		int m_spinCount = 0, m_spinCountBefore = 0;//��]�J�E���g	
		enSpinDirection m_nextDirection = enDirNum;//���ɓ��͂����]
		enSpinDirection m_backDirection = enDirNum;//���ɓ��͂���t��]	
		enSpinMode m_spinMode = enNone;//��]���[�h
	};
	CircleInputStatus m_circleInputStatus[enLRNUM];
	static constexpr float CIRCLE_INPUT_DEADZONE = 0.75f;//�f�b�h�]�[��
	static constexpr float CIRCLE_INPUT_RANGE = CMath::PI2 / enDirNum / 2.0f;//�e�����̓��͔͈�
	static constexpr float CIRCLE_INPUT_MAX_TIME = 30.0f;//���͗P�\
};

