#pragma once
#include "PartsList.h"

class IScene : public IGameObject {
public:
	virtual ~IScene() {}
protected:
	CFont m_font;
};

class CDeathHotoke;

/// <summary>
/// ��D�N���X
/// </summary>
class CHand {
public:

private:
	std::vector<PartType> m_hand;
};

/// <summary>
/// �A�Z���u�����
/// </summary>
class SceneAssemble : public IScene
{
public:
	SceneAssemble() = default;

	bool Start()override;
	void Update()override;
	void PostRender()override;

private:
	//std::unique_ptr<CHand> m_tefuda;
	std::unique_ptr<CDeathHotoke> m_hotoke;
};

/// <summary>
/// ��D�I����
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
/// �A�N�V�����I�����
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

class UIBox {
public:
	void Init();

	const CVector2& GetPos()const;
	
	void HitTest(const CVector2& testPos);
	void Drag(const CVector2& cursorPos);
	void Drop();

	void Draw();//HUD�ɕ`�� �v���~�e�B�u��

private:
	DW_GETSETCONSREF(CVector2, m_pos, Pos)
	DW_GETSETCONSREF(CVector2, m_size, Size)
};