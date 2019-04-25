#pragma once
class Camera : public IGameObject
{
public:
	Camera();
	~Camera();

	void Update();

private:
	GameObj::PerspectiveCamera m_cam;
	CVector3 m_pos, m_target;
};

