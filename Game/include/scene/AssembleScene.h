#pragma once

#include"CDeathHotoke.h"

class AssembleScene :
	public IGameObject
{
public:
	AssembleScene() = default;

	bool Start()override;

	void Update()override;
	void PostUpdate()override;

	//void PostLoopUpdate()override;
	void HUDRender(int)override;
	//void PostRender()override;

private:
	bool m_isReady = false;
	int m_nowEditBody[PLAYER_NUM] = {};
	int m_nowBodyType[PLAYER_NUM][CDeathHotoke::enPartsNum] = {};

	CFont m_font, m_fontBold;
	CDeathHotoke m_hotokes[PLAYER_NUM];
	GameObj::PerspectiveCamera m_cameras[PLAYER_NUM];
	GameObj::CDirectionLight m_light;

	std::vector<std::unique_ptr<IBodyPart>> m_parts[CDeathHotoke::enPartsNum];
};

