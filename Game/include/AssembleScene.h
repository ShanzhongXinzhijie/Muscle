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

	void PostLoopUpdate()override;
	void HUDRender(int)override;
	void PostRender()override;

private:
	bool m_isReady = false;
	int m_nowEditBody = 0, ;

	CFont m_font, m_fontBold;
	CDeathHotoke m_hotokes[PLAYER_NUM];

	std::vector<std::unique_ptr<IBodyPart>> m_parts[CDeathHotoke::enPartsNum];
};

