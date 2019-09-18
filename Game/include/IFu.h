#pragma once

/// <summary>
/// ゲームフィールド上に存在するものを表すインターフェースクラス
/// </summary>
class IFu 
{
public:

private:
	CVector3 m_pos,m_scale;
	CQuaternion m_rot;

	GameObj::CSkinModelRender m_model;

	DHCollision m_col;
};

