#include "stdafx.h"
#include "IGamePad.h"

namespace {
	//回転入力各方向
	static const CVector3 SpinDirection[4] = {
		   CVector3(0.0f,1.0f,0.0f),//上
		   CVector3(1.0f,0.0f,0.0f),//右
		   CVector3(0.0f,-1.0f,0.0f),//下
		   CVector3(-1.0f,0.0f,0.0f)//左
	};
}

void IGamePad::PreUpdate() {
	//ダブルタップ判定
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
		//ダブルタップ猶予時間処理
		if (m_doubleTapFirePhase[lr] > 0) {
			m_doubleTapFireTimer[lr]++;
			if (m_doubleTapFireTimer[lr] >= FIRE_DOUBLE_TAP_TIME) {
				m_doubleTapFirePhase[lr] = 0;
				m_doubleTapFireTimer[lr] = 0;
			}
		}
	}

	//回転入力判定
	int i = 0;
	for (auto& lr : m_circleInputStatus) {
		CVector2 stick = GetStick(i ? R : L);
		float length = stick.LengthSq();//入力距離
		stick.Normalize();//入力ベクトルの正規化

		//一回転前回転数更新
		if (lr.m_spinCountBefore + enDirNum <= lr.m_spinCount) {
			lr.m_spinCountBefore += enDirNum;
		}

		//入力猶予切れ
		lr.m_inputTimer -= 1.0f;
		if (lr.m_inputTimer < FLT_EPSILON) {
			//初期化
			lr.m_nextDirection = enDirNum; lr.m_backDirection = enDirNum; lr.m_spinMode = enNone;
			lr.m_inputTimer = 0.0f; lr.m_spinCount = 0; lr.m_spinCountBefore = 0;
		}

		//一定以上の入力距離
		if (length > CMath::Square(CIRCLE_INPUT_DEADZONE)) {
			if (lr.m_nextDirection == enDirNum) {//最初の入力				
				float angle = 10000.0f;
				for (int i = 0; i < enDirNum; i++) {
					float angle2 = CVector3::AngleOf2NormalizeVector(SpinDirection[i], { stick.x,stick.y,0.0f });
					if (angle > angle2) {
						angle = angle2;
						//次に入力するべき方向を設定
						lr.m_nextDirection = (enSpinDirection)((i + 1 == enDirNum) ? 0 : i + 1);
						lr.m_backDirection = (enSpinDirection)((i - 1 < 0) ? enDirNum - 1 : i - 1);
					}
				}
				lr.m_spinCount = 1;		//回転カウント初期化
				lr.m_inputTimer = CIRCLE_INPUT_MAX_TIME;	//入力猶予設定
				lr.m_spinMode = enNone;	//回転方向初期化
			}
			else {//最初でない入力
				enSpinDirection isReflesh = enDirNum;//次に入力する方向を更新するか?
				float angle;

				//時計回り
				angle = abs(CVector3::AngleOf2NormalizeVector(SpinDirection[lr.m_nextDirection], { stick.x,stick.y,0.0f }));
				if (angle < CIRCLE_INPUT_RANGE) {
					isReflesh = lr.m_nextDirection;
					if (lr.m_spinMode == enBack) { //回転方向が変わったらカウントリセット
						lr.m_spinCount = 0; lr.m_spinCountBefore = 0;
					}else {
						lr.m_spinCount++;
					}
					lr.m_inputTimer = CIRCLE_INPUT_MAX_TIME;
					lr.m_spinMode = enClockwise;
				}
				//反時計回り
				angle = abs(CVector3::AngleOf2NormalizeVector(SpinDirection[lr.m_backDirection], { stick.x,stick.y,0.0f }));
				if (angle < CIRCLE_INPUT_RANGE) {
					isReflesh = lr.m_backDirection;
					if (lr.m_spinMode == enClockwise) { //回転方向が変わったらカウントリセット
						lr.m_spinCount = 0; lr.m_spinCountBefore = 0;
					}else {
						lr.m_spinCount++;
					}
					lr.m_inputTimer = CIRCLE_INPUT_MAX_TIME;
					lr.m_spinMode = enBack;
				}
				//次に入力する方向を更新
				if (isReflesh != enDirNum) {
					lr.m_nextDirection = (enSpinDirection)((isReflesh + 1 == enDirNum) ? 0 : isReflesh + 1);
					lr.m_backDirection = (enSpinDirection)((isReflesh - 1 < 0) ? enDirNum - 1 : isReflesh - 1);
				}
			}
		}	
		i++;
	}
}
