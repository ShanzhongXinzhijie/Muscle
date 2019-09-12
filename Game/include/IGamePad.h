#pragma once

/// <summary>
/// ������̓N���X
/// </summary>
class IGamePad : public IGameObject
{
public:
	IGamePad() = default;
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
		if (!isFire) {
			isFire = GetKeyInput((lr == L) ? VK_LBUTTON : VK_RBUTTON);
		}
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
	/// �o�b�N�~���[���͂��擾
	/// </summary>
	[[nodiscard]]
	bool GetBackMirror() const {
		return m_usePad->GetButton(enButtonRSB);
	}

	/// <summary>
	/// �r���͂��擾
	/// </summary>
	[[nodiscard]]
	bool GetLegInput() const {
		return m_usePad->GetButton(enButtonLT);
	}
	[[nodiscard]]
	bool GetLegDown() const {
		return m_usePad->GetDown(enButtonLT);
	}

private:
	XInputPad* m_usePad = &Pad(0);//�g�p����p�b�h

	//�_�u���^�b�v�n
	bool m_isDoubleTapFire[enLRNUM] = {};
	int m_doubleTapFirePhase[enLRNUM] = {};
	int m_doubleTapFireTimer[enLRNUM] = {};
	static constexpr int FIRE_DOUBLE_TAP_TIME = 10;

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

