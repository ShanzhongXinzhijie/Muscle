#pragma once

class IScene : public IGameObject {
public:
	virtual ~IScene() {}
protected:
	CFont m_font;
};

class CDeathHotoke;

/// <summary>
/// アセンブル画面
/// </summary>
class SceneAssemble : public IScene
{
public:
	SceneAssemble() = default;

	bool Start()override;
	void Update()override;
	void PostRender()override;

private:
	std::unique_ptr<CHand> m_tefuda;
	std::unique_ptr<CDeathHotoke> m_hotoke;
};

/// <summary>
/// 手札選定画面
/// </summary>
class SceneMulligan : public IScene
{
public:
	SceneMulligan() = default;

	bool Start()override;
	void Update()override;
	void PostRender()override;

private:
	std::unique_ptr<CHand> m_tefuda;
};

/// <summary>
/// アクション選択画面
/// </summary>
class SceneCast : public IScene
{
public:
	SceneCast() = default;

	bool Start()override;
	void Update()override;
	void PostRender()override;

private:
};