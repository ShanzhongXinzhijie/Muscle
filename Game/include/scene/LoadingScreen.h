#pragma once

/// <summary>
/// ロード画面
/// 引数の処理を実行してロード画面を表示する
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

