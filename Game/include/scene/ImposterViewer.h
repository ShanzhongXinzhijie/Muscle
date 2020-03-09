#pragma once

class ImposterViewer :	public IGameObject
{
public:
	bool Start()override;
	//void Update()override;
	void PostLoopUpdate()override;
	void PostRender()override;

private:
	GameObj::CSkinModelRender m_model;
	CImposter m_imposter;
	CSprite m_sprite;
	GameObj::PerspectiveCamera m_camera;
};

