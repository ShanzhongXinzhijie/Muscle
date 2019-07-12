#include "stdafx.h"
#include "IGamePad.h"

void IGamePad::PreUpdate() {
	//�_�u���^�b�v����
	for (auto lr : { L,R }) {
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
}
