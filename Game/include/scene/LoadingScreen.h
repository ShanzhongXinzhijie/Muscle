#pragma once

/// <summary>
/// ���[�h���
/// �����̏��������s���ă��[�h��ʂ�\������
/// </summary>
class LoadingScreen : public IGameObject
{
public:
	LoadingScreen(std::function<void()> func) :m_func(func) {};

	bool Start()override;
	void PostLoopUpdate()override;
	void PostRender()override;

private:
	void Load() {
		m_func();
	}

private:
	bool m_isOneLooped = false;

	CFont m_font;
	CSprite m_sprite;

	std::function<void()> m_func;
};

