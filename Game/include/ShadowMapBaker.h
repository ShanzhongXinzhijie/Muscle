#pragma once
class ShadowMapBaker
{
public:
	ShadowMapBaker() = default;
	
	/// <summary>
	/// 描画するモデル追加
	/// </summary>
	/// <param name="caster">描画するモデル</param>
	/// <param name="isCasterSetUnable">設定したモデルのシャドウキャスターを今後オフにするか</param>
	void AddDrawModel(GameObj::CSkinModelRender& caster, bool isCasterSetUnable = true);
	
	/// <summary>
	/// シャドウマップのベイク
	/// </summary>
	/// <param name="x_size">横解像度</param>
	/// <param name="y_size">縦解像度</param>
	/// <param name="lightDir">ライト方向</param>
	/// <param name="area">シャドウマップ範囲</param>
	/// <param name="forcusPoint">シャドウマップの中心座標</param>
	void Bake(int x_size, int y_size, const CVector3& lightDir, const CVector3& area, const CVector3& forcusPoint);
	
	/// <summary>
	/// ベイク済みか?
	/// </summary>
	bool GetIsBaked()const { return m_isBaked; }

private:
	bool m_isBaked = false;
	ShadowMapHandler m_shadowMap;
};

