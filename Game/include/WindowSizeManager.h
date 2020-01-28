#pragma once

class WindowSizeManager :
	public IGameObject
{
public:
	void ChangeWindowSize(bool isTwoScreen);

	bool Start()override;

	void Update()override;

	void PostLoopUpdate()override;
	void PostRender()override;

private:
	int m_windowSize = WINDOW_SIZE;
	int m_3dScreenSize = SCREEN_SIZE_3D;

	bool m_isEnable = false;
};

