#include "stdafx.h"
#include "IGamePad.h"

namespace {
	//��]���͊e����
	static const CVector3 SpinDirection[4] = {
		   CVector3(0.0f,1.0f,0.0f),//��
		   CVector3(1.0f,0.0f,0.0f),//�E
		   CVector3(0.0f,-1.0f,0.0f),//��
		   CVector3(-1.0f,0.0f,0.0f)//��
	};
}

void IGamePad::PreUpdate() {
	//�_�u���^�b�v����
	for (auto lr : LR) {
		m_isDoubleTapFire[lr] = false;
		
		if (GetFire(lr)) {
			if (m_doubleTapFirePhase[lr] == 0) {
				m_doubleTapFirePhase[lr]++;
				m_doubleTapFireTimer[lr] = 0;
			}
			if (m_doubleTapFirePhase[lr] == 2) {
				m_isDoubleTapFire[lr] = true;
				m_doubleTapFirePhase[lr] = 0;
			}
		}
		else {
			if (m_doubleTapFirePhase[lr] == 1) {
				m_doubleTapFirePhase[lr]++;
			}
		}
		//�_�u���^�b�v�P�\���ԏ���
		if (m_doubleTapFirePhase[lr] > 0) {
			m_doubleTapFireTimer[lr]++;
			if (m_doubleTapFireTimer[lr] >= FIRE_DOUBLE_TAP_TIME) {
				m_doubleTapFirePhase[lr] = 0;
				m_doubleTapFireTimer[lr] = 0;
			}
		}
	}

	//��]���͔���
	int i = 0;
	for (auto& lr : m_circleInputStatus) {
		CVector2 stick = GetStick(i ? R : L);
		float length = stick.LengthSq();//���͋���
		stick.Normalize();//���̓x�N�g���̐��K��

		//���]�O��]���X�V
		if (lr.m_spinCountBefore + enDirNum <= lr.m_spinCount) {
			lr.m_spinCountBefore += enDirNum;
		}

		//���͗P�\�؂�
		lr.m_inputTimer -= 1.0f;
		if (lr.m_inputTimer < FLT_EPSILON) {
			//������
			lr.m_nextDirection = enDirNum; lr.m_backDirection = enDirNum; lr.m_spinMode = enNone;
			lr.m_inputTimer = 0.0f; lr.m_spinCount = 0; lr.m_spinCountBefore = 0;
		}

		//���ȏ�̓��͋���
		if (length > CMath::Square(CIRCLE_INPUT_DEADZONE)) {
			if (lr.m_nextDirection == enDirNum) {//�ŏ��̓���				
				float angle = 10000.0f;
				for (int i = 0; i < enDirNum; i++) {
					float angle2 = CVector3::AngleOf2NormalizeVector(SpinDirection[i], { stick.x,stick.y,0.0f });
					if (angle > angle2) {
						angle = angle2;
						//���ɓ��͂���ׂ�������ݒ�
						lr.m_nextDirection = (enSpinDirection)((i + 1 == enDirNum) ? 0 : i + 1);
						lr.m_backDirection = (enSpinDirection)((i - 1 < 0) ? enDirNum - 1 : i - 1);
					}
				}
				lr.m_spinCount = 1;		//��]�J�E���g������
				lr.m_inputTimer = CIRCLE_INPUT_MAX_TIME;	//���͗P�\�ݒ�
				lr.m_spinMode = enNone;	//��]����������
			}
			else {//�ŏ��łȂ�����
				enSpinDirection isReflesh = enDirNum;//���ɓ��͂���������X�V���邩?
				float angle;

				//���v���
				angle = abs(CVector3::AngleOf2NormalizeVector(SpinDirection[lr.m_nextDirection], { stick.x,stick.y,0.0f }));
				if (angle < CIRCLE_INPUT_RANGE) {
					isReflesh = lr.m_nextDirection;
					if (lr.m_spinMode == enBack) { //��]�������ς������J�E���g���Z�b�g
						lr.m_spinCount = 0; lr.m_spinCountBefore = 0;
					}else {
						lr.m_spinCount++;
					}
					lr.m_inputTimer = CIRCLE_INPUT_MAX_TIME;
					lr.m_spinMode = enClockwise;
				}
				//�����v���
				angle = abs(CVector3::AngleOf2NormalizeVector(SpinDirection[lr.m_backDirection], { stick.x,stick.y,0.0f }));
				if (angle < CIRCLE_INPUT_RANGE) {
					isReflesh = lr.m_backDirection;
					if (lr.m_spinMode == enClockwise) { //��]�������ς������J�E���g���Z�b�g
						lr.m_spinCount = 0; lr.m_spinCountBefore = 0;
					}else {
						lr.m_spinCount++;
					}
					lr.m_inputTimer = CIRCLE_INPUT_MAX_TIME;
					lr.m_spinMode = enBack;
				}
				//���ɓ��͂���������X�V
				if (isReflesh != enDirNum) {
					lr.m_nextDirection = (enSpinDirection)((isReflesh + 1 == enDirNum) ? 0 : isReflesh + 1);
					lr.m_backDirection = (enSpinDirection)((isReflesh - 1 < 0) ? enDirNum - 1 : isReflesh - 1);
				}
			}
		}	
		i++;
	}
}
