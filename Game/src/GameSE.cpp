#include"stdafx.h"
#include"GameSE.h"
#include"CGameMode.h"
#include"Game.h"

GameSE::GameSE(const wchar_t* fileName, const CVector3& pos, float distanceMeter, int screenNum, bool isLoop, bool mute)
	: SuicideObj::CSE(fileName)
{
	if (m_s_gameMode->GetPlayerNum() == 1 || !m_s_game) {
		SetPos(pos);
		SetDistance(distanceMeter*METER);
		m_is3D = true;
	}
	else {
		//FrontLeft, FrontRight, FrontCenter, LowFrequency, BackLeft, BackRight, SideLeft, SideRight
		SetOutChannelVolume(2, 0.5f);
		SetOutChannelVolume(3, 0.5f);

		if (screenNum < 0) {
			//ãóó£Ç©ÇÁ
			int nearPlayerNum = 0;
			float nearDistance = 0.0f;
			for (int i = 0; i < m_s_gameMode->GetPlayerNum(); i++) {
				float distanceSq = (m_s_game->GetPlayer(i)->GetDeathHotoke().GetPos() - pos).LengthSq();
				if (i == 0 || nearDistance > distanceSq) {
					nearPlayerNum = i;
					nearDistance = distanceSq;
				}
			}
			screenNum = nearPlayerNum;
		}

		if (screenNum == 0) {
			//ç∂Ç©ÇÁñ¬ÇÁÇ∑
			SetOutChannelVolume(1, 0.5f);
			SetOutChannelVolume(5, 0.5f);
			SetOutChannelVolume(7, 0.5f);
		}
		else {
			//âEÇ©ÇÁñ¬ÇÁÇ∑
			SetOutChannelVolume(0, 0.5f);
			SetOutChannelVolume(4, 0.5f);
			SetOutChannelVolume(6, 0.5f);
		}
		m_is3D = false;
	}
	if (mute) {
		SetVol(0.0f);
	}
	Play(m_is3D, isLoop);
}